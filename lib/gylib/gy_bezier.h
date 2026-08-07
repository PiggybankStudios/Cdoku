/*
File:   gy_bezier.h
Author: Taylor Robbins
Date:   01\24\2022
Description:
	** Holds functions that help us calculate positions on Bezier curves of 3rd and 4th order
	** Also contains structures and functions that help us manage a list of curves and lines
	** that make up a path or enclosed shape (like from vector art files)
*/

//TODO: Add functions that will help us detect intersections with bezier curves (or closest point on the curve)
//TODO: Add functions that allow us to ask if a point is "inside" a bezier shape
//TODO: Can we calculate the length of a bezier curve? Or maybe the maximum turn radius of the curve so we can use that to determine how many vertices we should use to draw the curve?
//TODO: Can we make some sort of curve fitting functions? Like given some vertices, find a curve that fits them as close as possible
//TODO: Add functions to quickly triangulate a bezier shape (or quickly turn a path into a list of vertices)

#ifndef _GY_BEZIER_H
#define _GY_BEZIER_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"
#include "gy_vectors.h"
#include "gy_directions.h"
#include "gy_variable_array.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
union Bezier3_t
{
	v2 points[3];
	struct
	{
		v2 start;
		v2 control;
		v2 end;
	};
};
union Bezier4_t
{
	v2 points[4];
	struct
	{
		v2 start;
		v2 control1;
		v2 control2;
		v2 end;
	};
};

