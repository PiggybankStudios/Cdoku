/*
File:   gy_rectangles.h
Author: Taylor Robbins
Date:   10\06\2021
Description:
	** Contains the definitions and related functions for rec, reci, aabb, obb2, and obb3
*/

#ifndef _GY_RECTANGLES_H
#define _GY_RECTANGLES_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

#ifdef _GY_QUATERNIONS_H
#define OBB3D_AVAILABLE    1
#else
#define OBB3D_AVAILABLE    0
#endif

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
//TODO: Can we move the += and -= operator overloads to the bottom of the file??
//These are technically aabb's in 2D but we just call them "rectangles" for simplicity
struct Rectangle_t
{
	union
	{
		v2 topLeft;
		struct { r32 x, y; };
		struct { r32 left, top; };
	};
	union
	{
		v2 size;
		struct { r32 width, height; };
	};
	
	inline Rectangle_t& operator += (v2 vector)
	{
		this->topLeft += vector;
		return *this;
	}
	inline Rectangle_t& operator -= (v2 vector)
	{
		this->topLeft -= vector;
		return *this;
	}
};

struct Rectanglei_t
{
	union
	{
		v2i topLeft;
		struct { i32 x, y; };
		struct { i32 left, top; };
	};
	union
	{
		v2i size;
		struct { i32 width, height; };
	};
	
	inline Rectanglei_t& operator += (v2i vector)
	{
		this->topLeft += vector;
		return *this;
	}
	inline Rectanglei_t& operator -= (v2i vector)
	{
		this->topLeft -= vector;
		return *this;
	}
};

//Axis-Aligned Bounding Box
struct Box_t
{
	union
	{
		v3 bottomLeft;
		struct { r32 x, y, z; };
		struct { r32 left, bottom, back; };
	};
	union
	{
		v3 size;
		struct { r32 width, height, depth; };
	};
	
	inline Box_t& operator += (v3 vector)
	{
		this->bottomLeft += vector;
		return *this;
	}
	inline Box_t& operator -= (v3 vector)
	{
		this->bottomLeft -= vector;
		return *this;
	}
};

struct Boxi_t
{
	union
	{
		v3i bottomLeft;
		struct { i32 x, y, z; };
		struct { i32 left, bottom, back; };
	};
	union
	{
		v3i size;
		struct { i32 width, height, depth; };
	};
	
	inline Boxi_t& operator += (v3i vector)
	{
		this->bottomLeft += vector;
		return *this;
	}
	inline Boxi_t& operator -= (v3i vector)
	{
		this->bottomLeft -= vector;
		return *this;
	}
};

//Oriented Bounding Box
struct Obb2D_t
{
	union
	{
		v2 center;
		struct { r32 x, y; };
	};
	union
	{
		v2 size;
		struct { r32 width, height; };
	};
	r32 rotation;
	
	inline Obb2D_t& operator += (v2 vector)
	{
		this->center += vector;
		return *this;
	}
	inline Obb2D_t& operator -= (v2 vector)
	{
		this->center -= vector;
		return *this;
	}
};

#if OBB3D_AVAILABLE
struct Obb3D_t
{
	union
	{
		v3 center;
		struct { r32 x, y, z; };
	};
	union
	{
		v3 size;
		struct { r32 width, height, depth; };
	};
	quat rotation;
	
	inline Obb3D_t& operator += (v3 vector)
	{
		this->center += vector;
		return *this;
	}
	inline Obb3D_t& operator -= (v3 vector)
	{
		this->center -= vector;
		return *this;
	}
};
#endif

typedef Rectangle_t   rec;
typedef Rectanglei_t  reci;
typedef Obb2D_t       obb2;
typedef Box_t         box;
typedef Boxi_t        boxi;
#if OBB3D_AVAILABLE
typedef Obb3D_t       obb3;
#endif

