/*
File:   gy_triangulation.h
Author: Taylor Robbins
Date:   01\24\2022
Description:
	** Holds functions that help us triangulate various kinds of none-triangle geometry
*/

#ifndef _GY_TRIANGULATION_H
#define _GY_TRIANGULATION_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_math.h"
#include "gy_memory.h"
#include "gy_vectors.h"
#include "gy_rectangles.h"
#include "gy_rays.h"
#include "gy_primitives.h"

// +--------------------------------------------------------------+
// |                           Structs                            |
// +--------------------------------------------------------------+
struct TriangulateResult_t
{
	MemArena_t* allocArena;
	u64* indices;
	u64 numIndices;
	u64 numParts;
	u64 numHoles;
};

struct TriangulatePart_t
{
	u64 numVertices;
	const v2* vertices;
	
	//Used by the algorithm. They don't need to be filled beforehand
	bool isClockwise;
	bool isHole;
	u64 holeParentIndex;
	bool hasHoles;
	u64 numHoles;
};

struct TriangulateVert_t
{
	bool removed;
	u64 index;
	v2 pos;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	bool IsPolygonClockwise(u64 numVertices, v2* vertices);
	u64 RemoveColinearAndDuplicateVertices(u64 numVertices, v2* vertices);
	TriangulateVert_t* GetPrevTriangulateVert(TriangulateVert_t* vertices, u64 totalNumVertices, u64 startIndex);
	TriangulateVert_t* GetNextTriangulateVert(TriangulateVert_t* vertices, u64 totalNumVertices, u64 startIndex);
	u64* Triangulate2DEarClip(MemArena_t* memArena, MemArena_t* tempArena, u64 numVertices, const v2* vertices, u64* numIndicesOut = nullptr, bool debugDontDeallocate = false);
#else

// +--------------------------------------------------------------+
// |                   Clockwise Polygon Check                    |
// +--------------------------------------------------------------+
bool IsPolygonClockwise(u64 numVertices, v2* vertices)
{
	if (numVertices < 3) { return true; }
	r32 aggregateValue = 0;
	for (u64 vIndex = 0; vIndex < numVertices; vIndex++)
	{
		v2 currentVert = vertices[vIndex];
		v2 nextVert = vertices[(vIndex+1) % numVertices];
		aggregateValue += (nextVert.x - currentVert.x) * (nextVert.y + currentVert.y);
	}
	return (aggregateValue <= 0);
}

// +--------------------------------------------------------------+
// |                   Remove Problem Vertices                    |
// +--------------------------------------------------------------+
//Returns new vertex count
u64 RemoveColinearAndDuplicateVertices(u64 numVertices, v2* vertices)
{
	u64 numVerticesRemoved = 0;
	u64 writeIndex = 0;
	for (u64 readIndex = 0; readIndex < numVertices; readIndex++)
	{
		v2 baseVert = vertices[readIndex];
		u64 numVerticesRemovedThisRound = 0;
		for (u64 forwardIndex = 0; forwardIndex+1 < numVertices; forwardIndex++)
		{
			u64 nextVertIndex = ((readIndex+forwardIndex+1) % numVertices);
			v2 nextVert = vertices[nextVertIndex];
			v2 nextNextVert = vertices[(readIndex+forwardIndex+2) % numVertices];
			v2 firstVec = Vec2Normalize(baseVert - nextVert);
			v2 secondVec = Vec2Normalize(nextNextVert - nextVert);
			r32 angleBetween = Vec2AngleBetween(firstVec, secondVec);
			if (BasicallyEqualR32(AbsR32(angleBetween), Pi32, 0.01f) || Vec2BasicallyEqual(nextVert, nextNextVert))
			{
				// GyLibPrintLine_D("Removing vertex[%llu] (%g,%g)(%g,%g)(%g,%g)",
				// 	nextVertIndex,
				// 	baseVert.x, baseVert.y,
				// 	nextVert.x, nextVert.y,
				// 	nextNextVert.x, nextNextVert.y
				// );
				numVerticesRemovedThisRound++;
			}
			else { break; }
		}
		if (readIndex != writeIndex)
		{
			vertices[writeIndex] = vertices[readIndex];
		}
		numVerticesRemoved += numVerticesRemovedThisRound;
		writeIndex++;
		readIndex += numVerticesRemovedThisRound;
	}
	return numVertices - numVerticesRemoved;
}

// +--------------------------------------------------------------+
// |                     Ear-Clipping Method                      |
// +--------------------------------------------------------------+
TriangulateVert_t* GetPrevTriangulateVert(TriangulateVert_t* vertices, u64 totalNumVertices, u64 startIndex)
{
	Assert(totalNumVertices > 0);
	NotNull(vertices);
	for (u64 vIndex = ((startIndex > 0) ? startIndex-1 : (totalNumVertices-1)); vIndex != startIndex; vIndex = ((vIndex > 0) ? vIndex-1 : (totalNumVertices-1)))
	{
		if (!vertices[vIndex].removed) { return &vertices[vIndex]; }
	}
	return nullptr;
}
TriangulateVert_t* GetNextTriangulateVert(TriangulateVert_t* vertices, u64 totalNumVertices, u64 startIndex)
{
	Assert(totalNumVertices > 0);
	NotNull(vertices);
	for (u64 vIndex = ((startIndex+1) % totalNumVertices); vIndex != startIndex; vIndex = ((vIndex+1) % totalNumVertices))
	{
		if (!vertices[vIndex].removed) { return &vertices[vIndex]; }
	}
	return nullptr;
}
u64* Triangulate2DEarClip(MemArena_t* memArena, MemArena_t* tempArena, u64 numVertices, const v2* vertices, u64* numIndicesOut = nullptr, bool debugDontDeallocate = false)
{
	AssertIf(numVertices > 0, vertices != nullptr);
	AssertIf(memArena != nullptr, tempArena != nullptr);
	Assert(DoesMemArenaSupportPushAndPop(tempArena));
	
	if (numIndicesOut != nullptr) { *numIndicesOut = 0; }
	
	u64 numIndices = 0;
	u64* result = nullptr;
	
	if (numVertices < 3) { return nullptr; }
	else if (numVertices == 3)
	{
		numIndices = 3;
		if (numIndicesOut != nullptr) { *numIndicesOut = numIndices; }
		if (memArena == nullptr) { return nullptr; }
		result = AllocArray(memArena, u64, numIndices);
		if (result == nullptr) { return nullptr; }
		NotNull(result);
		result[0] = 0;
		result[1] = 1;
		result[2] = 2;
		return result;
	}
	
	u64 numTriangles = numVertices - 2;
	numIndices = numTriangles*3;
	if (numIndicesOut != nullptr) { *numIndicesOut = numIndices; }
	if (memArena == nullptr) { return nullptr; }
	
	result = AllocArray(memArena, u64, numIndices);
	if (result == nullptr) { return nullptr; }
	
	PushMemMark(tempArena);
	u64 numUnremovedVerts = numVertices;
	TriangulateVert_t* workingVerts = AllocArray(tempArena, TriangulateVert_t, numVertices);
	NotNull(workingVerts);
	for (u64 vIndex = 0; vIndex < numVertices; vIndex++)
	{
		workingVerts[vIndex].removed = false;
		workingVerts[vIndex].index = vIndex;
		workingVerts[vIndex].pos = vertices[vIndex];
	}
	
	u64 resultIndex = 0;
	while (numUnremovedVerts > 3)
	{
		TriangulateVert_t* earVertex = nullptr;
		TriangulateVert_t* earPrevVertex = nullptr;
		TriangulateVert_t* earNextVertex = nullptr;
		for (u64 vIndex = 0; vIndex < numVertices; vIndex++)
		{
			TriangulateVert_t* vert1 = &workingVerts[vIndex];
			if (!vert1->removed)
			{
				// TriangulateVert_t GetPrevTriangulateVert(TriangulateVert_t* vertices, u64 totalNumVertices, u64 startIndex)
				TriangulateVert_t* vert0 = GetPrevTriangulateVert(workingVerts, numVertices, vIndex);
				TriangulateVert_t* vert2 = GetNextTriangulateVert(workingVerts, numVertices, vIndex);
				if (vert0 == nullptr || vert2 == nullptr || !IsTriangleClockwise(vert0->pos, vert1->pos, vert2->pos)) { continue; }
				
				earVertex = vert1;
				earPrevVertex = vert0;
				earNextVertex = vert2;
				for (u64 vIndex2 = 0; vIndex2 < numVertices; vIndex2++)
				{
					TriangulateVert_t* otherVert = &workingVerts[vIndex2];
					if (!otherVert->removed && otherVert->index != vert0->index && otherVert->index != vert1->index && otherVert->index != vert2->index)
					{
						if (IsInsideTriangle(vert0->pos, vert1->pos, vert2->pos, otherVert->pos))
						{
							earVertex = nullptr;
							earPrevVertex = nullptr;
							earNextVertex = nullptr;
							break;
						}
					}
				}
				if (earVertex != nullptr) { break; }
			}
		}
		if (earVertex == nullptr)
		{
			// GyLibPrintLine_W("The polygon is invalid after %llu triangles!", resultIndex/3);
			PopMemMark(tempArena);
			if (!debugDontDeallocate)
			{
				if (memArena != tempArena) { FreeMem(memArena, result); }
				if (numIndicesOut != nullptr) { *numIndicesOut = 0; }
				return nullptr;
			}
			else
			{
				if (numIndicesOut != nullptr) { *numIndicesOut = resultIndex; }
				return result;
			}
		}
		
		Assert(resultIndex+3 <= numIndices);
		result[resultIndex + 0] = earPrevVertex->index;
		result[resultIndex + 1] = earVertex->index;
		result[resultIndex + 2] = earNextVertex->index;
		resultIndex += 3;
		
		earVertex->removed = true;
		numUnremovedVerts--;
	}
	Assert(numUnremovedVerts == 3);
	
	TriangulateVert_t* finalCenterVert = GetNextTriangulateVert(workingVerts, numVertices, 0);
	NotNull(finalCenterVert);
	TriangulateVert_t* finalPrevVert = GetPrevTriangulateVert(workingVerts, numVertices, finalCenterVert->index);
	TriangulateVert_t* finalNextVert = GetNextTriangulateVert(workingVerts, numVertices, finalCenterVert->index);
	NotNull2(finalPrevVert, finalNextVert);
	Assert(finalPrevVert->index != finalNextVert->index);
	
	Assert(resultIndex == numIndices-3);
	result[resultIndex + 0] = finalPrevVert->index;
	result[resultIndex + 1] = finalCenterVert->index;
	result[resultIndex + 2] = finalNextVert->index;
	resultIndex += 3;
	
	PopMemMark(tempArena);
	
	return result;
}

// +--------------------------------------------------------------+
// |            Polygon With Multiple Pieces and Holes            |
// +--------------------------------------------------------------+
#if 0
struct TriangulateWithHolesPart_t
{
	u64 numVertices;
	const v2* vertices;
	
