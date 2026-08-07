/*
File:   gy_word_tree.h
Author: Taylor Robbins
Date:   05\10\2022
Description:
	** A "Word Tree" is a tree where each node contains up to 26 children (one slot for every letter a-z)
	** This is often useful for quick string to value lookup (as long as that string contains only alphabetic characters)
	** These trees are generally optimized for traversal, not creation, so some manipulating processes might be slower based on our implementation choices
	** Word Trees are case insensitive. ASCII Tree is an alternative to word tree that is case sensitive and supports 0x32-0x7E ASCII characters
*/

#ifndef _GY_WORD_TREE_H
#define _GY_WORD_TREE_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"
#include "gy_string.h"
#include "gy_variable_array.h"

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define WORD_TREE_CHILD_COUNT 26
#define WORD_TREE_FIRST_CHAR  'a' //'a' through 'z'

#define ASCII_TREE_CHILD_COUNT 95
#define ASCII_TREE_FIRST_CHAR  ' ' //space through '~'

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct WordTreeNode_t
{
	WordTreeNode_t* children[WORD_TREE_CHILD_COUNT];
};
union WordTreeLeaf_t
{
	u64 value64;
	void* valuePntr;
};
struct WordTree_t
{
	MemArena_t* allocArena;
	VarArray_t leafs; //WordTreeLeaf_t
	VarArray_t nodes; //WordTreeNode_t
};

//TODO: Implement an ASCII tree
#if 0
struct AsciiTree_t
{
	//TODO: Implement me!
};
#endif

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeWordTree(WordTree_t* tree);
	void CreateWordTree(WordTree_t* tree, MemArena_t* memArena, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8);
	WordTreeNode_t* WordTreeAddNode(WordTree_t* tree, WordTreeNode_t* node, char childChar, WordTreeLeaf_t** leafOut);
	WordTreeLeaf_t* WordTreeAddLeaf(WordTree_t* tree, MyStr_t word);
	WordTreeLeaf_t* WordTreeAddU64(WordTree_t* tree, MyStr_t word, u64 value64);
	WordTreeLeaf_t* WordTreeAddPntr(WordTree_t* tree, MyStr_t word, void* valuePntr);
	WordTreeLeaf_t* WordTreeGetLeaf(WordTree_t* tree, MyStr_t word);
	u64 WordTreeGetU64(WordTree_t* tree, MyStr_t word);
	void* WordTreeGetPntr(WordTree_t* tree, MyStr_t word);
#else

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void FreeWordTree(WordTree_t* tree)
{
	NotNull(tree);
	FreeVarArray(&tree->leafs);
	FreeVarArray(&tree->nodes);
	ClearPointer(tree);
}

void CreateWordTree(WordTree_t* tree, MemArena_t* memArena, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8)
{
	NotNull(tree);
	NotNull(memArena);
	ClearPointer(tree);
	tree->allocArena = memArena;
	CreateVarArray(&tree->leafs, memArena, sizeof(WordTreeLeaf_t), initialRequiredCapacity, exponentialChunkSize, allocChunkSize);
	CreateVarArray(&tree->nodes, memArena, sizeof(WordTreeNode_t), initialRequiredCapacity, exponentialChunkSize, allocChunkSize);
	WordTreeLeaf_t* rootLeaf = VarArrayAdd(&tree->leafs, WordTreeLeaf_t);
	WordTreeNode_t* rootNode = VarArrayAdd(&tree->nodes, WordTreeNode_t);
	NotNull2(rootLeaf, rootNode);
	ClearPointer(rootLeaf);
	ClearPointer(rootNode);
}

WordTreeNode_t* WordTreeAddNode(WordTree_t* tree, WordTreeNode_t* node, char childChar, WordTreeLeaf_t** leafOut)
{
	NotNull(tree);
	NotNull(node);
	
	if (childChar >= 'A' && childChar <= 'Z')
	{
		childChar = 'a' + (childChar - 'A');
	}
	AssertMsg(childChar >= WORD_TREE_FIRST_CHAR, "Non-alphabetic character in WordTreeAdd");
	u8 childIndex = CharToU8(childChar) - WORD_TREE_FIRST_CHAR;
	AssertMsg(childIndex < WORD_TREE_CHILD_COUNT, "Non-alphabetic character in WordTreeAdd");
	
	if (node->children[childIndex] != nullptr)
	{
		WordTreeNode_t* result = node->children[childIndex];
		if (leafOut != nullptr)
		{
			u64 nodeIndex = (u64)((u8*)result - ((u8*)tree->nodes.items)) / sizeof(WordTreeNode_t);
			*leafOut = VarArrayGet(&tree->leafs, nodeIndex, WordTreeLeaf_t);
		}
		return result;
	}
	
	void* oldNodesBase = tree->nodes.items;
	WordTreeLeaf_t* newLeaf = VarArrayAdd(&tree->leafs, WordTreeLeaf_t);
	WordTreeNode_t* newNode = VarArrayAdd(&tree->nodes, WordTreeNode_t);
	NotNull2(newLeaf, newNode);
	ClearPointer(newLeaf);
	ClearPointer(newNode);
	
	//Fix node children pointers if VarArray realloced it's items
	if (tree->nodes.items != oldNodesBase)
	{
		node = (WordTreeNode_t*)((u8*)node - (u8*)oldNodesBase + (u8*)tree->nodes.items);
		VarArrayLoop(&tree->nodes, nIndex)
		{
			VarArrayLoopGet(WordTreeNode_t, fixupNode, &tree->nodes, nIndex);
			for (u8 cIndex = 0; cIndex < WORD_TREE_CHILD_COUNT; cIndex++)
			{
				if (fixupNode->children[cIndex] != nullptr)
				{
					fixupNode->children[cIndex] = (WordTreeNode_t*)(((u8*)fixupNode->children[cIndex]) - (u8*)oldNodesBase + (u8*)tree->nodes.items);
				}
			}
		}
	}
	
	node->children[childIndex] = newNode;
	
	if (leafOut != nullptr) { *leafOut = newLeaf; }
	return newNode;
}