// +--------------------------------------------------------------+
// |                   Simple Value Definitions                   |
// +--------------------------------------------------------------+
#define Rec_Zero    NewRec(0, 0, 0, 0)
#define Rec_Default NewRec(0, 0, 1, 1)
#define Reci_Zero   NewReci(0, 0, 0, 0)
#define Box_Zero    NewBox(0, 0, 0, 0, 0, 0)
#define Boxi_Zero   NewBoxi(0, 0, 0, 0, 0, 0)
#define Obb2_Zero   NewObb2D(0, 0, 0, 0, 0)
#if OBB3D_AVAILABLE
#define Obb3_Zero   NewObb3D(0, 0, 0, 0, 0, 0, Quat_Identity)
#endif

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	rec NewRec(r32 left, r32 top, r32 width, r32 height);
	rec NewRec(v2 topLeft, r32 width, r32 height);
	rec NewRec(r32 left, r32 top, v2 size);
	rec NewRec(v2 topLeft, v2 size);
	rec NewRecCentered(r32 centerX, r32 centerY, r32 width, r32 height);
	rec NewRecCentered(v2 center, r32 width, r32 height);
	rec NewRecCentered(r32 centerX, r32 centerY, v2 size);
	rec NewRecCentered(v2 center, v2 size);
	rec NewRecBetween(r32 point1X, r32 point1Y, r32 point2X, r32 point2Y);
	rec NewRecBetween(r32 point1X, r32 point1Y, v2 point2);
	rec NewRecBetween(v2 point1, r32 point2X, r32 point2Y);
	rec NewRecBetween(v2 point1, v2 point2);
	reci NewReci(i32 left, i32 top, i32 width, i32 height);
	reci NewReci(v2i topLeft, i32 width, i32 height);
	reci NewReci(i32 left, i32 top, v2i size);
	reci NewReci(v2i topLeft, v2i size);
	reci NewReciBetween(i32 point1X, i32 point1Y, i32 point2X, i32 point2Y);
	reci NewReciBetween(i32 point1X, i32 point1Y, v2i point2);
	reci NewReciBetween(v2i point1, i32 point2X, i32 point2Y);
	reci NewReciBetween(v2i point1, v2i point2);
	box NewBox(r32 x, r32 y, r32 z, r32 width, r32 height, r32 depth);
	box NewBox(v3 bottomLeft, r32 width, r32 height, r32 depth);
	box NewBox(r32 x, r32 y, r32 z, v3 size);
	box NewBox(v3 bottomLeft, v3 size);
	box NewBoxCentered(r32 centerX, r32 centerY, r32 centerZ, r32 width, r32 height, r32 depth);
	box NewBoxCentered(v3 center, r32 width, r32 height, r32 depth);
	box NewBoxCentered(r32 centerX, r32 centerY, r32 centerZ, v3 size);
	box NewBoxCentered(v3 center, v3 size);
	box NewBoxBetween(r32 point1X, r32 point1Y, r32 point1Z, r32 point2X, r32 point2Y, r32 point2Z);
	box NewBoxBetween(r32 point1X, r32 point1Y, r32 point1Z, v3 point2);
	box NewBoxBetween(v3 point1, r32 point2X, r32 point2Y, r32 point2Z);
	box NewBoxBetween(v3 point1, v3 point2);
	boxi NewBoxi(i32 x, i32 y, i32 z, i32 width, i32 height, i32 depth);
	boxi NewBoxi(v3i bottomLeft, i32 width, i32 height, i32 depth);
	boxi NewBoxi(i32 x, i32 y, i32 z, v3i size);
	boxi NewBoxi(v3i bottomLeft, v3i size);
	boxi NewBoxiBetween(i32 point1X, i32 point1Y, i32 point1Z, i32 point2X, i32 point2Y, i32 point2Z);
	boxi NewBoxiBetween(i32 point1X, i32 point1Y, i32 point1Z, v3i point2);
	boxi NewBoxiBetween(v3i point1, i32 point2X, i32 point2Y, i32 point2Z);
	boxi NewBoxiBetween(v3i point1, v3i point2);
	obb2 NewObb2D(r32 centerX, r32 centerY, r32 width, r32 height, r32 rotation);
	obb2 NewObb2D(v2 center, r32 width, r32 height, r32 rotation);
	obb2 NewObb2D(r32 centerX, r32 centerY, v2 size, r32 rotation);
	obb2 NewObb2D(v2 center, v2 size, r32 rotation);
	#if OBB3D_AVAILABLE
	obb3 NewObb3D(r32 centerX, r32 centerY, r32 centerZ, r32 width, r32 height, r32 depth, quat rotation);
	obb3 NewObb3D(r32 centerX, r32 centerY, r32 centerZ, v3 size, quat rotation);
	obb3 NewObb3D(v3 center, r32 width, r32 height, r32 depth, quat rotation);
	obb3 NewObb3D(v3 center, v3 size, quat rotation);
	#endif
	rec ToRec(reci rectangle);
	obb2 ToObb2D(rec rectangle);
	box ToBox(boxi boundingBox);
	#if OBB3D_AVAILABLE
	obb3 ToObb3D(box boundingBox);
	#endif
	v2 GetObb2DRelativePos(obb2 boundingBox, v2 point);
	v2 GetObb2DWorldPoint(obb2 boundingBox, v2 relativeOffset);
	v2 GetObb2DRightVec(obb2 boundingBox);
	v2 GetObb2DDownVec(obb2 boundingBox);
	v2 GetObb2DLeftVec(obb2 boundingBox);
	v2 GetObb2DUpVec(obb2 boundingBox);
	v2 GetObb2DTopLeft(obb2 boundingBox);
	v2 GetObb2DTopRight(obb2 boundingBox);
	v2 GetObb2DBottomLeft(obb2 boundingBox);
	v2 GetObb2DBottomRight(obb2 boundingBox);
	rec RecShift(rec rectangle, r32 amountX, r32 amountY);
	rec RecShift(rec rectangle, v2 amount);
	rec RecScale(rec rectangle, r32 scalar);
	rec RecScale2(rec rectangle, r32 scaleX, r32 scaleY);
	rec RecScale2(rec rectangle, v2 scale);
	bool RecBasicallyEqual(rec left, rec right, r32 tolerance = 0.001f);
	reci ReciShift(reci rectangle, i32 amountX, i32 amountY);
	reci ReciShift(reci rectangle, v2i amount);
	reci ReciScale(reci rectangle, i32 scalar);
	reci ReciScale2(reci rectangle, i32 scaleX, i32 scaleY);
	reci ReciScale2(reci rectangle, v2i scale);
	bool ReciEqual(reci left, reci right);
	box BoxShift(box boundingBox, r32 amountX, r32 amountY, r32 amountZ);
	box BoxShift(box boundingBox, v3 amount);
	box BoxScale(box boundingBox, r32 scalar);
	box BoxScale3(box boundingBox, r32 scaleX, r32 scaleY, r32 scaleZ);
	box BoxScale3(box boundingBox, v3 scale);
	bool BoxBasicallyEqual(box left, box right, r32 tolerance = 0.001f);
	boxi BoxiShift(boxi boundingBox, i32 amountX, i32 amountY, i32 amountZ);
	boxi BoxiShift(boxi boundingBox, v3i amount);
	boxi BoxiScale(boxi boundingBox, i32 scalar);
	boxi BoxiScale3(boxi boundingBox, i32 scaleX, i32 scaleY, i32 scaleZ);
	boxi BoxiScale3(boxi boundingBox, v3i scale);
	bool BoxiEqual(boxi left, boxi right);
	obb2 Obb2DShift(obb2 boundingBox, r32 amountX, r32 amountY);
	obb2 Obb2DShift(obb2 boundingBox, v2 amount);
	obb2 Obb2DScale(obb2 boundingBox, r32 scalar);
	bool Obb2DBasicallyEqual(obb2 left, obb2 right, r32 tolerance = 0.001f);
	rec RecExpand(rec rectangle, r32 extraWidth, r32 extraHeight);
	rec RecExpand(rec rectangle, v2 extraSize);
	rec RecExpandX(rec rectangle, r32 extraWidth);
	rec RecExpandY(rec rectangle, r32 extraHeight);
	rec RecExpandLeft(rec rectangle, r32 extraLeft);
	rec RecExpandRight(rec rectangle, r32 extraRight);
	rec RecExpandUp(rec rectangle, r32 extraUp);
	rec RecExpandDown(rec rectangle, r32 extraDown);
	rec RecRetract(rec rectangle, r32 subWidth, r32 subHeight);
	rec RecRetract(rec rectangle, v2 subSize);
	rec RecRetractX(rec rectangle, r32 subWidth);
	rec RecRetractY(rec rectangle, r32 subHeight);
	rec RecInflate(rec rectangle, r32 extraWidth, r32 extraHeight);
	rec RecInflate(rec rectangle, v2 extraSize);
	rec RecInflateX(rec rectangle, r32 extraWidth);
	rec RecInflateY(rec rectangle, r32 extraHeight);
	rec RecDeflate(rec rectangle, r32 subWidth, r32 subHeight);
	rec RecDeflate(rec rectangle, v2 extraSize);
	rec RecDeflateX(rec rectangle, r32 subWidth);
	rec RecDeflateY(rec rectangle, r32 subHeight);
	reci ReciExpand(reci rectangle, i32 extraWidth, i32 extraHeight);
	reci ReciExpand(reci rectangle, v2i extraSize);
	reci ReciExpandX(reci rectangle, i32 extraWidth);
	reci ReciExpandY(reci rectangle, i32 extraHeight);
	reci ReciExpandLeft(reci rectangle, i32 extraLeft);
	reci ReciExpandRight(reci rectangle, i32 extraRight);
	reci ReciExpandUp(reci rectangle, i32 extraUp);
	reci ReciExpandDown(reci rectangle, i32 extraDown);
	reci ReciRetract(reci rectangle, i32 subWidth, i32 subHeight);
	reci ReciRetract(reci rectangle, v2i subSize);
	reci ReciRetractX(reci rectangle, i32 subWidth);
	reci ReciRetractY(reci rectangle, i32 subHeight);
	reci ReciInflate(reci rectangle, i32 extraWidth, i32 extraHeight);
	reci ReciInflate(reci rectangle, v2i extraSize);
	reci ReciInflateX(reci rectangle, i32 extraWidth);
	reci ReciInflateY(reci rectangle, i32 extraHeight);
	reci ReciDeflate(reci rectangle, i32 subWidth, i32 subHeight);
	reci ReciDeflate(reci rectangle, v2i extraSize);
	reci ReciDeflateX(reci rectangle, i32 subWidth);
	reci ReciDeflateY(reci rectangle, i32 subHeight);
	box BoxExpand(box boundingBox, r32 extraWidth, r32 extraHeight, r32 extraDepth);
	box BoxExpand(box boundingBox, v3 extraSize);
	box BoxExpandX(box boundingBox, r32 extraWidth);
	box BoxExpandY(box boundingBox, r32 extraHeight);
	box BoxExpandZ(box boundingBox, r32 extraDepth);
	box BoxRetract(box boundingBox, r32 subWidth, r32 subHeight, r32 subDepth);
	box BoxRetract(box boundingBox, v3 subSize);
	box BoxRetractX(box boundingBox, r32 subWidth);
	box BoxRetractY(box boundingBox, r32 subHeight);
	box BoxRetractZ(box boundingBox, r32 subDepth);
	box BoxInflate(box boundingBox, r32 extraWidth, r32 extraHeight, r32 extraDepth);
	box BoxInflate(box boundingBox, v3 extraSize);
	box BoxInflateX(box boundingBox, r32 extraWidth);
	box BoxInflateY(box boundingBox, r32 extraHeight);
	box BoxInflateZ(box boundingBox, r32 extraDepth);
	box BoxDeflate(box boundingBox, r32 subWidth, r32 subHeight, r32 subDepth);
	box BoxDeflate(box boundingBox, v3 extraSize);
	box BoxDeflateX(box boundingBox, r32 subWidth);
	box BoxDeflateY(box boundingBox, r32 subHeight);
	box BoxDeflateZ(box boundingBox, r32 subDepth);
	boxi BoxiExpand(boxi boundingBox, i32 extraWidth, i32 extraHeight, i32 extraDepth);
	boxi BoxiExpand(boxi boundingBox, v3i extraSize);
	boxi BoxiExpandX(boxi boundingBox, i32 extraWidth);
	boxi BoxiExpandY(boxi boundingBox, i32 extraHeight);
	boxi BoxiExpandZ(boxi boundingBox, i32 extraDepth);
	boxi BoxiRetract(boxi boundingBox, i32 subWidth, i32 subHeight, i32 subDepth);
	boxi BoxiRetract(boxi boundingBox, v3i subSize);
	boxi BoxiRetractX(boxi boundingBox, i32 subWidth);
	boxi BoxiRetractY(boxi boundingBox, i32 subHeight);
	boxi BoxiRetractZ(boxi boundingBox, i32 subDepth);
	boxi BoxiInflate(boxi boundingBox, i32 extraWidth, i32 extraHeight, i32 extraDepth);
	boxi BoxiInflate(boxi boundingBox, v3i extraSize);
	boxi BoxiInflateX(boxi boundingBox, i32 extraWidth);
	boxi BoxiInflateY(boxi boundingBox, i32 extraHeight);
	boxi BoxiInflateZ(boxi boundingBox, i32 extraDepth);
	boxi BoxiDeflate(boxi boundingBox, i32 subWidth, i32 subHeight, i32 subDepth);
	boxi BoxiDeflate(boxi boundingBox, v3i extraSize);
	boxi BoxiDeflateX(boxi boundingBox, i32 subWidth);
	boxi BoxiDeflateY(boxi boundingBox, i32 subHeight);
	boxi BoxiDeflateZ(boxi boundingBox, i32 subDepth);
	obb2 Obb2DInflate(obb2 rectangle, r32 extraWidth, r32 extraHeight);
	obb2 Obb2DInflate(obb2 rectangle, v2 extraSize);
	obb2 Obb2DInflateX(obb2 rectangle, r32 extraWidth);
	obb2 Obb2DInflateY(obb2 rectangle, r32 extraHeight);
	obb2 Obb2DDeflate(obb2 rectangle, r32 subWidth, r32 subHeight);
	obb2 Obb2DDeflate(obb2 rectangle, v2 extraSize);
	obb2 Obb2DDeflateX(obb2 rectangle, r32 subWidth);
	obb2 Obb2DDeflateY(obb2 rectangle, r32 subHeight);
	rec RecSquarify(rec rectangle, bool makeLarger = true, bool center = true);
	rec RecInvert(rec rectangle);
	rec RecInvertX(rec rectangle);
	rec RecInvertY(rec rectangle);
	rec RecUninvert(rec rectangle);
	rec RecBoth(rec rectangle1, rec rectangle2);
	rec RecExpandToVec2(rec rectangle, v2 newPoint);
	rec RecOverlap(rec rectangle1, rec rectangle2);
	reci ReciSquarify(reci rectangle, bool makeLarger = true);
	reci ReciInvert(reci rectangle);
	reci ReciInvertX(reci rectangle);
	reci ReciInvertY(reci rectangle);
	reci ReciUninvert(reci rectangle);
	reci ReciBoth(reci rectangle1, reci rectangle2);
	reci ReciExpandToVec2i(reci rectangle, v2i newPoint);
	reci ReciOverlap(reci rectangle1, reci rectangle2);
	box BoxCubify(box boundingBox, bool makeLarger = true, bool center = true);
	box BoxInvert(box boundingBox);
	box BoxInvertX(box boundingBox);
	box BoxInvertY(box boundingBox);
	box BoxInvertZ(box boundingBox);
	box BoxUninvert(box boundingBox);
	boxi BoxiCubify(boxi boundingBox, bool makeLarger = true);
	boxi BoxiInvert(boxi boundingBox);
	boxi BoxiInvertX(boxi boundingBox);
	boxi BoxiInvertY(boxi boundingBox);
	boxi BoxiInvertZ(boxi boundingBox);
	boxi BoxiUninvert(boxi boundingBox);
	boxi BoxiExpandToVec3i(boxi boundingBox, v3i newPoint);
	boxi BoxiBoth(boxi boundingBox1, boxi boundingBox2);
	obb2 Obb2DSquarify(obb2 rectangle, bool makeLarger = true);
	obb2 Obb2DInvert(obb2 rectangle);
	obb2 Obb2DInvertX(obb2 rectangle);
	obb2 Obb2DInvertY(obb2 rectangle);
	obb2 Obb2DUninvert(obb2 rectangle);
	rec GetObb2DAlignedBounds(obb2 boundingBox);
	bool IsInsideRec(rec rectangle, v2 point);
	bool IsInsideRec(rec rectangle, v2i point);
	bool RecsIntersect(rec rectangle1, rec rectangle2, bool inclusive = true);
	bool IsInsideReci(reci rectangle, v2i point, bool includePositiveEdges = false);
	bool RecisIntersect(reci rectangle1, reci rectangle2, bool inclusive = true);
	bool IsInsideBox(box boundingBox, v3 point);
	bool IsInsideBox(box boundingBox, v3i point);
	bool BoxesIntersect(box boundingBox1, box boundingBox2, bool inclusive = true);
	bool IsInsideBoxi(boxi boundingBox, v3i point, bool includePositiveEdges = false);
	bool BoxisIntersect(boxi boundingBox1, boxi boundingBox2, bool inclusive = true);
	bool IsInsideObb2D(obb2 rectangle, v2 point);
	void RecAlign(rec* rectangleOut, r32 alignmentScale = 1.0f);
	void RecAlignTopLeft(rec* rectangleOut, r32 alignmentScale = 1.0f);
	void RecAlignSize(rec* rectangleOut, r32 alignmentScale = 1.0f);
	void RecLayoutLeftOf(rec* rectangleOut, r32 posOfThingToRight, r32 rightPadding = 0.0f);
	void RecLayoutTopOf(rec* rectangleOut, r32 posOfThingDownwards, r32 bottomPadding = 0.0f);
	void RecLayoutRightOf(rec* rectangleOut, r32 posOfThingToLeft, r32 leftPadding = 0.0f);
	void RecLayoutRightOf(rec* rectangleOut, rec recToLeft, r32 leftPadding = 0.0f);
	void RecLayoutBottomOf(rec* rectangleOut, r32 posOfThingUpwards, r32 topPadding = 0.0f);
	void RecLayoutBottomOf(rec* rectangleOut, rec recUpwards, r32 topPadding = 0.0f);
	void RecLayoutBetweenX(rec* rectangleOut, r32 leftSide, r32 rightSide, r32 leftMargin = 0, r32 rightMargin = 0, r32 minWidth = 0);
	void RecLayoutBetweenY(rec* rectangleOut, r32 topSide, r32 bottomSide, r32 topMargin = 0, r32 bottomMargin = 0, r32 minHeight = 0);
	void RecLayoutLeftPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionWidth, bool shrinkOtherRec = false, r32 offsetAmount = 0);
	void RecLayoutRightPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionWidth, bool shrinkOtherRec = false, r32 offsetAmount = 0);
	void RecLayoutTopPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionHeight, bool shrinkOtherRec = false, r32 offsetAmount = 0);
	void RecLayoutBottomPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionHeight, bool shrinkOtherRec = false, r32 offsetAmount = 0);
	void RecLayoutVerticalCenter(rec* rectangleOut, r32 posToCenterTo, r32 percentage = 0.5f);
	void RecLayoutVerticalCenter(rec* rectangleOut, rec otherRectangle, r32 percentage = 0.5f);
	void RecLayoutHorizontalCenter(rec* rectangleOut, r32 posToCenterTo, r32 percentage = 0.5f);
	void RecLayoutHorizontalCenter(rec* rectangleOut, rec otherRectangle, r32 percentage = 0.5f);
	obb2 Obb2Line(v2 start, v2 end, r32 thickness);
	rec operator + (rec rectangle, v2 vector);
	rec operator - (rec rectangle, v2 vector);
	rec operator * (rec rectangle, r32 scale);
	rec operator / (rec rectangle, r32 scale);
	reci operator + (reci rectangle, v2i vector);
	reci operator - (reci rectangle, v2i vector);
	reci operator * (reci rectangle, i32 scale);
	reci operator / (reci rectangle, i32 scale);
	bool operator == (reci left, reci right);
	bool operator != (reci left, reci right);
	box operator + (box boundingBox, v3 vector);
	box operator - (box boundingBox, v3 vector);
	box operator * (box boundingBox, r32 scale);
	box operator / (box boundingBox, r32 scale);
	boxi operator + (boxi boundingBox, v3i vector);
	boxi operator - (boxi boundingBox, v3i vector);
	boxi operator * (boxi boundingBox, i32 scale);
	boxi operator / (boxi boundingBox, i32 scale);
	bool operator == (boxi left, boxi right);
	bool operator != (boxi left, boxi right);
	obb2 operator + (obb2 box, v2 vector);
	obb2 operator - (obb2 box, v2 vector);
	obb2 operator * (obb2 box, r32 scale);
	obb2 operator / (obb2 box, r32 scale);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
rec NewRec(r32 left, r32 top, r32 width, r32 height)
{
	rec result;
	result.left = left;
	result.top = top;
	result.width = width;
	result.height = height;
	return result;
}
rec NewRec(v2 topLeft, r32 width, r32 height)
{
	rec result;
	result.topLeft = topLeft;
	result.width = width;
	result.height = height;
	return result;
}
rec NewRec(r32 left, r32 top, v2 size)
{
	rec result;
	result.left = left;
	result.top = top;
	result.size = size;
	return result;
}
rec NewRec(v2 topLeft, v2 size)
{
	rec result;
	result.topLeft = topLeft;
	result.size = size;
	return result;
}
rec NewRecCentered(r32 centerX, r32 centerY, r32 width, r32 height)
{
	rec result;
	result.x = centerX - width/2;
	result.y = centerY - height/2;
	result.width = width;
	result.height = height;
	return result;
}
rec NewRecCentered(v2 center, r32 width, r32 height)
{
	rec result;
	result.x = center.x - width/2;
	result.y = center.y - height/2;
	result.width = width;
	result.height = height;
	return result;
}
rec NewRecCentered(r32 centerX, r32 centerY, v2 size)
{
	rec result;
	result.x = centerX - size.width/2;
	result.y = centerY - size.height/2;
	result.size = size;
	return result;
}
rec NewRecCentered(v2 center, v2 size)
{
	rec result;
	result.x = center.x - size.width/2;
	result.y = center.y - size.height/2;
	result.size = size;
	return result;
}
rec NewRecBetween(r32 point1X, r32 point1Y, r32 point2X, r32 point2Y)
{
	rec result;
	result.x = MinR32(point1X, point2X);
	result.y = MinR32(point1Y, point2Y);
	result.width = MaxR32(point1X, point2X) - result.x;
	result.height = MaxR32(point1Y, point2Y) - result.y;
	return result;
}
rec NewRecBetween(r32 point1X, r32 point1Y, v2 point2)
{
	rec result;
	result.x = MinR32(point1X, point2.x);
	result.y = MinR32(point1Y, point2.y);
	result.width = MaxR32(point1X, point2.x) - result.x;
	result.height = MaxR32(point1Y, point2.y) - result.y;
	return result;
}
rec NewRecBetween(v2 point1, r32 point2X, r32 point2Y)
{
	rec result;
	result.x = MinR32(point1.x, point2X);
	result.y = MinR32(point1.y, point2Y);
	result.width = MaxR32(point1.x, point2X) - result.x;
	result.height = MaxR32(point1.y, point2Y) - result.y;
	return result;
}
rec NewRecBetween(v2 point1, v2 point2)
{
	rec result;
	result.x = MinR32(point1.x, point2.x);
	result.y = MinR32(point1.y, point2.y);
	result.width = MaxR32(point1.x, point2.x) - result.x;
	result.height = MaxR32(point1.y, point2.y) - result.y;
	return result;
}