	bool isClockwise;
	
	bool isHole;
	u64 holeParentIndex;
	bool hasHoles;
	u64 numHoles;
};
bool Triangulate2DMultiPolygonWithHoles(MemArena_t* memArena, MemArena_t* tempArena, u64 numParts, TriangulatePart_t* parts, TriangulateResult_t* result, bool debugDontDeallocate = false)
{
	if (numParts == 0) { return false; }
	NotNull(parts);
	AssertIf(memArena != nullptr, tempArena != nullptr);
	NotNull(result);
	
	//if just one part, then no holes
	if (numParts == 1)
	{
		ClearPointer(result);
		result->allocArena = memArena;
		result->numParts = 1;
		result->numHoles = 0;
		result->indices = Triangulate2DEarClip(memArena, tempArena, parts[0].numVertices, parts[0].vertices, &result->numIndices, debugDontDeallocate);
		return (result->indices != nullptr);
	}
	
	// +====================================+
	// | Calculate Clockwise for All Parts  |
	// +====================================+
	u64 numClockwiseParts = 0;
	u64 numAntiClockwiseParts = 0;
	for (u64 pIndex = 0; pIndex < numParts; pIndex++)
	{
		TriangulateWithHolesPart_t* part = &parts[pIndex];
		Assert(part->numVertices > 0);
		NotNull(part->vertices);
		part->isHole = false;
		part->isClockwise = IsPolygonClockwise(part->numVertices, part->vertices);
		if (part->isClockwise) { numClockwiseParts++; }
		else { numAntiClockwiseParts++; }
	}
	
	//if no opposite winding orders, than no holes
	if (numClockwiseParts == 0 || numAntiClockwiseParts == 0)
	{
		//TODO: Implement this!
	}
	
	// +==============================+
	// | Pair up Holes to Outer Parts |
	// +==============================+
	u64 maxVerticesPerPart = 0;
	u64 numNonHoleParts = 0;
	u64 numHoleParts = 0;
	u64 numTrianglesTotal = 0;
	for (u64 pIndex = 0; pIndex < numParts; pIndex++)
	{
		TriangulateWithHolesPart_t* part = &parts[pIndex];
		if (!part->isHole)
		{
			numNonHoleParts++;
			part->hasHoles = false;
			part->numHoles = 0;
			u64 numVerticesInPart = part->numVertices;
			for (u64 hIndex = pIndex+1; hIndex < numParts; hIndex++)
			{
				TriangulateWithHolesPart_t* holePart = &parts[pIndex];
				//TODO: We should do a check to make sure at least one of the vertices in the holePart is inside the part
				if (holePart->isClockwise != part->isClockwise)
				{
					holePart->isHole = true;
					holePart->holeParentIndex = pIndex;
					part->hasHoles = true;
					part->numHoles++;
					numVerticesInPart += holePart->numVertices;
				}
			}
			maxVerticesPerPart = MaxU64(maxVerticesPerPart, numVerticesInPart);
			
			u64 numTriangles = (numVerticesInPart + part->numHoles*2)-2;
			numTrianglesTotal += numTriangles;
		}
		else
		{
			numHoleParts++;
		}
	}
	
	u64 numIndicesTotal = numTrianglesTotal*3;
	if (memArena == nullptr)
	{
		ClearPointer(result);
		result->numIndices = numIndicesTotal;
		result->numParts = numNonHoleParts;
		result->numHoles = numHoleParts;
		return true;
	}
	
	PushMemMark(tempArena);
	{
		Assert(maxVerticesPerPart > 0);
		v2* workingVerts = AllocArray(tempArena, v2, maxVerticesPerPart);
		NotNull(workingVerts);
		
		// +==================================================+
		// | Splice Hole Vertices Into Parts and Triangulate  |
		// +==================================================+
		for (u64 pIndex = 0; pIndex < numParts; pIndex++)
		{
			TriangulateWithHolesPart_t* part = &parts[pIndex];
			if (!part->isHole)
			{
				u64 numWorkingVerts = 0;
				
				MyMemCopy(&workingVerts[numWorkingVerts], part->vertices, sizeof(v2) * part->numVertices);
				numWorkingVerts += part->numVertices;
				
				for (u64 hIndex = pIndex+1; hIndex < numParts; hIndex++)
				{
					TriangulateWithHolesPart_t* holePart = &workingParts[pIndex];
					if (holePart->isHole && holePart->holeParentIndex == pIndex)
					{
						//TODO: Find a mutually visible point on the hole and point in the polygon and splice in the vertices of the hole
					}
				}
				Assert(numWorkingVerts <= maxVerticesPerPart);
				
				
			}
		}
	}
	PopMemMark(tempArena);
	
	return true;
}
#endif

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_TRIANGULATION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
TriangulateResult_t
TriangulatePart_t
TriangulateVert_t
@Functions
bool IsPolygonClockwise(u64 numVertices, v2* vertices)
u64 RemoveColinearAndDuplicateVertices(u64 numVertices, v2* vertices)
TriangulateVert_t* GetPrevTriangulateVert(TriangulateVert_t* vertices, u64 totalNumVertices, u64 startIndex)
TriangulateVert_t* GetNextTriangulateVert(TriangulateVert_t* vertices, u64 totalNumVertices, u64 startIndex)
u64* Triangulate2DEarClip(MemArena_t* memArena, MemArena_t* tempArena, u64 numVertices, const v2* vertices, u64* numIndicesOut = nullptr, bool debugDontDeallocate = false)
*/