WordTreeLeaf_t* WordTreeAddLeaf(WordTree_t* tree, MyStr_t word)
{
	NotNull(tree);
	NotNullStr(&word);
	if (word.length == 0) { return nullptr; }
	
	WordTreeNode_t* node = VarArrayGet(&tree->nodes, 0, WordTreeNode_t);
	for (u64 charIndex = 0; charIndex < word.length; charIndex++)
	{
		WordTreeLeaf_t* newLeaf = nullptr;
		WordTreeNode_t* newNode = WordTreeAddNode(tree, node, word.pntr[charIndex], &newLeaf);
		if (charIndex+1 == word.length || newNode == nullptr || newLeaf == nullptr)
		{
			return newLeaf;
		}
		node = newNode;
	}
	
	AssertMsg(false, "Reached the end of WordTreeAddLeaf which should be impossible");
	return nullptr;
}

WordTreeLeaf_t* WordTreeAddU64(WordTree_t* tree, MyStr_t word, u64 value64)
{
	WordTreeLeaf_t* newLeaf = WordTreeAddLeaf(tree, word);
	if (newLeaf != nullptr) { newLeaf->value64 = value64; }
	return newLeaf;
}
WordTreeLeaf_t* WordTreeAddPntr(WordTree_t* tree, MyStr_t word, void* valuePntr)
{
	WordTreeLeaf_t* newLeaf = WordTreeAddLeaf(tree, word);
	if (newLeaf != nullptr) { newLeaf->valuePntr = valuePntr; }
	return newLeaf;
}

WordTreeLeaf_t* WordTreeGetLeaf(WordTree_t* tree, MyStr_t word)
{
	NotNull(tree);
	NotNullStr(&word);
	if (word.length == 0) { return nullptr; }
	
	WordTreeNode_t* node = VarArrayGet(&tree->nodes, 0, WordTreeNode_t);
	for (u64 charIndex = 0; charIndex < word.length && node != nullptr; charIndex++)
	{
		char nextChar = word.pntr[charIndex];
		if (nextChar >= 'A' && nextChar <= 'Z') { nextChar = 'a' + (nextChar - 'A'); }
		if (nextChar < WORD_TREE_FIRST_CHAR) { return nullptr; }
		u8 childIndex = CharToU8(nextChar) - WORD_TREE_FIRST_CHAR;
		if (childIndex >= WORD_TREE_CHILD_COUNT) { return nullptr; }
		node = node->children[childIndex];
	}
	
	if (node == nullptr) { return nullptr; }
	u64 nodeIndex = (u64)(node - ((WordTreeNode_t*)tree->nodes.items));
	WordTreeLeaf_t* leaf = VarArrayGet(&tree->leafs, nodeIndex, WordTreeLeaf_t);
	return leaf;
}
u64 WordTreeGetU64(WordTree_t* tree, MyStr_t word)
{
	WordTreeLeaf_t* leaf = WordTreeGetLeaf(tree, word);
	NotNull(leaf);
	return leaf->value64;
}
void* WordTreeGetPntr(WordTree_t* tree, MyStr_t word)
{
	WordTreeLeaf_t* leaf = WordTreeGetLeaf(tree, word);
	NotNull(leaf);
	return leaf->valuePntr;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_WORD_TREE_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
WORD_TREE_CHILD_COUNT
WORD_TREE_FIRST_CHAR
ASCII_TREE_CHILD_COUNT
ASCII_TREE_FIRST_CHAR
@Types
WordTreeNode_t
WordTreeLeaf_t
WordTree_t
AsciiTree_t
@Functions
void FreeWordTree(WordTree_t* tree)
void CreateWordTree(WordTree_t* tree, MemArena_t* memArena, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8)
WordTreeNode_t* WordTreeAddNode(WordTree_t* tree, WordTreeNode_t* node, char childChar, WordTreeLeaf_t** leafOut)
WordTreeLeaf_t* WordTreeAddLeaf(WordTree_t* tree, MyStr_t word)
WordTreeLeaf_t* WordTreeAddU64(WordTree_t* tree, MyStr_t word, u64 value64)
WordTreeLeaf_t* WordTreeAddPntr(WordTree_t* tree, MyStr_t word, void* valuePntr)
WordTreeLeaf_t* WordTreeGetLeaf(WordTree_t* tree, MyStr_t word)
u64 WordTreeGetU64(WordTree_t* tree, MyStr_t word)
void* WordTreeGetPntr(WordTree_t* tree, MyStr_t word)
*/