reci NewReci(i32 left, i32 top, i32 width, i32 height)
{
	reci result;
	result.left = left;
	result.top = top;
	result.width = width;
	result.height = height;
	return result;
}
reci NewReci(v2i topLeft, i32 width, i32 height)
{
	reci result;
	result.topLeft = topLeft;
	result.width = width;
	result.height = height;
	return result;
}
reci NewReci(i32 left, i32 top, v2i size)
{
	reci result;
	result.left = left;
	result.top = top;
	result.size = size;
	return result;
}
reci NewReci(v2i topLeft, v2i size)
{
	reci result;
	result.topLeft = topLeft;
	result.size = size;
	return result;
}
reci NewReciBetween(i32 point1X, i32 point1Y, i32 point2X, i32 point2Y)
{
	reci result;
	result.x = MinI32(point1X, point2X);
	result.y = MinI32(point1Y, point2Y);
	result.width = MaxI32(point1X, point2X) - result.x;
	result.height = MaxI32(point1Y, point2Y) - result.y;
	return result;
}
reci NewReciBetween(i32 point1X, i32 point1Y, v2i point2)
{
	reci result;
	result.x = MinI32(point1X, point2.x);
	result.y = MinI32(point1Y, point2.y);
	result.width = MaxI32(point1X, point2.x) - result.x;
	result.height = MaxI32(point1Y, point2.y) - result.y;
	return result;
}
reci NewReciBetween(v2i point1, i32 point2X, i32 point2Y)
{
	reci result;
	result.x = MinI32(point1.x, point2X);
	result.y = MinI32(point1.y, point2Y);
	result.width = MaxI32(point1.x, point2X) - result.x;
	result.height = MaxI32(point1.y, point2Y) - result.y;
	return result;
}
reci NewReciBetween(v2i point1, v2i point2)
{
	reci result;
	result.x = MinI32(point1.x, point2.x);
	result.y = MinI32(point1.y, point2.y);
	result.width = MaxI32(point1.x, point2.x) - result.x;
	result.height = MaxI32(point1.y, point2.y) - result.y;
	return result;
}

box NewBox(r32 x, r32 y, r32 z, r32 width, r32 height, r32 depth)
{
	box result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
box NewBox(v3 bottomLeft, r32 width, r32 height, r32 depth)
{
	box result;
	result.bottomLeft = bottomLeft;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
box NewBox(r32 x, r32 y, r32 z, v3 size)
{
	box result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.size = size;
	return result;
}
box NewBox(v3 bottomLeft, v3 size)
{
	box result;
	result.bottomLeft = bottomLeft;
	result.size = size;
	return result;
}
box NewBoxCentered(r32 centerX, r32 centerY, r32 centerZ, r32 width, r32 height, r32 depth)
{
	box result;
	result.x = centerX - width/2;
	result.y = centerY - height/2;
	result.z = centerZ - depth/2;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
box NewBoxCentered(v3 center, r32 width, r32 height, r32 depth)
{
	box result;
	result.x = center.x - width/2;
	result.y = center.y - height/2;
	result.z = center.z - depth/2;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
box NewBoxCentered(r32 centerX, r32 centerY, r32 centerZ, v3 size)
{
	box result;
	result.x = centerX - size.width/2;
	result.y = centerY - size.height/2;
	result.z = centerZ - size.depth/2;
	result.size = size;
	return result;
}
box NewBoxCentered(v3 center, v3 size)
{
	box result;
	result.x = center.x - size.width/2;
	result.y = center.y - size.height/2;
	result.z = center.z - size.depth/2;
	result.size = size;
	return result;
}
box NewBoxBetween(r32 point1X, r32 point1Y, r32 point1Z, r32 point2X, r32 point2Y, r32 point2Z)
{
	box result;
	result.x = MinR32(point1X, point2X);
	result.y = MinR32(point1Y, point2Y);
	result.z = MinR32(point1Z, point2Z);
	result.width = MaxR32(point1X, point2X) - result.x;
	result.height = MaxR32(point1Y, point2Y) - result.y;
	result.depth = MaxR32(point1Z, point2Z) - result.z;
	return result;
}
box NewBoxBetween(r32 point1X, r32 point1Y, r32 point1Z, v3 point2)
{
	box result;
	result.x = MinR32(point1X, point2.x);
	result.y = MinR32(point1Y, point2.y);
	result.z = MinR32(point1Z, point2.z);
	result.width = MaxR32(point1X, point2.x) - result.x;
	result.height = MaxR32(point1Y, point2.y) - result.y;
	result.depth = MaxR32(point1Z, point2.z) - result.z;
	return result;
}
box NewBoxBetween(v3 point1, r32 point2X, r32 point2Y, r32 point2Z)
{
	box result;
	result.x = MinR32(point1.x, point2X);
	result.y = MinR32(point1.y, point2Y);
	result.z = MinR32(point1.z, point2Z);
	result.width = MaxR32(point1.x, point2X) - result.x;
	result.height = MaxR32(point1.y, point2Y) - result.y;
	result.depth = MaxR32(point1.z, point2Z) - result.z;
	return result;
}
box NewBoxBetween(v3 point1, v3 point2)
{
	box result;
	result.x = MinR32(point1.x, point2.x);
	result.y = MinR32(point1.y, point2.y);
	result.z = MinR32(point1.z, point2.z);
	result.width = MaxR32(point1.x, point2.x) - result.x;
	result.height = MaxR32(point1.y, point2.y) - result.y;
	result.depth = MaxR32(point1.z, point2.z) - result.z;
	return result;
}

boxi NewBoxi(i32 x, i32 y, i32 z, i32 width, i32 height, i32 depth)
{
	boxi result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
boxi NewBoxi(v3i bottomLeft, i32 width, i32 height, i32 depth)
{
	boxi result;
	result.bottomLeft = bottomLeft;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
boxi NewBoxi(i32 x, i32 y, i32 z, v3i size)
{
	boxi result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.size = size;
	return result;
}
boxi NewBoxi(v3i bottomLeft, v3i size)
{
	boxi result;
	result.bottomLeft = bottomLeft;
	result.size = size;
	return result;
}
boxi NewBoxiBetween(i32 point1X, i32 point1Y, i32 point1Z, i32 point2X, i32 point2Y, i32 point2Z)
{
	boxi result;
	result.x = MinI32(point1X, point2X);
	result.y = MinI32(point1Y, point2Y);
	result.z = MinI32(point1Z, point2Z);
	result.width = MaxI32(point1X, point2X) - result.x;
	result.height = MaxI32(point1Y, point2Y) - result.y;
	result.depth = MaxI32(point1Z, point2Z) - result.z;
	return result;
}
boxi NewBoxiBetween(i32 point1X, i32 point1Y, i32 point1Z, v3i point2)
{
	boxi result;
	result.x = MinI32(point1X, point2.x);
	result.y = MinI32(point1Y, point2.y);
	result.z = MinI32(point1Z, point2.z);
	result.width = MaxI32(point1X, point2.x) - result.x;
	result.height = MaxI32(point1Y, point2.y) - result.y;
	result.depth = MaxI32(point1Z, point2.z) - result.z;
	return result;
}
boxi NewBoxiBetween(v3i point1, i32 point2X, i32 point2Y, i32 point2Z)
{
	boxi result;
	result.x = MinI32(point1.x, point2X);
	result.y = MinI32(point1.y, point2Y);
	result.z = MinI32(point1.z, point2Z);
	result.width = MaxI32(point1.x, point2X) - result.x;
	result.height = MaxI32(point1.y, point2Y) - result.y;
	result.depth = MaxI32(point1.z, point2Z) - result.z;
	return result;
}
boxi NewBoxiBetween(v3i point1, v3i point2)
{
	boxi result;
	result.x = MinI32(point1.x, point2.x);
	result.y = MinI32(point1.y, point2.y);
	result.z = MinI32(point1.z, point2.z);
	result.width = MaxI32(point1.x, point2.x) - result.x;
	result.height = MaxI32(point1.y, point2.y) - result.y;
	result.depth = MaxI32(point1.z, point2.z) - result.z;
	return result;
}

obb2 NewObb2D(r32 centerX, r32 centerY, r32 width, r32 height, r32 rotation)
{
	obb2 result;
	result.x = centerX;
	result.y = centerY;
	result.width = width;
	result.height = height;
	result.rotation = rotation;
	return result;
}
obb2 NewObb2D(v2 center, r32 width, r32 height, r32 rotation)
{
	obb2 result;
	result.center = center;
	result.width = width;
	result.height = height;
	result.rotation = rotation;
	return result;
}
obb2 NewObb2D(r32 centerX, r32 centerY, v2 size, r32 rotation)
{
	obb2 result;
	result.x = centerX;
	result.y = centerY;
	result.size = size;
	result.rotation = rotation;
	return result;
}
obb2 NewObb2D(v2 center, v2 size, r32 rotation)
{
	obb2 result;
	result.center = center;
	result.size = size;
	result.rotation = rotation;
	return result;
}

#if OBB3D_AVAILABLE
obb3 NewObb3D(r32 centerX, r32 centerY, r32 centerZ, r32 width, r32 height, r32 depth, quat rotation)
{
	obb3 result;
	result.center.x = centerX;
	result.center.y = centerY;
	result.center.z = centerZ;
	result.width = width;
	result.height = height;
	result.depth = depth;
	result.rotation = rotation;
	return result;
}
obb3 NewObb3D(r32 centerX, r32 centerY, r32 centerZ, v3 size, quat rotation)
{
	obb3 result;
	result.center.x = centerX;
	result.center.y = centerY;
	result.center.z = centerZ;
	result.size = size;
	result.rotation = rotation;
	return result;
}
obb3 NewObb3D(v3 center, r32 width, r32 height, r32 depth, quat rotation)
{
	obb3 result;
	result.center = center;
	result.width = width;
	result.height = height;
	result.depth = depth;
	result.rotation = rotation;
	return result;
}
obb3 NewObb3D(v3 center, v3 size, quat rotation)
{
	obb3 result;
	result.center = center;
	result.size = size;
	result.rotation = rotation;
	return result;
}
#endif

// +--------------------------------------------------------------+
// |                 Simple Conversions and Casts                 |
// +--------------------------------------------------------------+
rec ToRec(reci rectangle)
{
	rec result;
	result.x = (r32)rectangle.x;
	result.y = (r32)rectangle.y;
	result.width = (r32)rectangle.width;
	result.height = (r32)rectangle.height;
	return result;
}
obb2 ToObb2D(rec rectangle)
{
	obb2 result;
	result.x = rectangle.x + rectangle.width/2;
	result.y = rectangle.y + rectangle.height/2;
	result.size = rectangle.size;
	result.rotation = 0;
	return result;
}
box ToBox(boxi boundingBox)
{
	box result;
	result.x = (r32)boundingBox.x;
	result.y = (r32)boundingBox.y;
	result.z = (r32)boundingBox.z;
	result.width  = (r32)boundingBox.width;
	result.height = (r32)boundingBox.height;
	result.depth  = (r32)boundingBox.depth;
	return result;
}
#if OBB3D_AVAILABLE
obb3 ToObb3D(box boundingBox)
{
	obb3 result;
	result.x = boundingBox.x + boundingBox.width/2;
	result.y = boundingBox.y + boundingBox.height/2;
	result.z = boundingBox.z + boundingBox.depth/2;
	result.size = boundingBox.size;
	result.rotation = Quat_Identity;
	return result;
}
#endif

// +--------------------------------------------------------------+
// |                 Simple Information Functions                 |
// +--------------------------------------------------------------+
// +==============================+
// |            Obb2D             |
// +==============================+
v2 GetObb2DRelativePos(obb2 boundingBox, v2 point)
{
	v2 result;
	v2 rotVec = NewVec2(CosR32(boundingBox.rotation), SinR32(boundingBox.rotation));
	result.x = Vec2Dot(point - boundingBox.center, rotVec);
	result.y = Vec2Dot(point - boundingBox.center, Vec2PerpRight(rotVec));
	return result;
}
v2 GetObb2DWorldPoint(obb2 boundingBox, v2 relativeOffset)
{
	v2 result = boundingBox.center;
	v2 rotVec = NewVec2(CosR32(boundingBox.rotation), SinR32(boundingBox.rotation));
	result += (-boundingBox.width/2 + relativeOffset.x) * rotVec;
	result += (-boundingBox.height/2 + relativeOffset.y) * Vec2PerpRight(rotVec);
	return result;
}

v2 GetObb2DRightVec(obb2 boundingBox)
{
	return Vec2FromAngle(boundingBox.rotation);
}
v2 GetObb2DDownVec(obb2 boundingBox)
{
	return Vec2PerpRight(Vec2FromAngle(boundingBox.rotation));
}
v2 GetObb2DLeftVec(obb2 boundingBox)
{
	return -Vec2FromAngle(boundingBox.rotation);
}
v2 GetObb2DUpVec(obb2 boundingBox)
{
	return Vec2PerpLeft(Vec2FromAngle(boundingBox.rotation));
}

v2 GetObb2DTopLeft(obb2 boundingBox)
{
	return GetObb2DWorldPoint(boundingBox, Vec2_Zero);
}
v2 GetObb2DTopRight(obb2 boundingBox)
{
	return GetObb2DWorldPoint(boundingBox, NewVec2(boundingBox.width, 0));
}
v2 GetObb2DBottomLeft(obb2 boundingBox)
{
	return GetObb2DWorldPoint(boundingBox, NewVec2(0, boundingBox.height));
}
v2 GetObb2DBottomRight(obb2 boundingBox)
{
	return GetObb2DWorldPoint(boundingBox, NewVec2(boundingBox.width, boundingBox.height));
}

// +--------------------------------------------------------------+
// |                Operator Overload Equivalents                 |
// +--------------------------------------------------------------+
// +==============================+
// |             Rec              |
// +==============================+
rec RecShift(rec rectangle, r32 amountX, r32 amountY)
{
	rec result;
	result.x = rectangle.x + amountX;
	result.y = rectangle.y + amountY;
	result.size = rectangle.size;
	return result;
}
rec RecShift(rec rectangle, v2 amount)
{
	rec result;
	result.x = rectangle.x + amount.x;
	result.y = rectangle.y + amount.y;
	result.size = rectangle.size;
	return result;
}
rec RecScale(rec rectangle, r32 scalar)
{
	rec result;
	result.x = rectangle.x * scalar;
	result.y = rectangle.y * scalar;
	result.width = rectangle.width * scalar;
	result.height = rectangle.height * scalar;
	return result;
}
rec RecScale2(rec rectangle, r32 scaleX, r32 scaleY)
{
	rec result;
	result.x = rectangle.x * scaleX;
	result.y = rectangle.y * scaleY;
	result.width = rectangle.width * scaleX;
	result.height = rectangle.height * scaleY;
	return result;
}
rec RecScale2(rec rectangle, v2 scale)
{
	rec result;
	result.x = rectangle.x * scale.x;
	result.y = rectangle.y * scale.y;
	result.width = rectangle.width * scale.x;
	result.height = rectangle.height * scale.y;
	return result;
}
bool RecBasicallyEqual(rec left, rec right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x,      right.x,      tolerance)) { return false; }
	if (!BasicallyEqualR32(left.y,      right.y,      tolerance)) { return false; }
	if (!BasicallyEqualR32(left.width,  right.width,  tolerance)) { return false; }
	if (!BasicallyEqualR32(left.height, right.height, tolerance)) { return false; }
	return true;
}

// +==============================+
// |             Reci             |
// +==============================+
reci ReciShift(reci rectangle, i32 amountX, i32 amountY)
{
	reci result;
	result.x = rectangle.x + amountX;
	result.y = rectangle.y + amountY;
	result.size = rectangle.size;
	return result;
}
reci ReciShift(reci rectangle, v2i amount)
{
	reci result;
	result.x = rectangle.x + amount.x;
	result.y = rectangle.y + amount.y;
	result.size = rectangle.size;
	return result;
}
reci ReciScale(reci rectangle, i32 scalar)
{
	reci result;
	result.x = rectangle.x * scalar;
	result.y = rectangle.y * scalar;
	result.width = rectangle.width * scalar;
	result.height = rectangle.height * scalar;
	return result;
}
reci ReciScale2(reci rectangle, i32 scaleX, i32 scaleY)
{
	reci result;
	result.x = rectangle.x * scaleX;
	result.y = rectangle.y * scaleY;
	result.width = rectangle.width * scaleX;
	result.height = rectangle.height * scaleY;
	return result;
}
reci ReciScale2(reci rectangle, v2i scale)
{
	reci result;
	result.x = rectangle.x * scale.x;
	result.y = rectangle.y * scale.y;
	result.width = rectangle.width * scale.x;
	result.height = rectangle.height * scale.y;
	return result;
}
bool ReciEqual(reci left, reci right)
{
	if (left.x      != right.x)      { return false; }
	if (left.y      != right.y)      { return false; }
	if (left.width  != right.width)  { return false; }
	if (left.height != right.height) { return false; }
	return true;
}

// +==============================+
// |             Box              |
// +==============================+
box BoxShift(box boundingBox, r32 amountX, r32 amountY, r32 amountZ)
{
	box result;
	result.x = boundingBox.x + amountX;
	result.y = boundingBox.y + amountY;
	result.z = boundingBox.z + amountZ;
	result.size = boundingBox.size;
	return result;
}
box BoxShift(box boundingBox, v3 amount)
{
	box result;
	result.x = boundingBox.x + amount.x;
	result.y = boundingBox.y + amount.y;
	result.z = boundingBox.z + amount.z;
	result.size = boundingBox.size;
	return result;
}
box BoxScale(box boundingBox, r32 scalar)
{
	box result;
	result.x = boundingBox.x * scalar;
	result.y = boundingBox.y * scalar;
	result.z = boundingBox.z * scalar;
	result.width = boundingBox.width * scalar;
	result.height = boundingBox.height * scalar;
	result.depth = boundingBox.depth * scalar;
	return result;
}
box BoxScale3(box boundingBox, r32 scaleX, r32 scaleY, r32 scaleZ)
{
	box result;
	result.x = boundingBox.x * scaleX;
	result.y = boundingBox.y * scaleY;
	result.z = boundingBox.z * scaleZ;
	result.width = boundingBox.width * scaleX;
	result.height = boundingBox.height * scaleY;
	result.depth = boundingBox.depth * scaleZ;
	return result;
}
box BoxScale3(box boundingBox, v3 scale)
{
	box result;
	result.x = boundingBox.x * scale.x;
	result.y = boundingBox.y * scale.y;
	result.z = boundingBox.z * scale.z;
	result.width = boundingBox.width * scale.x;
	result.height = boundingBox.height * scale.y;
	result.depth = boundingBox.depth * scale.z;
	return result;
}
bool BoxBasicallyEqual(box left, box right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x,      right.x,      tolerance)) { return false; }
	if (!BasicallyEqualR32(left.y,      right.y,      tolerance)) { return false; }
	if (!BasicallyEqualR32(left.z,      right.z,      tolerance)) { return false; }
	if (!BasicallyEqualR32(left.width,  right.width,  tolerance)) { return false; }
	if (!BasicallyEqualR32(left.height, right.height, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.depth,  right.depth,  tolerance)) { return false; }
	return true;
}

// +==============================+
// |             Boxi             |
// +==============================+
boxi BoxiShift(boxi boundingBox, i32 amountX, i32 amountY, i32 amountZ)
{
	boxi result;
	result.x = boundingBox.x + amountX;
	result.y = boundingBox.y + amountY;
	result.z = boundingBox.z + amountZ;
	result.size = boundingBox.size;
	return result;
}
boxi BoxiShift(boxi boundingBox, v3i amount)
{
	boxi result;
	result.x = boundingBox.x + amount.x;
	result.y = boundingBox.y + amount.y;
	result.z = boundingBox.z + amount.z;
	result.size = boundingBox.size;
	return result;
}
boxi BoxiScale(boxi boundingBox, i32 scalar)
{
	boxi result;
	result.x = boundingBox.x * scalar;
	result.y = boundingBox.y * scalar;
	result.z = boundingBox.z * scalar;
	result.width = boundingBox.width * scalar;
	result.height = boundingBox.height * scalar;
	result.depth = boundingBox.depth * scalar;
	return result;
}
boxi BoxiScale3(boxi boundingBox, i32 scaleX, i32 scaleY, i32 scaleZ)
{
	boxi result;
	result.x = boundingBox.x * scaleX;
	result.y = boundingBox.y * scaleY;
	result.z = boundingBox.z * scaleZ;
	result.width = boundingBox.width * scaleX;
	result.height = boundingBox.height * scaleY;
	result.depth = boundingBox.depth * scaleZ;
	return result;
}
boxi BoxiScale3(boxi boundingBox, v3i scale)
{
	boxi result;
	result.x = boundingBox.x * scale.x;
	result.y = boundingBox.y * scale.y;
	result.z = boundingBox.z * scale.z;
	result.width = boundingBox.width * scale.x;
	result.height = boundingBox.height * scale.y;
	result.depth = boundingBox.depth * scale.z;
	return result;
}
bool BoxiEqual(boxi left, boxi right)
{
	if (left.x      != right.x)      { return false; }
	if (left.y      != right.y)      { return false; }
	if (left.z      != right.z)      { return false; }
	if (left.width  != right.width)  { return false; }
	if (left.height != right.height) { return false; }
	if (left.depth  != right.depth)  { return false; }
	return true;
}

// +==============================+
// |            Obb2D             |
// +==============================+
obb2 Obb2DShift(obb2 boundingBox, r32 amountX, r32 amountY)
{
	obb2 result;
	result.x = boundingBox.x + amountX;
	result.y = boundingBox.y + amountY;
	result.size = boundingBox.size;
	result.rotation = boundingBox.rotation;
	return result;
}
obb2 Obb2DShift(obb2 boundingBox, v2 amount)
{
	obb2 result;
	result.x = boundingBox.x + amount.x;
	result.y = boundingBox.y + amount.y;
	result.size = boundingBox.size;
	result.rotation = boundingBox.rotation;
	return result;
}
obb2 Obb2DScale(obb2 boundingBox, r32 scalar)
{
	obb2 result;
	result.x = boundingBox.x * scalar;
	result.y = boundingBox.y * scalar;
	result.width = boundingBox.width * scalar;
	result.height = boundingBox.height * scalar;
	result.rotation = boundingBox.rotation;
	return result;
}
bool Obb2DBasicallyEqual(obb2 left, obb2 right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x,        right.x,        tolerance)) { return false; }
	if (!BasicallyEqualR32(left.y,        right.y,        tolerance)) { return false; }
	if (!BasicallyEqualR32(left.width,    right.width,    tolerance)) { return false; }
	if (!BasicallyEqualR32(left.height,   right.height,   tolerance)) { return false; }
	if (!BasicallyEqualR32(left.rotation, right.rotation, tolerance)) { return false; }
	return true;
}