enum BezierPathPartType_t
{
	BezierPathPartType_None = 0,
	BezierPathPartType_Line,
	BezierPathPartType_Curve3,
	BezierPathPartType_Curve4,
	BezierPathPartType_EllipseArc,
	BezierPathPartType_NumTypes,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetBezierPathPartTypeStr(BezierPathPartType_t enumValue);
#else
const char* GetBezierPathPartTypeStr(BezierPathPartType_t enumValue)
{
	switch (enumValue)
	{
		case BezierPathPartType_None:       return "None";
		case BezierPathPartType_Line:       return "Line";
		case BezierPathPartType_Curve3:     return "Curve3";
		case BezierPathPartType_Curve4:     return "Curve4";
		case BezierPathPartType_EllipseArc: return "EllipseArc";
		default: return "Unknown";
	}
}
#endif
enum BezierPathPartArcFlag_t
{
	BezierPathPartArcFlag_None  = 0x00,
	BezierPathPartArcFlag_Large = 0x01,
	BezierPathPartArcFlag_Sweep = 0x02,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetBezierPathPartArcFlagStr(BezierPathPartArcFlag_t enumValue);
#else
const char* GetBezierPathPartArcFlagStr(BezierPathPartArcFlag_t enumValue)
{
	switch (enumValue)
	{
		case BezierPathPartArcFlag_None:  return "None";
		case BezierPathPartArcFlag_Large: return "Large";
		case BezierPathPartArcFlag_Sweep: return "Sweep";
		default: return "Unknown";
	}
}
#endif
struct BezierPathPart_t
{
	BezierPathPartType_t type;
	v2 endPos;
	v2 control1; //only if BezierPathPartType_Curve3 or BezierPathPartType_Curve4
	v2 control2; //only if BezierPathPartType_Curve4
	v2 radius; //only if BezierPathPartType_EllipseArc
	r32 axisAngle; //only if BezierPathPartType_EllipseArc
	u8 arcFlags; //only if BezierPathPartType_EllipseArc
	bool detached;
	v2 startPos; //only if detached
};
struct BezierPath_t
{
	bool isClosedLoop;
	VarArray_t parts; //BezierPathPart_t
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	v2 ParametricLine(v2 start, v2 end, r32 time);
	v2 BezierCurve3(v2 start, v2 control, v2 end, r32 time);
	v2 BezierCurve4(v2 start, v2 control1, v2 control2, v2 end, r32 time);
	v2 EllipseArcCurve(v2 center, v2 radius, r32 axisAngle, r32 startAngle, r32 angleDelta, r32 time);
	bool GetEllipseArcCurveCenterAndAngles(v2 start, v2 radius, r32 axisAngle, u8 arcFlags, v2 end, v2* centerOut = nullptr, r32* startAngleOut = nullptr, r32* angleDeltaOut = nullptr);
	v2 EllipseArcCurveStartEnd(v2 start, v2 radius, r32 axisAngle, u8 arcFlags, v2 end, r32 time);
	Bezier3_t NewBezier3(v2 start, v2 control, v2 end);
	Bezier4_t NewBezier4(v2 start, v2 control1, v2 control2, v2 end);
	BezierPathPart_t NewBezierPathPartLine(v2 endPos);
	BezierPathPart_t NewBezierPathPartCurve3(v2 control, v2 endPos);
	BezierPathPart_t NewBezierPathPartCurve4(v2 control1, v2 control2, v2 endPos);
	Bezier3_t ToBezier3(v2 startPos, BezierPathPart_t pathPart);
	Bezier4_t ToBezier4(v2 startPos, BezierPathPart_t pathPart);
	Bezier3_t Bezier3RotateAround(Bezier3_t curve, r32 rotation, v2 rotationOrigin);
	Bezier3_t Bezier3Rotate(Bezier3_t curve, r32 rotation);
	Bezier4_t Bezier4RotateAround(Bezier4_t curve, r32 rotation, v2 rotationOrigin);
	Bezier4_t Bezier4Rotate(Bezier4_t curve, r32 rotation);
	void FreeBezierPath(BezierPath_t* path);
	void CreateBezierPath(BezierPath_t* pathOut, MemArena_t* memArena, u64 numPartsExpected = 0);
	u64 GetNumSubPathsInBezierPath(const BezierPath_t* path);
	u64 GetNumVerticesInBezierPath(const BezierPath_t* path);
	v2 GetPointOnBezierPathPart(v2 currentPos, const BezierPathPart_t* part, r32 time);
	rec GetBoundsForBezierPathPart(v2 currentPos, const BezierPathPart_t* part);
	rec GetBoundsForBezierPath(const BezierPath_t* path);
	BezierPathPart_t* AddBezierPathPartDetachedLine(BezierPath_t* path, v2 startPos, v2 endPos);
	BezierPathPart_t* AddBezierPathPartDetachedCurve3(BezierPath_t* path, v2 startPos, v2 control, v2 endPos);
	BezierPathPart_t* AddBezierPathPartDetachedCurve4(BezierPath_t* path, v2 startPos, v2 control1, v2 control2, v2 endPos);
	BezierPathPart_t* AddBezierPathPartLine(BezierPath_t* path, v2 endPos);
	BezierPathPart_t* AddBezierPathPartCurve3(BezierPath_t* path, v2 control, v2 endPos);
	BezierPathPart_t* AddBezierPathPartCurve4(BezierPath_t* path, v2 control1, v2 control2, v2 endPos);
	Bezier4_t GetBezierCurveForQuarterCircle(v2 center, r32 radius, u8 dir = Dir2_Right|Dir2_Down);
	void GenerateBezierPathForCircle(MemArena_t* arena, v2 center, r32 radius, BezierPath_t* pathOut);
	void GenerateBezierPathForRectangle(MemArena_t* arena, rec rectangle, BezierPath_t* pathOut);
	void GenerateBezierPathForObb2(MemArena_t* arena, obb2 boundingBox, BezierPath_t* pathOut);
	void GenerateBezierPathForRoundedRectangle(MemArena_t* arena, rec rectangle, r32 cornerRadius, BezierPath_t* pathOut);
	void GenerateBezierPathForRoundedObb2(MemArena_t* arena, obb2 boundingBox, r32 cornerRadius, BezierPath_t* pathOut);
#else

// +--------------------------------------------------------------+
// |                     Curve Math Functions                     |
// +--------------------------------------------------------------+
v2 ParametricLine(v2 start, v2 end, r32 time)
{
	return start + ((end - start) * time);
}
v2 BezierCurve3(v2 start, v2 control, v2 end, r32 time)
{
	r32 invTime = (1 - time);
	return NewVec2(
		(Square(invTime)*start.x) + (2*time*(invTime)*control.x) + Square(time)*end.x,
		(Square(invTime)*start.y) + (2*time*(invTime)*control.y) + Square(time)*end.y
	);
}
v2 BezierCurve4(v2 start, v2 control1, v2 control2, v2 end, r32 time)
{
	r32 invTime = (1 - time);
	return NewVec2(
		(Cube(invTime)*start.x) + 3*Square(invTime)*time*control1.x + (3*(invTime)*Square(time)*control2.x) + Cube(time)*end.x,
		(Cube(invTime)*start.y) + 3*Square(invTime)*time*control1.y + (3*(invTime)*Square(time)*control2.y) + Cube(time)*end.y
	);
}
v2 EllipseArcCurve(v2 center, v2 radius, r32 axisAngle, r32 startAngle, r32 angleDelta, r32 time)
{
	r32 angle = startAngle + angleDelta*time;
	v2 result = center;
	result += radius.x * CosR32(angle) * Vec2FromAngle(axisAngle);
	result += radius.y * SinR32(angle) * Vec2PerpRight(Vec2FromAngle(axisAngle));
	return result;
}
bool GetEllipseArcCurveCenterAndAngles(v2 start, v2 radius, r32 axisAngle, u8 arcFlags, v2 end, v2* centerOut = nullptr, r32* startAngleOut = nullptr, r32* angleDeltaOut = nullptr)
{
	//https://www.w3.org/TR/SVG2/implnote.html#ArcConversionEndpointToCenter
	//https://observablehq.com/@toja/ellipse-and-elliptical-arc-conversion
	v2 midpoint = (start + end) / 2;
	v2 posPrime = Vec2_Zero;
	posPrime.x =  CosR32(axisAngle) * ((start.x - end.x) / 2) + SinR32(axisAngle) * ((start.y - end.y) / 2);
	posPrime.y = -SinR32(axisAngle) * ((start.x - end.x) / 2) + CosR32(axisAngle) * ((start.y - end.y) / 2);
	r32 solutionCeof = (
		(Square(radius.x) * Square(radius.y) - Square(radius.x) * Square(posPrime.y) - Square(radius.y) * Square(posPrime.x))
		/
		(Square(radius.x) * Square(posPrime.y) + Square(radius.y) * Square(posPrime.x))
	);
	if (solutionCeof < 0) { return false; }
	solutionCeof = SqrtR32(solutionCeof);
	v2 centerPrime = Vec2_Zero;
	centerPrime.x = solutionCeof * (( radius.x * posPrime.y) / radius.y);
	centerPrime.y = solutionCeof * ((-radius.y * posPrime.x) / radius.x);
	if (IsFlagSet(arcFlags, BezierPathPartArcFlag_Large) == IsFlagSet(arcFlags, BezierPathPartArcFlag_Sweep))
	{
		centerPrime = -centerPrime;
	}
	v2 center = Vec2_Zero;
	center.x = CosR32(axisAngle)  * centerPrime.x - SinR32(axisAngle) * centerPrime.y + midpoint.x;
	center.y = SinR32(axisAngle)  * centerPrime.x + CosR32(axisAngle) * centerPrime.y + midpoint.y;
	
	v2 someVector = NewVec2((posPrime.x - centerPrime.x) / radius.x, (posPrime.y - centerPrime.y) / radius.y);
	v2 someVector2 = NewVec2((-posPrime.x - centerPrime.x) / radius.x, (-posPrime.y - centerPrime.y) / radius.y);
	r32 startAngle = Vec2AngleBetween(Vec2_Right, someVector);
	r32 angleDelta = Vec2AngleBetween(someVector, someVector2);
	angleDelta = ModR32(angleDelta, TwoPi32);
	if (!IsFlagSet(arcFlags, BezierPathPartArcFlag_Sweep) && angleDelta > 0) { angleDelta -= TwoPi32; }
	if (IsFlagSet(arcFlags, BezierPathPartArcFlag_Sweep) && angleDelta < 0) { angleDelta += TwoPi32; }
	
	if (centerOut != nullptr) { *centerOut = center; }
	if (startAngleOut != nullptr) { *startAngleOut = startAngle; }
	if (angleDeltaOut != nullptr) { *angleDeltaOut = angleDelta; }
	return true;
}
v2 EllipseArcCurveStartEnd(v2 start, v2 radius, r32 axisAngle, u8 arcFlags, v2 end, r32 time)
{
	v2 center = Vec2_Zero;
	r32 startAngle = 0;
	r32 angleDelta = 0;
	if (GetEllipseArcCurveCenterAndAngles(start, radius, axisAngle, arcFlags, end, &center, &startAngle, &angleDelta))
	{
		return EllipseArcCurve(center, radius, axisAngle, startAngle, angleDelta, time);
	}
	else
	{
		return ParametricLine(start, end, time);
	}
}

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Bezier3_t NewBezier3(v2 start, v2 control, v2 end)
{
	Bezier3_t result;
	result.start = start;
	result.control = control;
	result.end = end;
	return result;
}
Bezier4_t NewBezier4(v2 start, v2 control1, v2 control2, v2 end)
{
	Bezier4_t result;
	result.start = start;
	result.control1 = control1;
	result.control2 = control2;
	result.end = end;
	return result;
}

BezierPathPart_t NewBezierPathPartLine(v2 endPos)
{
	BezierPathPart_t result = {};
	result.type = BezierPathPartType_Line;
	result.endPos = endPos;
	return result;
}
BezierPathPart_t NewBezierPathPartCurve3(v2 control, v2 endPos)
{
	BezierPathPart_t result = {};
	result.type = BezierPathPartType_Curve3;
	result.control1 = control;
	result.endPos = endPos;
	return result;
}
BezierPathPart_t NewBezierPathPartCurve4(v2 control1, v2 control2, v2 endPos)
{
	BezierPathPart_t result = {};
	result.type = BezierPathPartType_Curve4;
	result.control1 = control1;
	result.control2 = control2;
	result.endPos = endPos;
	return result;
}

// +--------------------------------------------------------------+
// |                     Conversion Functions                     |
// +--------------------------------------------------------------+
Bezier3_t ToBezier3(v2 startPos, BezierPathPart_t pathPart)
{
	Assert(pathPart.type == BezierPathPartType_Curve3);
	if (pathPart.detached)
	{
		return NewBezier3(pathPart.startPos, pathPart.control1, pathPart.endPos);
	}
	else
	{
		return NewBezier3(startPos, pathPart.control1, pathPart.endPos);
	}
}
Bezier4_t ToBezier4(v2 startPos, BezierPathPart_t pathPart)
{
	Assert(pathPart.type == BezierPathPartType_Curve4);
	if (pathPart.detached)
	{
		return NewBezier4(pathPart.startPos, pathPart.control1, pathPart.control2, pathPart.endPos);
	}
	else
	{
		return NewBezier4(startPos, pathPart.control1, pathPart.control2, pathPart.endPos);
	}
}

// +--------------------------------------------------------------+
// |                    Manipulation Functions                    |
// +--------------------------------------------------------------+
Bezier3_t Bezier3RotateAround(Bezier3_t curve, r32 rotation, v2 rotationOrigin)
{
	Bezier3_t result = {};
	result.start   = rotationOrigin + Vec2Rotate(curve.start   - rotationOrigin, rotation);
	result.control = rotationOrigin + Vec2Rotate(curve.control - rotationOrigin, rotation);
	result.end     = rotationOrigin + Vec2Rotate(curve.end     - rotationOrigin, rotation);
	return result;
}
Bezier3_t Bezier3Rotate(Bezier3_t curve, r32 rotation)
{
	Bezier3_t result = {};
	result.start   = Vec2Rotate(curve.start,   rotation);
	result.control = Vec2Rotate(curve.control, rotation);
	result.end     = Vec2Rotate(curve.end,     rotation);
	return result;
}

Bezier4_t Bezier4RotateAround(Bezier4_t curve, r32 rotation, v2 rotationOrigin)
{
	Bezier4_t result = {};
	result.start    = rotationOrigin + Vec2Rotate(curve.start    - rotationOrigin, rotation);
	result.control1 = rotationOrigin + Vec2Rotate(curve.control1 - rotationOrigin, rotation);
	result.control2 = rotationOrigin + Vec2Rotate(curve.control2 - rotationOrigin, rotation);
	result.end      = rotationOrigin + Vec2Rotate(curve.end      - rotationOrigin, rotation);
	return result;
}
Bezier4_t Bezier4Rotate(Bezier4_t curve, r32 rotation)
{
	Bezier4_t result = {};
	result.start    = Vec2Rotate(curve.start,    rotation);
	result.control1 = Vec2Rotate(curve.control1, rotation);
	result.control2 = Vec2Rotate(curve.control2, rotation);
	result.end      = Vec2Rotate(curve.end,      rotation);
	return result;
}

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeBezierPath(BezierPath_t* path)
{
	NotNull(path);
	FreeVarArray(&path->parts);
	ClearPointer(path);
}

void CreateBezierPath(BezierPath_t* pathOut, MemArena_t* memArena, u64 numPartsExpected = 0)
{
	NotNull(pathOut);
	ClearPointer(pathOut);
	pathOut->isClosedLoop = false;
	CreateVarArray(&pathOut->parts, memArena, sizeof(BezierPathPart_t), numPartsExpected);
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
u64 GetNumSubPathsInBezierPath(const BezierPath_t* path)
{
	NotNull(path);
	u64 result = 0;
	VarArrayLoop(&path->parts, pIndex)
	{
		VarArrayLoopGet(BezierPathPart_t, part, &path->parts, pIndex);
		AssertIf(pIndex == 0, part->detached);
		if (part->detached) { result++; }
	}
	return result;
}
u64 GetNumVerticesInBezierPath(const BezierPath_t* path)
{
	NotNull(path);
	u64 result = 0;
	VarArrayLoop(&path->parts, pIndex)
	{
		VarArrayLoopGet(BezierPathPart_t, part, &path->parts, pIndex);
		AssertIf(pIndex == 0, part->detached);
		if (pIndex == path->parts.length-1 && path->isClosedLoop)
		{
			result += (part->detached ? 1 : 0);
		}
		else
		{
			result += (part->detached ? 2 : 1);
		}
	}
	return result;
}
v2 GetPointOnBezierPathPart(v2 currentPos, const BezierPathPart_t* part, r32 time)
{
	if (part->detached) { currentPos = part->startPos; }
	switch (part->type)
	{
		case BezierPathPartType_Line: return currentPos + (part->endPos - currentPos) * time;
		case BezierPathPartType_Curve3: return BezierCurve3(currentPos, part->control1, part->endPos, time);
		case BezierPathPartType_Curve4: return BezierCurve4(currentPos, part->control1, part->control2, part->endPos, time);
		case BezierPathPartType_EllipseArc: return EllipseArcCurveStartEnd(currentPos, part->radius, part->axisAngle, part->arcFlags, part->endPos, time);
		default: DebugAssert(false); return currentPos;
	}
}

rec GetBoundsForBezierPathPart(v2 currentPos, const BezierPathPart_t* part)
{
	NotNull(part);
	rec result = NewRecBetween(currentPos, part->endPos);
	if (part->type != BezierPathPartType_Line)
	{
		const u64 subdivideCount = 100;
		for (u64 tIndex = 0; tIndex < subdivideCount; tIndex++)
		{
			v2 pointOnCurve = GetPointOnBezierPathPart(currentPos, part, (r32)tIndex / (r32)subdivideCount);
			result = RecExpandToVec2(result, pointOnCurve);
		}
	}
	return result;
}
rec GetBoundsForBezierPath(const BezierPath_t* path)
{
	NotNull(path);
	rec result = Rec_Zero;
	v2 currentPos = Vec2_Zero;
	VarArrayLoop(&path->parts, pIndex)
	{
		VarArrayLoopGet(BezierPathPart_t, pathPart, &path->parts, pIndex);
		if (pathPart->detached) { currentPos = pathPart->startPos; }
		if (pIndex == 0) { result = GetBoundsForBezierPathPart(currentPos, pathPart); }
		else { result = RecBoth(result, GetBoundsForBezierPathPart(currentPos, pathPart)); }
		currentPos = pathPart->endPos;
	}
	return result;
}

// +--------------------------------------------------------------+
// |                      Path Manipulation                       |
// +--------------------------------------------------------------+
BezierPathPart_t* AddBezierPathPartDetachedLine(BezierPath_t* path, v2 startPos, v2 endPos)
{
	NotNull(path);
	BezierPathPart_t* result = VarArrayAdd(&path->parts, BezierPathPart_t);
	if (result == nullptr) { return nullptr; }
	*result = NewBezierPathPartLine(endPos);
	result->detached = true;
	result->startPos = startPos;
	return result;
}
BezierPathPart_t* AddBezierPathPartDetachedCurve3(BezierPath_t* path, v2 startPos, v2 control, v2 endPos)
{
	NotNull(path);
	BezierPathPart_t* result = VarArrayAdd(&path->parts, BezierPathPart_t);
	if (result == nullptr) { return nullptr; }
	*result = NewBezierPathPartCurve3(control, endPos);
	result->detached = true;
	result->startPos = startPos;
	return result;
}
BezierPathPart_t* AddBezierPathPartDetachedCurve4(BezierPath_t* path, v2 startPos, v2 control1, v2 control2, v2 endPos)
{
	NotNull(path);
	BezierPathPart_t* result = VarArrayAdd(&path->parts, BezierPathPart_t);
	if (result == nullptr) { return nullptr; }
	*result = NewBezierPathPartCurve4(control1, control2, endPos);
	result->detached = true;
	result->startPos = startPos;
	return result;
}

BezierPathPart_t* AddBezierPathPartLine(BezierPath_t* path, v2 endPos)
{
	NotNull(path);
	DebugAssert(path->parts.length > 0); //first part should be detached
	BezierPathPart_t* result = VarArrayAdd(&path->parts, BezierPathPart_t);
	if (result == nullptr) { return nullptr; }
	*result = NewBezierPathPartLine(endPos);
	return result;
}
BezierPathPart_t* AddBezierPathPartCurve3(BezierPath_t* path, v2 control, v2 endPos)
{
	NotNull(path);
	DebugAssert(path->parts.length > 0); //first part should be detached
	BezierPathPart_t* result = VarArrayAdd(&path->parts, BezierPathPart_t);
	if (result == nullptr) { return nullptr; }
	*result = NewBezierPathPartCurve3(control, endPos);
	return result;
}
BezierPathPart_t* AddBezierPathPartCurve4(BezierPath_t* path, v2 control1, v2 control2, v2 endPos)
{
	NotNull(path);
	DebugAssert(path->parts.length > 0); //first part should be detached
	BezierPathPart_t* result = VarArrayAdd(&path->parts, BezierPathPart_t);
	if (result == nullptr) { return nullptr; }
	*result = NewBezierPathPartCurve4(control1, control2, endPos);
	return result;
}

// +--------------------------------------------------------------+
// |                     Generating Functions                     |
// +--------------------------------------------------------------+
Bezier4_t GetBezierCurveForQuarterCircle(v2 center, r32 radius, u8 dir = Dir2_Right|Dir2_Down)
{
	//https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%C3%A9zier-curves
	const r32 quarterCircleControlPointDist = (4.0f/3.0f) * TanR32(Pi32 / 8.0f);
	
	Bezier4_t result;
	result.start = NewVec2(radius, 0);
	result.end   = NewVec2(0, radius);
	result.control1 = result.start + NewVec2(0, quarterCircleControlPointDist * radius);
	result.control2 = result.end + NewVec2(quarterCircleControlPointDist * radius, 0);
	
	switch (dir)
	{
		case (Dir2_Right|Dir2_Down): /*Nothing to do*/ break;
		case (Dir2_Right|Dir2_Up):   result.start = Vec2PerpLeft(result.start); result.control1 = Vec2PerpLeft(result.control1); result.control2 = Vec2PerpLeft(result.control2); result.end = Vec2PerpLeft(result.end); break;
		case (Dir2_Left |Dir2_Down): result.start = Vec2PerpRight(result.start); result.control1 = Vec2PerpRight(result.control1); result.control2 = Vec2PerpRight(result.control2); result.end = Vec2PerpRight(result.end); break;
		case (Dir2_Left |Dir2_Up):   result.start = -result.start; result.control1 = -result.control1; result.control2 = -result.control2; result.end = -result.end; break;
		default: DebugAssert(false); break;
	}
	result.start    += center;
	result.control1 += center;
	result.control2 += center;
	result.end      += center;
	
	return result;
}
void GenerateBezierPathForCircle(MemArena_t* arena, v2 center, r32 radius, BezierPath_t* pathOut)
{
	NotNull(arena);
	NotNull(pathOut);
	CreateBezierPath(pathOut, arena, 4);
	Bezier4_t curve1 = GetBezierCurveForQuarterCircle(center, radius, (Dir2_Right|Dir2_Down));
	Bezier4_t curve2 = GetBezierCurveForQuarterCircle(center, radius, (Dir2_Left |Dir2_Down));
	Bezier4_t curve3 = GetBezierCurveForQuarterCircle(center, radius, (Dir2_Left |Dir2_Up));
	Bezier4_t curve4 = GetBezierCurveForQuarterCircle(center, radius, (Dir2_Right|Dir2_Up));
	AddBezierPathPartDetachedCurve4(pathOut, curve1.start, curve1.control1, curve1.control2, curve1.end);
	AddBezierPathPartCurve4(pathOut, curve2.control1, curve2.control2, curve2.end);
	AddBezierPathPartCurve4(pathOut, curve3.control1, curve3.control2, curve3.end);
	AddBezierPathPartCurve4(pathOut, curve4.control1, curve4.control2, curve4.end);
}

void GenerateBezierPathForRectangle(MemArena_t* arena, rec rectangle, BezierPath_t* pathOut)
{
	NotNull(arena);
	NotNull(pathOut);
	CreateBezierPath(pathOut, arena, 4);
	AddBezierPathPartDetachedLine(pathOut, NewVec2(rectangle.x, rectangle.y), NewVec2(rectangle.x + rectangle.width, rectangle.y));
	AddBezierPathPartLine(pathOut, NewVec2(rectangle.x + rectangle.width, rectangle.y + rectangle.height));
	AddBezierPathPartLine(pathOut, NewVec2(rectangle.x, rectangle.y + rectangle.height));
	AddBezierPathPartLine(pathOut, NewVec2(rectangle.x, rectangle.y));
	pathOut->isClosedLoop = true;
}
void GenerateBezierPathForObb2(MemArena_t* arena, obb2 boundingBox, BezierPath_t* pathOut)
{
	NotNull(arena);
	NotNull(pathOut);
	CreateBezierPath(pathOut, arena, 4);
	AddBezierPathPartDetachedLine(pathOut, GetObb2DTopRight(boundingBox), GetObb2DBottomRight(boundingBox));
	AddBezierPathPartLine(pathOut, GetObb2DBottomLeft(boundingBox));
	AddBezierPathPartLine(pathOut, GetObb2DTopLeft(boundingBox));
	AddBezierPathPartLine(pathOut, GetObb2DTopRight(boundingBox));
	pathOut->isClosedLoop = true;
}

void GenerateBezierPathForRoundedRectangle(MemArena_t* arena, rec rectangle, r32 cornerRadius, BezierPath_t* pathOut)
{
	NotNull(arena);
	NotNull(pathOut);
	if (cornerRadius > 0)
	{
		CreateBezierPath(pathOut, arena, 8);
		Bezier4_t curve1 = GetBezierCurveForQuarterCircle(NewVec2(rectangle.x + rectangle.width - cornerRadius, rectangle.y + rectangle.height - cornerRadius), cornerRadius, (Dir2_Right|Dir2_Down));
		Bezier4_t curve2 = GetBezierCurveForQuarterCircle(NewVec2(rectangle.x                   + cornerRadius, rectangle.y + rectangle.height - cornerRadius), cornerRadius, (Dir2_Left |Dir2_Down));
		Bezier4_t curve3 = GetBezierCurveForQuarterCircle(NewVec2(rectangle.x                   + cornerRadius, rectangle.y                    + cornerRadius), cornerRadius, (Dir2_Left |Dir2_Up));
		Bezier4_t curve4 = GetBezierCurveForQuarterCircle(NewVec2(rectangle.x + rectangle.width - cornerRadius, rectangle.y                    + cornerRadius), cornerRadius, (Dir2_Right|Dir2_Up));
		AddBezierPathPartDetachedLine(pathOut, curve4.end, curve1.start);
		AddBezierPathPartCurve4(pathOut, curve1.control1, curve1.control2, curve1.end);
		AddBezierPathPartLine(pathOut, curve2.start);
		AddBezierPathPartCurve4(pathOut, curve2.control1, curve2.control2, curve2.end);
		AddBezierPathPartLine(pathOut, curve3.start);
		AddBezierPathPartCurve4(pathOut, curve3.control1, curve3.control2, curve3.end);
		AddBezierPathPartLine(pathOut, curve4.start);
		AddBezierPathPartCurve4(pathOut, curve4.control1, curve4.control2, curve4.end);
		pathOut->isClosedLoop = true;
	}
	else
	{
		GenerateBezierPathForRectangle(arena, rectangle, pathOut);
	}
}
void GenerateBezierPathForRoundedObb2(MemArena_t* arena, obb2 boundingBox, r32 cornerRadius, BezierPath_t* pathOut)
{
	NotNull(arena);
	NotNull(pathOut);
	if (cornerRadius > 0)
	{
		CreateBezierPath(pathOut, arena, 8);
		v2 circleCenter1 = GetObb2DWorldPoint(boundingBox, NewVec2(boundingBox.width - cornerRadius, boundingBox.height - cornerRadius));
		v2 circleCenter2 = GetObb2DWorldPoint(boundingBox, NewVec2(cornerRadius, boundingBox.height - cornerRadius));
		v2 circleCenter3 = GetObb2DWorldPoint(boundingBox, NewVec2(cornerRadius, cornerRadius));
		v2 circleCenter4 = GetObb2DWorldPoint(boundingBox, NewVec2(boundingBox.width - cornerRadius, cornerRadius));
		Bezier4_t curve1 = GetBezierCurveForQuarterCircle(circleCenter1, cornerRadius, (Dir2_Right|Dir2_Down));
		Bezier4_t curve2 = GetBezierCurveForQuarterCircle(circleCenter2, cornerRadius, (Dir2_Left |Dir2_Down));
		Bezier4_t curve3 = GetBezierCurveForQuarterCircle(circleCenter3, cornerRadius, (Dir2_Left |Dir2_Up));
		Bezier4_t curve4 = GetBezierCurveForQuarterCircle(circleCenter4, cornerRadius, (Dir2_Right|Dir2_Up));
		curve1 = Bezier4RotateAround(curve1, boundingBox.rotation, circleCenter1);
		curve2 = Bezier4RotateAround(curve2, boundingBox.rotation, circleCenter2);
		curve3 = Bezier4RotateAround(curve3, boundingBox.rotation, circleCenter3);
		curve4 = Bezier4RotateAround(curve4, boundingBox.rotation, circleCenter4);
		AddBezierPathPartDetachedLine(pathOut, curve4.end, curve1.start);
		AddBezierPathPartCurve4(pathOut, curve1.control1, curve1.control2, curve1.end);
		AddBezierPathPartLine(pathOut, curve2.start);
		AddBezierPathPartCurve4(pathOut, curve2.control1, curve2.control2, curve2.end);
		AddBezierPathPartLine(pathOut, curve3.start);
		AddBezierPathPartCurve4(pathOut, curve3.control1, curve3.control2, curve3.end);
		AddBezierPathPartLine(pathOut, curve4.start);
		AddBezierPathPartCurve4(pathOut, curve4.control1, curve4.control2, curve4.end);
		pathOut->isClosedLoop = true;
	}
	else
	{
		GenerateBezierPathForObb2(arena, boundingBox, pathOut);
	}
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_BEZIER_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
Bezier3_t
Bezier4_t
BezierPathPartType_t
BezierPathPartArcFlag_t
BezierPathPart_t
BezierPath_t
@Functions
v2 ParametricLine(v2 start, v2 end, r32 time)
v2 BezierCurve3(v2 start, v2 control, v2 end, r32 time)
v2 BezierCurve4(v2 start, v2 control1, v2 control2, v2 end, r32 time)
v2 EllipseArcCurve(v2 center, v2 radius, r32 axisAngle, r32 startAngle, r32 angleDelta, r32 time)
bool GetEllipseArcCurveCenterAndAngles(v2 start, v2 radius, r32 axisAngle, u8 arcFlags, v2 end, v2* centerOut = nullptr, r32* startAngleOut = nullptr, r32* angleDeltaOut = nullptr)
v2 EllipseArcCurveStartEnd(v2 start, v2 radius, r32 axisAngle, u8 arcFlags, v2 end, r32 time)
Bezier3_t NewBezier3(v2 start, v2 control, v2 end)
Bezier4_t NewBezier4(v2 start, v2 control1, v2 control2, v2 end)
BezierPathPart_t NewBezierPathPartLine(v2 endPos)
BezierPathPart_t NewBezierPathPartCurve3(v2 control, v2 endPos)
BezierPathPart_t NewBezierPathPartCurve4(v2 control1, v2 control2, v2 endPos)
Bezier3_t ToBezier3(v2 startPos, BezierPathPart_t pathPart)
Bezier4_t ToBezier4(v2 startPos, BezierPathPart_t pathPart)
Bezier3_t Bezier3RotateAround(Bezier3_t curve, r32 rotation, v2 rotationOrigin)
Bezier3_t Bezier3Rotate(Bezier3_t curve, r32 rotation)
Bezier4_t Bezier4RotateAround(Bezier4_t curve, r32 rotation, v2 rotationOrigin)
Bezier4_t Bezier4Rotate(Bezier4_t curve, r32 rotation)
void FreeBezierPath(BezierPath_t* path)
void CreateBezierPath(BezierPath_t* pathOut, MemArena_t* memArena, u64 numPartsExpected = 0)
u64 GetNumSubPathsInBezierPath(const BezierPath_t* path)
u64 GetNumVerticesInBezierPath(const BezierPath_t* path)
v2 GetPointOnBezierPathPart(v2 currentPos, const BezierPathPart_t* part, r32 time)
rec GetBoundsForBezierPathPart(v2 currentPos, const BezierPathPart_t* part)
rec GetBoundsForBezierPath(const BezierPath_t* path)
BezierPathPart_t* AddBezierPathPartDetachedLine(BezierPath_t* path, v2 startPos, v2 endPos)
BezierPathPart_t* AddBezierPathPartDetachedCurve3(BezierPath_t* path, v2 startPos, v2 control, v2 endPos)
BezierPathPart_t* AddBezierPathPartDetachedCurve4(BezierPath_t* path, v2 startPos, v2 control1, v2 control2, v2 endPos)
BezierPathPart_t* AddBezierPathPartLine(BezierPath_t* path, v2 endPos)
BezierPathPart_t* AddBezierPathPartCurve3(BezierPath_t* path, v2 control, v2 endPos)
BezierPathPart_t* AddBezierPathPartCurve4(BezierPath_t* path, v2 control1, v2 control2, v2 endPos)
Bezier4_t GetBezierCurveForQuarterCircle(v2 center, r32 radius, u8 dir = Dir2_Right|Dir2_Down)
void GenerateBezierPathForCircle(MemArena_t* arena, v2 center, r32 radius, BezierPath_t* pathOut)
void GenerateBezierPathForRectangle(MemArena_t* arena, rec rectangle, BezierPath_t* pathOut)
void GenerateBezierPathForObb2(MemArena_t* arena, obb2 boundingBox, BezierPath_t* pathOut)
void GenerateBezierPathForRoundedRectangle(MemArena_t* arena, rec rectangle, r32 cornerRadius, BezierPath_t* pathOut)
void GenerateBezierPathForRoundedObb2(MemArena_t* arena, obb2 boundingBox, r32 cornerRadius, BezierPath_t* pathOut)
*/