// +==============================+
// |            Obb3D             |
// +==============================+
//TODO: Implement these functions

// +--------------------------------------------------------------+
// |                Expand Retract Inflate Deflate                |
// +--------------------------------------------------------------+
//Expand/Retract: +------+ (top left position stays the same)
//                ||---->|
//                ||     |
//                |V     |
//                +------+
//Inflate/Deflate: +------+ (total inflation/deflation is 2 times given value)
//                 |<^^^^>|
//                 |<    >|
//                 |<vvvv>|
//                 +------+

// +==============================+
// |             Rec              |
// +==============================+
rec RecExpand(rec rectangle, r32 extraWidth, r32 extraHeight)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height + extraHeight;
	return result;
}
rec RecExpand(rec rectangle, v2 extraSize)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraSize.width;
	result.height = rectangle.height + extraSize.height;
	return result;
}
rec RecExpandX(rec rectangle, r32 extraWidth)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height;
	return result;
}
rec RecExpandY(rec rectangle, r32 extraHeight)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight;
	return result;
}
rec RecExpandLeft(rec rectangle, r32 extraLeft)
{
	rec result;
	result.x = rectangle.x - extraLeft;
	result.y = rectangle.y;
	result.width = rectangle.width + extraLeft;
	result.height = rectangle.height;
	return result;
}
rec RecExpandRight(rec rectangle, r32 extraRight)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y;
	result.width = rectangle.width + extraRight;
	result.height = rectangle.height;
	return result;
}
rec RecExpandUp(rec rectangle, r32 extraUp)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y - extraUp;
	result.width = rectangle.width;
	result.height = rectangle.height + extraUp;
	return result;
}
rec RecExpandDown(rec rectangle, r32 extraDown)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y;
	result.width = rectangle.width;
	result.height = rectangle.height + extraDown;
	return result;
}
rec RecRetract(rec rectangle, r32 subWidth, r32 subHeight)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height - subHeight;
	return result;
}
rec RecRetract(rec rectangle, v2 subSize)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subSize.width;
	result.height = rectangle.height - subSize.height;
	return result;
}
rec RecRetractX(rec rectangle, r32 subWidth)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height;
	return result;
}
rec RecRetractY(rec rectangle, r32 subHeight)
{
	rec result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight;
	return result;
}
rec RecInflate(rec rectangle, r32 extraWidth, r32 extraHeight)
{
	rec result;
	result.x = rectangle.x - extraWidth;
	result.y = rectangle.y - extraHeight;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
rec RecInflate(rec rectangle, v2 extraSize)
{
	rec result;
	result.x = rectangle.x - extraSize.width;
	result.y = rectangle.y - extraSize.height;
	result.width = rectangle.width + extraSize.width*2;
	result.height = rectangle.height + extraSize.height*2;
	return result;
}
rec RecInflateX(rec rectangle, r32 extraWidth)
{
	rec result;
	result.x = rectangle.x - extraWidth;
	result.y = rectangle.y;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height;
	return result;
}
rec RecInflateY(rec rectangle, r32 extraHeight)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y - extraHeight;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
rec RecDeflate(rec rectangle, r32 subWidth, r32 subHeight)
{
	rec result;
	result.x = rectangle.x + subWidth;
	result.y = rectangle.y + subHeight;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height - subHeight*2;
	return result;
}
rec RecDeflate(rec rectangle, v2 extraSize)
{
	rec result;
	result.x = rectangle.x + extraSize.width;
	result.y = rectangle.y + extraSize.height;
	result.width = rectangle.width - extraSize.width*2;
	result.height = rectangle.height - extraSize.height*2;
	return result;
}
rec RecDeflateX(rec rectangle, r32 subWidth)
{
	rec result;
	result.x = rectangle.x + subWidth;
	result.y = rectangle.y;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height;
	return result;
}
rec RecDeflateY(rec rectangle, r32 subHeight)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y + subHeight;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight*2;
	return result;
}

// +==============================+
// |             Reci             |
// +==============================+
reci ReciExpand(reci rectangle, i32 extraWidth, i32 extraHeight)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height + extraHeight;
	return result;
}
reci ReciExpand(reci rectangle, v2i extraSize)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraSize.width;
	result.height = rectangle.height + extraSize.height;
	return result;
}
reci ReciExpandX(reci rectangle, i32 extraWidth)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height;
	return result;
}
reci ReciExpandY(reci rectangle, i32 extraHeight)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight;
	return result;
}
reci ReciExpandLeft(reci rectangle, i32 extraLeft)
{
	reci result;
	result.x = rectangle.x - extraLeft;
	result.y = rectangle.y;
	result.width = rectangle.width + extraLeft;
	result.height = rectangle.height;
	return result;
}
reci ReciExpandRight(reci rectangle, i32 extraRight)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y;
	result.width = rectangle.width + extraRight;
	result.height = rectangle.height;
	return result;
}
reci ReciExpandUp(reci rectangle, i32 extraUp)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y - extraUp;
	result.width = rectangle.width;
	result.height = rectangle.height + extraUp;
	return result;
}
reci ReciExpandDown(reci rectangle, i32 extraDown)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y;
	result.width = rectangle.width;
	result.height = rectangle.height + extraDown;
	return result;
}
reci ReciRetract(reci rectangle, i32 subWidth, i32 subHeight)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height - subHeight;
	return result;
}
reci ReciRetract(reci rectangle, v2i subSize)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subSize.width;
	result.height = rectangle.height - subSize.height;
	return result;
}
reci ReciRetractX(reci rectangle, i32 subWidth)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height;
	return result;
}
reci ReciRetractY(reci rectangle, i32 subHeight)
{
	reci result;
	result.topLeft = rectangle.topLeft;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight;
	return result;
}
reci ReciInflate(reci rectangle, i32 extraWidth, i32 extraHeight)
{
	reci result;
	result.x = rectangle.x - extraWidth;
	result.y = rectangle.y - extraHeight;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
reci ReciInflate(reci rectangle, v2i extraSize)
{
	reci result;
	result.x = rectangle.x - extraSize.width;
	result.y = rectangle.y - extraSize.height;
	result.width = rectangle.width + extraSize.width*2;
	result.height = rectangle.height + extraSize.height*2;
	return result;
}
reci ReciInflateX(reci rectangle, i32 extraWidth)
{
	reci result;
	result.x = rectangle.x - extraWidth;
	result.y = rectangle.y;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height;
	return result;
}
reci ReciInflateY(reci rectangle, i32 extraHeight)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y - extraHeight;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
reci ReciDeflate(reci rectangle, i32 subWidth, i32 subHeight)
{
	reci result;
	result.x = rectangle.x + subWidth;
	result.y = rectangle.y + subHeight;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height - subHeight*2;
	return result;
}
reci ReciDeflate(reci rectangle, v2i extraSize)
{
	reci result;
	result.x = rectangle.x + extraSize.width;
	result.y = rectangle.y + extraSize.height;
	result.width = rectangle.width - extraSize.width*2;
	result.height = rectangle.height - extraSize.height*2;
	return result;
}
reci ReciDeflateX(reci rectangle, i32 subWidth)
{
	reci result;
	result.x = rectangle.x + subWidth;
	result.y = rectangle.y;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height;
	return result;
}
reci ReciDeflateY(reci rectangle, i32 subHeight)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y + subHeight;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight*2;
	return result;
}

// +==============================+
// |             Box              |
// +==============================+
box BoxExpand(box boundingBox, r32 extraWidth, r32 extraHeight, r32 extraDepth)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width + extraWidth;
	result.height = boundingBox.height + extraHeight;
	result.depth = boundingBox.depth + extraDepth;
	return result;
}
box BoxExpand(box boundingBox, v3 extraSize)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width + extraSize.width;
	result.height = boundingBox.height + extraSize.height;
	result.depth = boundingBox.depth + extraSize.depth;
	return result;
}
box BoxExpandX(box boundingBox, r32 extraWidth)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width + extraWidth;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxExpandY(box boundingBox, r32 extraHeight)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height + extraHeight;
	result.depth = boundingBox.depth;
	return result;
}
box BoxExpandZ(box boundingBox, r32 extraDepth)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth + extraDepth;
	return result;
}
box BoxRetract(box boundingBox, r32 subWidth, r32 subHeight, r32 subDepth)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width - subWidth;
	result.height = boundingBox.height - subHeight;
	result.depth = boundingBox.depth - subDepth;
	return result;
}
box BoxRetract(box boundingBox, v3 subSize)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width - subSize.width;
	result.height = boundingBox.height - subSize.height;
	result.depth = boundingBox.depth - subSize.depth;
	return result;
}
box BoxRetractX(box boundingBox, r32 subWidth)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width - subWidth;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxRetractY(box boundingBox, r32 subHeight)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height - subHeight;
	result.depth = boundingBox.depth;
	return result;
}
box BoxRetractZ(box boundingBox, r32 subDepth)
{
	box result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth - subDepth;
	return result;
}
box BoxInflate(box boundingBox, r32 extraWidth, r32 extraHeight, r32 extraDepth)
{
	box result;
	result.x = boundingBox.x - extraWidth;
	result.y = boundingBox.y - extraHeight;
	result.z = boundingBox.z - extraDepth;
	result.width = boundingBox.width + extraWidth*2;
	result.height = boundingBox.height + extraHeight*2;
	result.depth = boundingBox.depth + extraDepth*2;
	return result;
}
box BoxInflate(box boundingBox, v3 extraSize)
{
	box result;
	result.x = boundingBox.x - extraSize.width;
	result.y = boundingBox.y - extraSize.height;
	result.z = boundingBox.z - extraSize.depth;
	result.width = boundingBox.width + extraSize.width*2;
	result.height = boundingBox.height + extraSize.height*2;
	result.depth = boundingBox.depth + extraSize.depth*2;
	return result;
}
box BoxInflateX(box boundingBox, r32 extraWidth)
{
	box result;
	result.x = boundingBox.x - extraWidth;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = boundingBox.width + extraWidth*2;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxInflateY(box boundingBox, r32 extraHeight)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y - extraHeight;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = boundingBox.height + extraHeight*2;
	result.depth = boundingBox.depth;
	return result;
}
box BoxInflateZ(box boundingBox, r32 extraDepth)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z - extraDepth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth + extraDepth*2;
	return result;
}
box BoxDeflate(box boundingBox, r32 subWidth, r32 subHeight, r32 subDepth)
{
	box result;
	result.x = boundingBox.x + subWidth;
	result.y = boundingBox.y + subHeight;
	result.z = boundingBox.z + subDepth;
	result.width = boundingBox.width - subWidth*2;
	result.height = boundingBox.height - subHeight*2;
	result.depth = boundingBox.depth - subDepth*2;
	return result;
}
box BoxDeflate(box boundingBox, v3 extraSize)
{
	box result;
	result.x = boundingBox.x + extraSize.width;
	result.y = boundingBox.y + extraSize.height;
	result.z = boundingBox.z + extraSize.depth;
	result.width = boundingBox.width - extraSize.width*2;
	result.height = boundingBox.height - extraSize.height*2;
	result.depth = boundingBox.depth - extraSize.depth*2;
	return result;
}
box BoxDeflateX(box boundingBox, r32 subWidth)
{
	box result;
	result.x = boundingBox.x + subWidth;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = boundingBox.width - subWidth*2;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxDeflateY(box boundingBox, r32 subHeight)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y + subHeight;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = boundingBox.height - subHeight*2;
	result.depth = boundingBox.depth;
	return result;
}
box BoxDeflateZ(box boundingBox, r32 subDepth)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z + subDepth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth - subDepth*2;
	return result;
}

// +==============================+
// |             Boxi             |
// +==============================+
boxi BoxiExpand(boxi boundingBox, i32 extraWidth, i32 extraHeight, i32 extraDepth)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width + extraWidth;
	result.height = boundingBox.height + extraHeight;
	result.depth = boundingBox.depth + extraDepth;
	return result;
}
boxi BoxiExpand(boxi boundingBox, v3i extraSize)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width + extraSize.width;
	result.height = boundingBox.height + extraSize.height;
	result.depth = boundingBox.depth + extraSize.depth;
	return result;
}
boxi BoxiExpandX(boxi boundingBox, i32 extraWidth)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width + extraWidth;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiExpandY(boxi boundingBox, i32 extraHeight)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height + extraHeight;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiExpandZ(boxi boundingBox, i32 extraDepth)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth + extraDepth;
	return result;
}
boxi BoxiRetract(boxi boundingBox, i32 subWidth, i32 subHeight, i32 subDepth)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width - subWidth;
	result.height = boundingBox.height - subHeight;
	result.depth = boundingBox.depth - subDepth;
	return result;
}
boxi BoxiRetract(boxi boundingBox, v3i subSize)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width - subSize.width;
	result.height = boundingBox.height - subSize.height;
	result.depth = boundingBox.depth - subSize.depth;
	return result;
}
boxi BoxiRetractX(boxi boundingBox, i32 subWidth)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width - subWidth;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiRetractY(boxi boundingBox, i32 subHeight)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height - subHeight;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiRetractZ(boxi boundingBox, i32 subDepth)
{
	boxi result;
	result.bottomLeft = boundingBox.bottomLeft;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth - subDepth;
	return result;
}
boxi BoxiInflate(boxi boundingBox, i32 extraWidth, i32 extraHeight, i32 extraDepth)
{
	boxi result;
	result.x = boundingBox.x - extraWidth;
	result.y = boundingBox.y - extraHeight;
	result.z = boundingBox.z - extraDepth;
	result.width = boundingBox.width + extraWidth*2;
	result.height = boundingBox.height + extraHeight*2;
	result.depth = boundingBox.depth + extraDepth*2;
	return result;
}
boxi BoxiInflate(boxi boundingBox, v3i extraSize)
{
	boxi result;
	result.x = boundingBox.x - extraSize.width;
	result.y = boundingBox.y - extraSize.height;
	result.z = boundingBox.z - extraSize.depth;
	result.width = boundingBox.width + extraSize.width*2;
	result.height = boundingBox.height + extraSize.height*2;
	result.depth = boundingBox.depth + extraSize.depth*2;
	return result;
}
boxi BoxiInflateX(boxi boundingBox, i32 extraWidth)
{
	boxi result;
	result.x = boundingBox.x - extraWidth;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = boundingBox.width + extraWidth*2;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiInflateY(boxi boundingBox, i32 extraHeight)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y - extraHeight;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = boundingBox.height + extraHeight*2;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiInflateZ(boxi boundingBox, i32 extraDepth)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z - extraDepth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth + extraDepth*2;
	return result;
}
boxi BoxiDeflate(boxi boundingBox, i32 subWidth, i32 subHeight, i32 subDepth)
{
	boxi result;
	result.x = boundingBox.x + subWidth;
	result.y = boundingBox.y + subHeight;
	result.z = boundingBox.z + subDepth;
	result.width = boundingBox.width - subWidth*2;
	result.height = boundingBox.height - subHeight*2;
	result.depth = boundingBox.depth - subDepth*2;
	return result;
}
boxi BoxiDeflate(boxi boundingBox, v3i extraSize)
{
	boxi result;
	result.x = boundingBox.x + extraSize.width;
	result.y = boundingBox.y + extraSize.height;
	result.z = boundingBox.z + extraSize.depth;
	result.width = boundingBox.width - extraSize.width*2;
	result.height = boundingBox.height - extraSize.height*2;
	result.depth = boundingBox.depth - extraSize.depth*2;
	return result;
}
boxi BoxiDeflateX(boxi boundingBox, i32 subWidth)
{
	boxi result;
	result.x = boundingBox.x + subWidth;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = boundingBox.width - subWidth*2;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiDeflateY(boxi boundingBox, i32 subHeight)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y + subHeight;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = boundingBox.height - subHeight*2;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiDeflateZ(boxi boundingBox, i32 subDepth)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z + subDepth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth - subDepth*2;
	return result;
}

// +==============================+
// |            Obb2D             |
// +==============================+
//TODO: Implement these functions properly by moving the center with correct respect for rotation
#if 0
obb2 Obb2DExpand(obb2 rectangle, r32 extraWidth, r32 extraHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height + extraHeight;
	return result;
}
obb2 Obb2DExpand(obb2 rectangle, v2 extraSize)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraSize.width;
	result.height = rectangle.height + extraSize.height;
	return result;
}
obb2 Obb2DExpandX(obb2 rectangle, r32 extraWidth)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraWidth;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DExpandY(obb2 rectangle, r32 extraHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight;
	return result;
}
obb2 Obb2DRetract(obb2 rectangle, r32 subWidth, r32 subHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height - subHeight;
	return result;
}
obb2 Obb2DRetract(obb2 rectangle, v2 subSize)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subSize.width;
	result.height = rectangle.height - subSize.height;
	return result;
}
obb2 Obb2DRetractX(obb2 rectangle, r32 subWidth)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subWidth;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DRetractY(obb2 rectangle, r32 subHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight;
	return result;
}
#endif
obb2 Obb2DInflate(obb2 rectangle, r32 extraWidth, r32 extraHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
obb2 Obb2DInflate(obb2 rectangle, v2 extraSize)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraSize.width*2;
	result.height = rectangle.height + extraSize.height*2;
	return result;
}
obb2 Obb2DInflateX(obb2 rectangle, r32 extraWidth)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width + extraWidth*2;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DInflateY(obb2 rectangle, r32 extraHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = rectangle.height + extraHeight*2;
	return result;
}
obb2 Obb2DDeflate(obb2 rectangle, r32 subWidth, r32 subHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height - subHeight*2;
	return result;
}
obb2 Obb2DDeflate(obb2 rectangle, v2 extraSize)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - extraSize.width*2;
	result.height = rectangle.height - extraSize.height*2;
	return result;
}
obb2 Obb2DDeflateX(obb2 rectangle, r32 subWidth)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width - subWidth*2;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DDeflateY(obb2 rectangle, r32 subHeight)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = rectangle.height - subHeight*2;
	return result;
}

//TODO: Add Obb3D functions

// +--------------------------------------------------------------+
// |                 Other Manipulation Functions                 |
// +--------------------------------------------------------------+
// +==============================+
// |             Rec              |
// +==============================+
rec RecSquarify(rec rectangle, bool makeLarger = true, bool center = true)
{
	rec result;
	r32 newSize = (makeLarger ? MaxR32(rectangle.width, rectangle.height) : MinR32(rectangle.width, rectangle.height));
	result.x = rectangle.x - (center ? (newSize - rectangle.width)/2 : 0);
	result.y = rectangle.y - (center ? (newSize - rectangle.height)/2 : 0);
	result.width = newSize;
	result.height = newSize;
	return result;
}
rec RecInvert(rec rectangle)
{
	rec result;
	result.x = rectangle.x + rectangle.width;
	result.y = rectangle.y + rectangle.height;
	result.width = -rectangle.width;
	result.height = -rectangle.height;
	return result;
}
rec RecInvertX(rec rectangle)
{
	rec result;
	result.x = rectangle.x + rectangle.width;
	result.y = rectangle.y;
	result.width = -rectangle.width;
	result.height = rectangle.height;
	return result;
}
rec RecInvertY(rec rectangle)
{
	rec result;
	result.x = rectangle.x;
	result.y = rectangle.y + rectangle.height;
	result.width = rectangle.width;
	result.height = -rectangle.height;
	return result;
}
rec RecUninvert(rec rectangle)
{
	rec result;
	result.x = MinR32(rectangle.x, rectangle.x + rectangle.width);
	result.y = MinR32(rectangle.y, rectangle.y + rectangle.height);
	result.width = AbsR32(rectangle.width);
	result.height = AbsR32(rectangle.height);
	return result;
}
rec RecBoth(rec rectangle1, rec rectangle2)
{
	rec result;
	result.x = MinR32(rectangle1.x, rectangle2.x);
	result.y = MinR32(rectangle1.y, rectangle2.y);
	result.width = MaxR32(rectangle1.x + rectangle1.width, rectangle2.x + rectangle2.width) - result.x;
	result.height = MaxR32(rectangle1.y + rectangle1.height, rectangle2.y + rectangle2.height) - result.y;
	return result;
}
rec RecExpandToVec2(rec rectangle, v2 newPoint)
{
	rec result;
	result.x = MinR32(rectangle.x, newPoint.x);
	result.y = MinR32(rectangle.y, newPoint.y);
	result.width = MaxR32(rectangle.x + rectangle.width, newPoint.x) - result.x;
	result.height = MaxR32(rectangle.y + rectangle.height, newPoint.y) - result.y;
	return result;
}
rec RecOverlap(rec rectangle1, rec rectangle2)
{
	r32 minX = MaxR32(rectangle1.x, rectangle2.x);
	r32 minY = MaxR32(rectangle1.y, rectangle2.y);
	r32 maxX = MinR32(rectangle1.x+rectangle1.width, rectangle2.x+rectangle2.width);
	r32 maxY = MinR32(rectangle1.y+rectangle1.height, rectangle2.y+rectangle2.height);
	rec result = NewRec(minX, minY, MaxR32(maxX-minX, 0), MaxR32(maxY-minY, 0));
	return result;
}

// +==============================+
// |             Reci             |
// +==============================+
reci ReciSquarify(reci rectangle, bool makeLarger = true)
{
	reci result;
	i32 newSize = (makeLarger ? MaxI32(rectangle.width, rectangle.height) : MinI32(rectangle.width, rectangle.height));
	result.x = rectangle.x;
	result.y = rectangle.y;
	result.width = newSize;
	result.height = newSize;
	return result;
}
reci ReciInvert(reci rectangle)
{
	reci result;
	result.x = rectangle.x + rectangle.width;
	result.y = rectangle.y + rectangle.height;
	result.width = -rectangle.width;
	result.height = -rectangle.height;
	return result;
}
reci ReciInvertX(reci rectangle)
{
	reci result;
	result.x = rectangle.x + rectangle.width;
	result.y = rectangle.y;
	result.width = -rectangle.width;
	result.height = rectangle.height;
	return result;
}
reci ReciInvertY(reci rectangle)
{
	reci result;
	result.x = rectangle.x;
	result.y = rectangle.y + rectangle.height;
	result.width = rectangle.width;
	result.height = -rectangle.height;
	return result;
}
reci ReciUninvert(reci rectangle)
{
	reci result;
	result.x = MinI32(rectangle.x, rectangle.x + rectangle.width);
	result.y = MinI32(rectangle.y, rectangle.y + rectangle.height);
	result.width = AbsI32(rectangle.width);
	result.height = AbsI32(rectangle.height);
	return result;
}
reci ReciBoth(reci rectangle1, reci rectangle2)
{
	reci result;
	result.x = MinI32(rectangle1.x, rectangle2.x);
	result.y = MinI32(rectangle1.y, rectangle2.y);
	result.width = MaxI32(rectangle1.x + rectangle1.width, rectangle2.x + rectangle2.width) - result.x;
	result.height = MaxI32(rectangle1.y + rectangle1.height, rectangle2.y + rectangle2.height) - result.y;
	return result;
}
reci ReciExpandToVec2i(reci rectangle, v2i newPoint)
{
	reci result;
	result.x = MinI32(rectangle.x, newPoint.x);
	result.y = MinI32(rectangle.y, newPoint.y);
	result.width = MaxI32(rectangle.x + rectangle.width, newPoint.x) - result.x;
	result.height = MaxI32(rectangle.y + rectangle.height, newPoint.y) - result.y;
	return result;
}
reci ReciOverlap(reci rectangle1, reci rectangle2)
{
	i32 minX = MaxI32(rectangle1.x, rectangle2.x);
	i32 minY = MaxI32(rectangle1.y, rectangle2.y);
	i32 maxX = MinI32(rectangle1.x+rectangle1.width, rectangle2.x+rectangle2.width);
	i32 maxY = MinI32(rectangle1.y+rectangle1.height, rectangle2.y+rectangle2.height);
	reci result = NewReci(minX, minY, MaxI32(maxX-minX, 0), MaxI32(maxY-minY, 0));
	return result;
}


// +==============================+
// |             Box              |
// +==============================+
box BoxCubify(box boundingBox, bool makeLarger = true, bool center = true)
{
	box result;
	r32 newSize = (makeLarger ? MaxR32(boundingBox.width, boundingBox.height, boundingBox.depth) : MinR32(boundingBox.width, boundingBox.height, boundingBox.depth));
	result.x = boundingBox.x - (center ? (newSize - boundingBox.width)/2 : 0);
	result.y = boundingBox.y - (center ? (newSize - boundingBox.height)/2 : 0);
	result.z = boundingBox.z - (center ? (newSize - boundingBox.depth)/2 : 0);
	result.width = newSize;
	result.height = newSize;
	result.depth = newSize;
	return result;
}
box BoxInvert(box boundingBox)
{
	box result;
	result.x = boundingBox.x + boundingBox.width;
	result.y = boundingBox.y + boundingBox.height;
	result.z = boundingBox.z + boundingBox.depth;
	result.width = -boundingBox.width;
	result.height = -boundingBox.height;
	result.depth = -boundingBox.depth;
	return result;
}
box BoxInvertX(box boundingBox)
{
	box result;
	result.x = boundingBox.x + boundingBox.width;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = -boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxInvertY(box boundingBox)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y + boundingBox.height;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = -boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
box BoxInvertZ(box boundingBox)
{
	box result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z + boundingBox.depth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = -boundingBox.depth;
	return result;
}
box BoxUninvert(box boundingBox)
{
	box result;
	result.x = MinR32(boundingBox.x, boundingBox.x + boundingBox.width);
	result.y = MinR32(boundingBox.y, boundingBox.y + boundingBox.height);
	result.z = MinR32(boundingBox.z, boundingBox.z + boundingBox.depth);
	result.width = AbsR32(boundingBox.width);
	result.height = AbsR32(boundingBox.height);
	result.depth = AbsR32(boundingBox.depth);
	return result;
}
//TODO: Add BoxBoth BoxOverlap, and BoxExpandToVec3

// +==============================+
// |             Boxi             |
// +==============================+
boxi BoxiCubify(boxi boundingBox, bool makeLarger = true)
{
	boxi result;
	i32 newSize = (makeLarger ? MaxI32(boundingBox.width, boundingBox.height, boundingBox.depth) : MinI32(boundingBox.width, boundingBox.height, boundingBox.depth));
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = newSize;
	result.height = newSize;
	result.depth = newSize;
	return result;
}
boxi BoxiInvert(boxi boundingBox)
{
	boxi result;
	result.x = boundingBox.x + boundingBox.width;
	result.y = boundingBox.y + boundingBox.height;
	result.z = boundingBox.z + boundingBox.depth;
	result.width = -boundingBox.width;
	result.height = -boundingBox.height;
	result.depth = -boundingBox.depth;
	return result;
}
boxi BoxiInvertX(boxi boundingBox)
{
	boxi result;
	result.x = boundingBox.x + boundingBox.width;
	result.y = boundingBox.y;
	result.z = boundingBox.z;
	result.width = -boundingBox.width;
	result.height = boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiInvertY(boxi boundingBox)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y + boundingBox.height;
	result.z = boundingBox.z;
	result.width = boundingBox.width;
	result.height = -boundingBox.height;
	result.depth = boundingBox.depth;
	return result;
}
boxi BoxiInvertZ(boxi boundingBox)
{
	boxi result;
	result.x = boundingBox.x;
	result.y = boundingBox.y;
	result.z = boundingBox.z + boundingBox.depth;
	result.width = boundingBox.width;
	result.height = boundingBox.height;
	result.depth = -boundingBox.depth;
	return result;
}
boxi BoxiUninvert(boxi boundingBox)
{
	boxi result;
	result.x = MinI32(boundingBox.x, boundingBox.x + boundingBox.width);
	result.y = MinI32(boundingBox.y, boundingBox.y + boundingBox.height);
	result.z = MinI32(boundingBox.z, boundingBox.z + boundingBox.depth);
	result.width = AbsI32(boundingBox.width);
	result.height = AbsI32(boundingBox.height);
	result.depth = AbsI32(boundingBox.depth);
	return result;
}
boxi BoxiExpandToVec3i(boxi boundingBox, v3i newPoint)
{
	boxi result;
	result.x = MinI32(boundingBox.x, newPoint.x);
	result.y = MinI32(boundingBox.y, newPoint.y);
	result.z = MinI32(boundingBox.z, newPoint.z);
	result.width  = MaxI32(boundingBox.x + boundingBox.width, newPoint.x) - result.x;
	result.height = MaxI32(boundingBox.y + boundingBox.height, newPoint.y) - result.y;
	result.depth  = MaxI32(boundingBox.z + boundingBox.depth, newPoint.z) - result.z;
	return result;
}
boxi BoxiBoth(boxi boundingBox1, boxi boundingBox2)
{
	boxi result;
	result.x = MinI32(boundingBox1.x, boundingBox2.x);
	result.y = MinI32(boundingBox1.y, boundingBox2.y);
	result.z = MinI32(boundingBox1.z, boundingBox2.z);
	result.width = MaxI32(boundingBox1.x + boundingBox1.width, boundingBox2.x + boundingBox2.width) - result.x;
	result.height = MaxI32(boundingBox1.y + boundingBox1.height, boundingBox2.y + boundingBox2.height) - result.y;
	result.depth = MaxI32(boundingBox1.z + boundingBox1.depth, boundingBox2.z + boundingBox2.depth) - result.z;
	return result;
}
//TODO: Add BoxiOverlap

// +==============================+
// |            Obb2D             |
// +==============================+
obb2 Obb2DSquarify(obb2 rectangle, bool makeLarger = true)
{
	obb2 result;
	r32 newSize = (makeLarger ? MaxR32(rectangle.width, rectangle.height) : MinR32(rectangle.width, rectangle.height));
	result.center = rectangle.center;
	result.width = newSize;
	result.height = newSize;
	return result;
}
obb2 Obb2DInvert(obb2 rectangle)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = -rectangle.width;
	result.height = -rectangle.height;
	return result;
}
obb2 Obb2DInvertX(obb2 rectangle)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = -rectangle.width;
	result.height = rectangle.height;
	return result;
}
obb2 Obb2DInvertY(obb2 rectangle)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = rectangle.width;
	result.height = -rectangle.height;
	return result;
}
obb2 Obb2DUninvert(obb2 rectangle)
{
	obb2 result;
	result.center = rectangle.center;
	result.width = AbsR32(rectangle.width);
	result.height = AbsR32(rectangle.height);
	return result;
}
rec GetObb2DAlignedBounds(obb2 boundingBox) //TODO: Remove "Aligned" from the name to make it shorter?
{
	rec result = NewRec(GetObb2DTopLeft(boundingBox), Vec2_Zero);
	result = RecExpandToVec2(result, GetObb2DTopRight(boundingBox));
	result = RecExpandToVec2(result, GetObb2DBottomLeft(boundingBox));
	result = RecExpandToVec2(result, GetObb2DBottomRight(boundingBox));
	return result;
}

//TODO: Add Obb3D functions

// +--------------------------------------------------------------+
// |                Collision and Inclusion Tests                 |
// +--------------------------------------------------------------+
// +==============================+
// |             Rec              |
// +==============================+
bool IsInsideRec(rec rectangle, v2 point) //TODO: Add includeNeg and includePos options!
{
	if (point.x < rectangle.x) { return false; }
	if (point.y < rectangle.y) { return false; }
	if (point.x >= rectangle.x + rectangle.width) { return false; }
	if (point.y >= rectangle.y + rectangle.height) { return false; }
	return true;
}
bool IsInsideRec(rec rectangle, v2i point)
{
	return IsInsideRec(rectangle, ToVec2(point));
}

bool RecsIntersect(rec rectangle1, rec rectangle2, bool inclusive = true)
{
	if (inclusive)
	{
		if (rectangle1.x <= rectangle2.x + rectangle2.width &&
			rectangle1.x + rectangle1.width >= rectangle2.x &&
			rectangle1.y <= rectangle2.y + rectangle2.height &&
			rectangle1.y + rectangle1.height >= rectangle2.y)
		{
			return true;
		}
	}
	else
	{
		if (rectangle1.x < rectangle2.x + rectangle2.width &&
			rectangle1.x + rectangle1.width > rectangle2.x &&
			rectangle1.y < rectangle2.y + rectangle2.height &&
			rectangle1.y + rectangle1.height > rectangle2.y)
		{
			return true;
		}
	}
	return false;
}

// +==============================+
// |             Reci             |
// +==============================+
bool IsInsideReci(reci rectangle, v2i point, bool includePositiveEdges = false) //TODO: Add includeNeg and rename includePos option
{
	if (point.x < rectangle.x) { return false; }
	if (point.y < rectangle.y) { return false; }
	if (point.x > rectangle.x + rectangle.width) { return false; }
	if (point.y > rectangle.y + rectangle.height) { return false; }
	if (!includePositiveEdges && point.x == rectangle.x + rectangle.width) { return false; }
	if (!includePositiveEdges && point.y == rectangle.y + rectangle.height) { return false; }
	return true;
}

bool RecisIntersect(reci rectangle1, reci rectangle2, bool inclusive = true)
{
	if (inclusive)
	{
		if (rectangle1.x <= rectangle2.x + rectangle2.width &&
			rectangle1.x + rectangle1.width >= rectangle2.x &&
			rectangle1.y <= rectangle2.y + rectangle2.height &&
			rectangle1.y + rectangle1.height >= rectangle2.y)
		{
			return true;
		}
	}
	else
	{
		if (rectangle1.x < rectangle2.x + rectangle2.width &&
			rectangle1.x + rectangle1.width > rectangle2.x &&
			rectangle1.y < rectangle2.y + rectangle2.height &&
			rectangle1.y + rectangle1.height > rectangle2.y)
		{
			return true;
		}
	}
	return false;
}

// +==============================+
// |             Box              |
// +==============================+
bool IsInsideBox(box boundingBox, v3 point)
{
	if (point.x < boundingBox.x) { return false; }
	if (point.y < boundingBox.y) { return false; }
	if (point.z < boundingBox.z) { return false; }
	if (point.x >= boundingBox.x + boundingBox.width) { return false; }
	if (point.y >= boundingBox.y + boundingBox.height) { return false; }
	if (point.z >= boundingBox.z + boundingBox.depth) { return false; }
	return true;
}
bool IsInsideBox(box boundingBox, v3i point)
{
	return IsInsideBox(boundingBox, ToVec3(point));
}

bool BoxesIntersect(box boundingBox1, box boundingBox2, bool inclusive = true)
{
	if (inclusive)
	{
		if (boundingBox1.x <= boundingBox2.x + boundingBox2.width &&
			boundingBox1.x + boundingBox1.width >= boundingBox2.x &&
			boundingBox1.y <= boundingBox2.y + boundingBox2.height &&
			boundingBox1.y + boundingBox1.height >= boundingBox2.y &&
			boundingBox1.z <= boundingBox2.z + boundingBox2.depth &&
			boundingBox1.z + boundingBox1.depth >= boundingBox2.z)
		{
			return true;
		}
	}
	else
	{
		if (boundingBox1.x < boundingBox2.x + boundingBox2.width &&
			boundingBox1.x + boundingBox1.width > boundingBox2.x &&
			boundingBox1.y < boundingBox2.y + boundingBox2.height &&
			boundingBox1.y + boundingBox1.height > boundingBox2.y &&
			boundingBox1.z < boundingBox2.z + boundingBox2.depth &&
			boundingBox1.z + boundingBox1.depth > boundingBox2.z)
		{
			return true;
		}
	}
	return false;
}

// +==============================+
// |             Boxi             |
// +==============================+
bool IsInsideBoxi(boxi boundingBox, v3i point, bool includePositiveEdges = false)
{
	if (point.x < boundingBox.x) { return false; }
	if (point.y < boundingBox.y) { return false; }
	if (point.z < boundingBox.z) { return false; }
	if (point.x > boundingBox.x + boundingBox.width) { return false; }
	if (point.y > boundingBox.y + boundingBox.height) { return false; }
	if (point.z > boundingBox.z + boundingBox.depth) { return false; }
	if (!includePositiveEdges && point.x == boundingBox.x + boundingBox.width) { return false; }
	if (!includePositiveEdges && point.y == boundingBox.y + boundingBox.height) { return false; }
	if (!includePositiveEdges && point.z == boundingBox.z + boundingBox.depth) { return false; }
	return true;
}

bool BoxisIntersect(boxi boundingBox1, boxi boundingBox2, bool inclusive = true)
{
	if (inclusive)
	{
		if (boundingBox1.x <= boundingBox2.x + boundingBox2.width &&
			boundingBox1.x + boundingBox1.width >= boundingBox2.x &&
			boundingBox1.y <= boundingBox2.y + boundingBox2.height &&
			boundingBox1.y + boundingBox1.height >= boundingBox2.y &&
			boundingBox1.z <= boundingBox2.z + boundingBox2.depth &&
			boundingBox1.z + boundingBox1.depth >= boundingBox2.z)
		{
			return true;
		}
	}
	else
	{
		if (boundingBox1.x < boundingBox2.x + boundingBox2.width &&
			boundingBox1.x + boundingBox1.width > boundingBox2.x &&
			boundingBox1.y < boundingBox2.y + boundingBox2.height &&
			boundingBox1.y + boundingBox1.height > boundingBox2.y &&
			boundingBox1.z < boundingBox2.z + boundingBox2.depth &&
			boundingBox1.z + boundingBox1.depth > boundingBox2.z)
		{
			return true;
		}
	}
	return false;
}

// +==============================+
// |            Obb2D             |
// +==============================+
bool IsInsideObb2D(obb2 rectangle, v2 point)
{
	if (rectangle.rotation == 0) { return IsInsideRec(NewRec(rectangle.center - rectangle.size/2, rectangle.size), point); } //TODO: Is this really useful?? Let's time it!
	r32 cosValue = CosR32(rectangle.rotation);
	r32 sinValue = SinR32(rectangle.rotation);
	r32 dotX = Vec2Dot(point - rectangle.center, NewVec2(cosValue, sinValue));
	r32 dotY = Vec2Dot(point - rectangle.center, NewVec2(-sinValue, cosValue));
	if (dotX >= rectangle.width/2) { return false; }
	if (dotY >= rectangle.height/2) { return false; }
	if (dotX < -rectangle.width/2) { return false; }
	if (dotY < -rectangle.height/2) { return false; }
	return true;
}

// TODO: Add Obb2sIntersect

//TODO: Add Obb3D functions

// +--------------------------------------------------------------+
// |             Quickhand Functions for Layout Code              |
// +--------------------------------------------------------------+
void RecAlign(rec* rectangleOut, r32 alignmentScale = 1.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	if (alignmentScale > 0)
	{
		//NOTE: We do something a little more special than just rounding in this scenario
		//      We might round up the width/height even if on their own they would round down but
		//      if the x/y coordinate contributes enough it can round to the closest value up
		r32 resultX = RoundR32(rectangleOut->x * alignmentScale) / alignmentScale;
		r32 resultY = RoundR32(rectangleOut->y * alignmentScale) / alignmentScale;
		r32 resultWidth = RoundR32((rectangleOut->x + rectangleOut->width) * alignmentScale) / alignmentScale - resultX;
		r32 resultHeight = RoundR32((rectangleOut->y + rectangleOut->height) * alignmentScale) / alignmentScale - resultY;
		rectangleOut->x = resultX;
		rectangleOut->y = resultY;
		rectangleOut->width = resultWidth;
		rectangleOut->height = resultHeight;
	}
}
void RecAlignTopLeft(rec* rectangleOut, r32 alignmentScale = 1.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	if (alignmentScale > 0)
	{
		rectangleOut->x = RoundR32(rectangleOut->x * alignmentScale) / alignmentScale;
		rectangleOut->y = RoundR32(rectangleOut->y * alignmentScale) / alignmentScale;
	}
}
void RecAlignSize(rec* rectangleOut, r32 alignmentScale = 1.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	if (alignmentScale > 0)
	{
		rectangleOut->width = RoundR32(rectangleOut->width * alignmentScale) / alignmentScale;
		rectangleOut->height = RoundR32(rectangleOut->height * alignmentScale) / alignmentScale;
	}
}

//This assumes the width is already calculated
void RecLayoutLeftOf(rec* rectangleOut, r32 posOfThingToRight, r32 rightPadding = 0.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = posOfThingToRight - rightPadding - rectangleOut->width;
}
//This assumes the height is already calculated
void RecLayoutTopOf(rec* rectangleOut, r32 posOfThingDownwards, r32 bottomPadding = 0.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->y = posOfThingDownwards - bottomPadding - rectangleOut->height;
}

void RecLayoutRightOf(rec* rectangleOut, r32 posOfThingToLeft, r32 leftPadding = 0.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = posOfThingToLeft + leftPadding;
}
void RecLayoutRightOf(rec* rectangleOut, rec recToLeft, r32 leftPadding = 0.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = recToLeft.x + recToLeft.width + leftPadding;
}
void RecLayoutBottomOf(rec* rectangleOut, r32 posOfThingUpwards, r32 topPadding = 0.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->y = posOfThingUpwards + topPadding;
}
void RecLayoutBottomOf(rec* rectangleOut, rec recUpwards, r32 topPadding = 0.0f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->y = recUpwards.y + recUpwards.height + topPadding;
}

void RecLayoutBetweenX(rec* rectangleOut, r32 leftSide, r32 rightSide, r32 leftMargin = 0, r32 rightMargin = 0, r32 minWidth = 0)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = leftSide + leftMargin;
	rectangleOut->width = MaxR32(minWidth, rightSide - rightMargin - rectangleOut->x);
}
void RecLayoutBetweenY(rec* rectangleOut, r32 topSide, r32 bottomSide, r32 topMargin = 0, r32 bottomMargin = 0, r32 minHeight = 0)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->y = topSide + topMargin;
	rectangleOut->height = MaxR32(minHeight, bottomSide - bottomMargin - rectangleOut->y);
}

void RecLayoutLeftPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionWidth, bool shrinkOtherRec = false, r32 offsetAmount = 0)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = otherRectangle->x + offsetAmount;
	rectangleOut->y = otherRectangle->y;
	rectangleOut->width = portionWidth;
	rectangleOut->height = otherRectangle->height;
	if (shrinkOtherRec)
	{
		otherRectangle->x += portionWidth + offsetAmount;
		otherRectangle->width -= portionWidth + offsetAmount;
	}
}
void RecLayoutRightPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionWidth, bool shrinkOtherRec = false, r32 offsetAmount = 0)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = otherRectangle->x + otherRectangle->width - portionWidth - offsetAmount;
	rectangleOut->y = otherRectangle->y;
	rectangleOut->width = portionWidth;
	rectangleOut->height = otherRectangle->height;
	if (shrinkOtherRec)
	{
		otherRectangle->width -= portionWidth + offsetAmount;
	}
}
void RecLayoutTopPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionHeight, bool shrinkOtherRec = false, r32 offsetAmount = 0)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = otherRectangle->x;
	rectangleOut->y = otherRectangle->y + offsetAmount;
	rectangleOut->width = otherRectangle->width;
	rectangleOut->height = portionHeight;
	if (shrinkOtherRec)
	{
		otherRectangle->y += portionHeight + offsetAmount;
		otherRectangle->height -= portionHeight + offsetAmount;
	}
}
void RecLayoutBottomPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionHeight, bool shrinkOtherRec = false, r32 offsetAmount = 0)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = otherRectangle->x;
	rectangleOut->y = otherRectangle->y + otherRectangle->height - portionHeight - offsetAmount;
	rectangleOut->width = otherRectangle->width;
	rectangleOut->height = portionHeight;
	if (shrinkOtherRec)
	{
		otherRectangle->height -= portionHeight + offsetAmount;
	}
}

//This assumes height has already been calculated
void RecLayoutVerticalCenter(rec* rectangleOut, r32 posToCenterTo, r32 percentage = 0.5f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->y = posToCenterTo - rectangleOut->height*percentage;
}
//This assumes height has already been calculated
void RecLayoutVerticalCenter(rec* rectangleOut, rec otherRectangle, r32 percentage = 0.5f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->y = otherRectangle.y + otherRectangle.height*percentage - rectangleOut->height*percentage;
}
//This assumes width has already been calculated
void RecLayoutHorizontalCenter(rec* rectangleOut, r32 posToCenterTo, r32 percentage = 0.5f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = posToCenterTo - rectangleOut->width*percentage;
}
void RecLayoutHorizontalCenter(rec* rectangleOut, rec otherRectangle, r32 percentage = 0.5f)
{
	DebugAssert_(rectangleOut != nullptr);
	rectangleOut->x = otherRectangle.x + otherRectangle.width*percentage - rectangleOut->width*percentage;
}

// +--------------------------------------------------------------+
// |                 Other Complicated Functions                  |
// +--------------------------------------------------------------+
obb2 Obb2Line(v2 start, v2 end, r32 thickness)
{
	obb2 result;
	result.center = (start + end) / 2;
	result.width = Vec2Length(end - start);
	result.height = thickness;
	result.rotation = AtanR32(end.y - start.y, end.x - start.x);
	if (result.rotation < 0) { result.rotation += TwoPi32; }
	return result;
}

// +--------------------------------------------------------------+
// |                      Operator Overloads                      |
// +--------------------------------------------------------------+
rec operator + (rec rectangle, v2 vector) { return RecShift(rectangle, vector); }
rec operator - (rec rectangle, v2 vector) { return RecShift(rectangle, -vector); }
rec operator * (rec rectangle, r32 scale) { return RecScale(rectangle, scale); }
rec operator / (rec rectangle, r32 scale) { return RecScale(rectangle, 1/scale); }

reci operator + (reci rectangle, v2i vector) { return ReciShift(rectangle, vector); }
reci operator - (reci rectangle, v2i vector) { return ReciShift(rectangle, -vector); }
reci operator * (reci rectangle, i32 scale)  { return ReciScale(rectangle, scale); }
reci operator / (reci rectangle, i32 scale)  { return ReciScale(rectangle, 1/scale); }
bool operator == (reci left, reci right) { return  (left.x == right.x && left.y == right.y && left.width == right.width && left.height == right.height); }
bool operator != (reci left, reci right) { return !(left.x == right.x && left.y == right.y && left.width == right.width && left.height == right.height); }

box operator + (box boundingBox, v3 vector) { return BoxShift(boundingBox, vector); }
box operator - (box boundingBox, v3 vector) { return BoxShift(boundingBox, -vector); }
box operator * (box boundingBox, r32 scale) { return BoxScale(boundingBox, scale); }
box operator / (box boundingBox, r32 scale) { return BoxScale(boundingBox, 1/scale); }

boxi operator + (boxi boundingBox, v3i vector) { return BoxiShift(boundingBox, vector); }
boxi operator - (boxi boundingBox, v3i vector) { return BoxiShift(boundingBox, -vector); }
boxi operator * (boxi boundingBox, i32 scale)  { return BoxiScale(boundingBox, scale); }
boxi operator / (boxi boundingBox, i32 scale)  { return BoxiScale(boundingBox, 1/scale); }
bool operator == (boxi left, boxi right) { return  (left.x == right.x && left.y == right.y && left.z == right.z && left.width == right.width && left.height == right.height && left.depth == right.depth); }
bool operator != (boxi left, boxi right) { return !(left.x == right.x && left.y == right.y && left.z == right.z && left.width == right.width && left.height == right.height && left.depth == right.depth); }

obb2 operator + (obb2 box, v2 vector) { return Obb2DShift(box, vector); }
obb2 operator - (obb2 box, v2 vector) { return Obb2DShift(box, -vector); }
obb2 operator * (obb2 box, r32 scale) { return Obb2DScale(box, scale); }
obb2 operator / (obb2 box, r32 scale) { return Obb2DScale(box, 1/scale); }

//TODO: Add operator overloads for obb3

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_RECTANGLES_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
Rec_Zero
Rec_Default
Reci_Zero
Box_Zero
Boxi_Zero
Obb2_Zero
@Types
Rectangle_t
Rectanglei_t
Box_t
Boxi_t
Obb2D_t
rec
reci
obb2
obb3
box
boxi
@Functions
rec NewRec(v2 topLeft, v2 size)
rec NewRecCentered(v2 center, v2 size)
rec NewRecBetween(r32 point1X, r32 point1Y, r32 point2X, r32 point2Y)
reci NewReci(v2i topLeft, v2i size)
box NewBox(v3 bottomLeft, v3 size)
box NewBoxCentered(v3 center, v3 size)
boxi NewBoxi(v3i bottomLeft, v3i size)
boxi NewBoxiBetween(v3i point1, v3i point2)
obb2 NewObb2D(v2 center, v2 size, r32 rotation)
rec ToRec(reci rectangle)
obb2 ToObb2D(rec rectangle)
box ToBox(boxi boundingBox)
obb3 ToObb3D(box boundingBox)
v2 GetObb2DRelativePos(obb2 boundingBox, v2 point)
v2 GetObb2DWorldPoint(obb2 boundingBox, v2 relativeOffset)
v2 GetObb2DRightVec(obb2 boundingBox)
v2 GetObb2DDownVec(obb2 boundingBox)
v2 GetObb2DLeftVec(obb2 boundingBox)
v2 GetObb2DUpVec(obb2 boundingBox)
v2 GetObb2DTopLeft(obb2 boundingBox)
v2 GetObb2DTopRight(obb2 boundingBox)
v2 GetObb2DBottomLeft(obb2 boundingBox)
v2 GetObb2DBottomRight(obb2 boundingBox)
rec RecShift(rec rectangle, r32 amountX, r32 amountY)
rec RecScale(rec rectangle, r32 scalar)
rec RecScale2(rec rectangle, r32 scaleX, r32 scaleY)
bool RecBasicallyEqual(rec left, rec right, r32 tolerance = 0.001f)
reci ReciShift(reci rectangle, i32 amountX, i32 amountY)
reci ReciScale(reci rectangle, i32 scalar)
reci ReciScale2(reci rectangle, i32 scaleX, i32 scaleY)
bool ReciEqual(reci left, reci right)
box BoxShift(box boundingBox, r32 amountX, r32 amountY, r32 amountZ)
box BoxScale(box boundingBox, r32 scalar)
box BoxScale3(box boundingBox, r32 scaleX, r32 scaleY, r32 scaleZ)
bool BoxBasicallyEqual(box left, box right, r32 tolerance = 0.001f)
boxi BoxiShift(boxi boundingBox, i32 amountX, i32 amountY, i32 amountZ)
boxi BoxiScale(boxi boundingBox, i32 scalar)
boxi BoxiScale3(boxi boundingBox, i32 scaleX, i32 scaleY, i32 scaleZ)
bool BoxiEqual(boxi left, boxi right)
obb2 Obb2DShift(obb2 boundingBox, r32 amountX, r32 amountY)
obb2 Obb2DScale(obb2 boundingBox, r32 scalar)
bool Obb2DBasicallyEqual(obb2 left, obb2 right, r32 tolerance = 0.001f)
rec RecExpand(rec rectangle, r32 extraWidth, r32 extraHeight)
rec RecExpandX(rec rectangle, r32 extraWidth)
rec RecExpandY(rec rectangle, r32 extraHeight)
rec RecExpandLeft(rec rectangle, r32 extraLeft)
rec RecExpandRight(rec rectangle, r32 extraRight)
rec RecExpandUp(rec rectangle, r32 extraUp)
rec RecExpandDown(rec rectangle, r32 extraDown)
rec RecRetract(rec rectangle, r32 subWidth, r32 subHeight)
rec RecRetractX(rec rectangle, r32 subWidth)
rec RecRetractY(rec rectangle, r32 subHeight)
rec RecInflate(rec rectangle, r32 extraWidth, r32 extraHeight)
rec RecInflateX(rec rectangle, r32 extraWidth)
rec RecInflateY(rec rectangle, r32 extraHeight)
rec RecDeflate(rec rectangle, r32 subWidth, r32 subHeight)
rec RecDeflateX(rec rectangle, r32 subWidth)
rec RecDeflateY(rec rectangle, r32 subHeight)
reci ReciExpand(reci rectangle, i32 extraWidth, i32 extraHeight)
reci ReciExpandX(reci rectangle, i32 extraWidth)
reci ReciExpandY(reci rectangle, i32 extraHeight)
reci ReciExpandLeft(reci rectangle, i32 extraLeft)
reci ReciExpandRight(reci rectangle, i32 extraRight)
reci ReciExpandUp(reci rectangle, i32 extraUp)
reci ReciExpandDown(reci rectangle, i32 extraDown)
reci ReciRetract(reci rectangle, i32 subWidth, i32 subHeight)
reci ReciRetractX(reci rectangle, i32 subWidth)
reci ReciRetractY(reci rectangle, i32 subHeight)
reci ReciInflate(reci rectangle, i32 extraWidth, i32 extraHeight)
reci ReciInflateX(reci rectangle, i32 extraWidth)
reci ReciInflateY(reci rectangle, i32 extraHeight)
reci ReciDeflate(reci rectangle, i32 subWidth, i32 subHeight)
reci ReciDeflateX(reci rectangle, i32 subWidth)
reci ReciDeflateY(reci rectangle, i32 subHeight)
box BoxExpand(box boundingBox, r32 extraWidth, r32 extraHeight, r32 extraDepth)
box BoxExpandX(box boundingBox, r32 extraWidth)
box BoxExpandY(box boundingBox, r32 extraHeight)
box BoxExpandZ(box boundingBox, r32 extraDepth)
box BoxRetract(box boundingBox, r32 subWidth, r32 subHeight, r32 subDepth)
box BoxRetractX(box boundingBox, r32 subWidth)
box BoxRetractY(box boundingBox, r32 subHeight)
box BoxRetractZ(box boundingBox, r32 subDepth)
box BoxInflate(box boundingBox, r32 extraWidth, r32 extraHeight, r32 extraDepth)
box BoxInflateX(box boundingBox, r32 extraWidth)
box BoxInflateY(box boundingBox, r32 extraHeight)
box BoxInflateZ(box boundingBox, r32 extraDepth)
box BoxDeflate(box boundingBox, r32 subWidth, r32 subHeight, r32 subDepth)
box BoxDeflateX(box boundingBox, r32 subWidth)
box BoxDeflateY(box boundingBox, r32 subHeight)
box BoxDeflateZ(box boundingBox, r32 subDepth)
boxi BoxiExpand(boxi boundingBox, i32 extraWidth, i32 extraHeight, i32 extraDepth)
boxi BoxiExpandX(boxi boundingBox, i32 extraWidth)
boxi BoxiExpandY(boxi boundingBox, i32 extraHeight)
boxi BoxiExpandZ(boxi boundingBox, i32 extraDepth)
boxi BoxiRetract(boxi boundingBox, i32 subWidth, i32 subHeight, i32 subDepth)
boxi BoxiRetractX(boxi boundingBox, i32 subWidth)
boxi BoxiRetractY(boxi boundingBox, i32 subHeight)
boxi BoxiRetractZ(boxi boundingBox, i32 subDepth)
boxi BoxiInflate(boxi boundingBox, i32 extraWidth, i32 extraHeight, i32 extraDepth)
boxi BoxiInflateX(boxi boundingBox, i32 extraWidth)
boxi BoxiInflateY(boxi boundingBox, i32 extraHeight)
boxi BoxiInflateZ(boxi boundingBox, i32 extraDepth)
boxi BoxiDeflate(boxi boundingBox, i32 subWidth, i32 subHeight, i32 subDepth)
boxi BoxiDeflateX(boxi boundingBox, i32 subWidth)
boxi BoxiDeflateY(boxi boundingBox, i32 subHeight)
boxi BoxiDeflateZ(boxi boundingBox, i32 subDepth)
obb2 Obb2DInflate(obb2 rectangle, r32 extraWidth, r32 extraHeight)
obb2 Obb2DInflateX(obb2 rectangle, r32 extraWidth)
obb2 Obb2DInflateY(obb2 rectangle, r32 extraHeight)
obb2 Obb2DDeflate(obb2 rectangle, r32 subWidth, r32 subHeight)
obb2 Obb2DDeflateX(obb2 rectangle, r32 subWidth)
obb2 Obb2DDeflateY(obb2 rectangle, r32 subHeight)
rec RecSquarify(rec rectangle, bool makeLarger = true, bool center = true)
rec RecInvert(rec rectangle)
rec RecInvertX(rec rectangle)
rec RecInvertY(rec rectangle)
rec RecUninvert(rec rectangle)
rec RecBoth(rec rectangle1, rec rectangle2)
rec RecExpandToVec2(rec rectangle, v2 newPoint)
rec RecOverlap(rec rectangle1, rec rectangle2)
reci ReciSquarify(reci rectangle, bool makeLarger = true)
reci ReciInvert(reci rectangle)
reci ReciInvertX(reci rectangle)
reci ReciInvertY(reci rectangle)
reci ReciUninvert(reci rectangle)
reci ReciBoth(reci rectangle1, reci rectangle2)
reci ReciExpandToVec2i(reci rectangle, v2i newPoint)
reci ReciOverlap(reci rectangle1, reci rectangle2)
box BoxCubify(box boundingBox, bool makeLarger = true, bool center = true)
box BoxInvert(box boundingBox)
box BoxInvertX(box boundingBox)
box BoxInvertY(box boundingBox)
box BoxInvertZ(box boundingBox)
box BoxUninvert(box boundingBox)
boxi BoxiCubify(boxi boundingBox, bool makeLarger = true)
boxi BoxiInvert(boxi boundingBox)
boxi BoxiInvertX(boxi boundingBox)
boxi BoxiInvertY(boxi boundingBox)
boxi BoxiInvertZ(boxi boundingBox)
boxi BoxiUninvert(boxi boundingBox)
boxi BoxiExpandToVec3i(boxi boundingBox, v3i newPoint)
boxi BoxiBoth(boxi boundingBox1, boxi boundingBox2)
obb2 Obb2DSquarify(obb2 rectangle, bool makeLarger = true)
obb2 Obb2DInvert(obb2 rectangle)
obb2 Obb2DInvertX(obb2 rectangle)
obb2 Obb2DInvertY(obb2 rectangle)
obb2 Obb2DUninvert(obb2 rectangle)
rec GetObb2DAlignedBounds(obb2 boundingBox)
bool IsInsideRec(rec rectangle, v2 point)
bool RecsIntersect(rec rectangle1, rec rectangle2, bool inclusive = true)
bool IsInsideReci(reci rectangle, v2i point, bool includePositiveEdges = false)
bool RecisIntersect(reci rectangle1, reci rectangle2, bool inclusive = true)
bool IsInsideBox(box boundingBox, v3 point)
bool BoxesIntersect(box boundingBox1, box boundingBox2, bool inclusive = true)
bool IsInsideBoxi(boxi boundingBox, v3i point, bool includePositiveEdges = false)
bool BoxisIntersect(boxi boundingBox1, boxi boundingBox2, bool inclusive = true)
bool IsInsideObb2D(obb2 rectangle, v2 point)
void RecAlign(rec* rectangleOut, r32 alignmentScale = 1.0f)
void RecAlignTopLeft(rec* rectangleOut, r32 alignmentScale = 1.0f)
void RecAlignSize(rec* rectangleOut, r32 alignmentScale = 1.0f)
void RecLayoutLeftOf(rec* rectangleOut, r32 posOfThingToRight, r32 rightPadding = 0.0f)
void RecLayoutTopOf(rec* rectangleOut, r32 posOfThingDownwards, r32 bottomPadding = 0.0f)
void RecLayoutRightOf(rec* rectangleOut, r32 posOfThingToLeft, r32 leftPadding = 0.0f)
void RecLayoutBottomOf(rec* rectangleOut, r32 posOfThingUpwards, r32 topPadding = 0.0f)
void RecLayoutBetweenX(rec* rectangleOut, r32 leftSide, r32 rightSide, r32 leftMargin = 0, r32 rightMargin = 0, r32 minWidth = 0)
void RecLayoutBetweenY(rec* rectangleOut, r32 topSide, r32 bottomSide, r32 topMargin = 0, r32 bottomMargin = 0, r32 minHeight = 0)
void RecLayoutLeftPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionWidth, bool shrinkOtherRec = false, r32 offsetAmount = 0)
void RecLayoutRightPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionWidth, bool shrinkOtherRec = false, r32 offsetAmount = 0)
void RecLayoutTopPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionHeight, bool shrinkOtherRec = false, r32 offsetAmount = 0)
void RecLayoutBottomPortionOf(rec* rectangleOut, rec* otherRectangle, r32 portionHeight, bool shrinkOtherRec = false, r32 offsetAmount = 0)
void RecLayoutVerticalCenter(rec* rectangleOut, rec otherRectangle, r32 percentage = 0.5f)
void RecLayoutHorizontalCenter(rec* rectangleOut, rec otherRectangle, r32 percentage = 0.5f)
obb2 Obb2Line(v2 start, v2 end, r32 thickness)
*/
