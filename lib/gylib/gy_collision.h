/*
File:   gy_collision.h
Author: Taylor Robbins
Date:   10\09\2021
Description:
	** Contains a bunch of functions that help us calculate intersections and collision information between various primitive shapes in 2D and 3D
	
	NOTE: Depending on what files are included before this various functions may or may not be available
*/

#ifndef _GY_COLLISION_H
#define _GY_COLLISION_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct RayVsRectangle2DResult_t
{
	bool intersects;
	r32 enterTime;
	r32 exitTime;
	v2 enterPos;
	v2 exitPos;
	Dir2_t enterSide;
	Dir2_t exitSide;
};

struct RayVsObb2DResult_t
{
	bool intersects;
	r32 enterTime;
	r32 exitTime;
	v2 enterPos;
	v2 exitPos;
	Dir2_t enterSide;
	Dir2_t exitSide;
	v2 enterSideNormal;
	v2 exitSideNormal;
};

struct RayVsBoxResult_t
{
	bool intersects;
	r32 enterTime;
	r32 exitTime;
	v3 enterPos;
	v3 exitPos;
	Dir3_t enterSide;
	Dir3_t exitSide;
};

struct CircleVsRecResult_t
{
	bool intersects;
	v2 closestPoint;
	Dir2Ex_t recSide; //Will never be None
	v2 circleResolveVec;
};

struct SphereVsBoxResult_t
{
	bool intersects;
	v3 closestPoint;
	Dir3Ex_t boxSide; //Will never be None
	v3 sphereResolveVec;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	bool RayVsRectangle2D(Ray2_t ray, rec rectangle, RayVsRectangle2DResult_t* result, bool giveNegativeTimes = false);
	bool RayVsObb2D(Ray2_t ray, obb2 boundingBox, RayVsObb2DResult_t* result, bool giveNegativeTimes = false);
	bool RayVsBox(Ray3_t ray, box boundingBox, RayVsBoxResult_t* result, bool giveNegativeTimes = false, bool inclusive = true);
	v2 GetClosestPointInRec(rec rectangle, v2 point, Dir2Ex_t* dirOut = nullptr);
	v2 GetClosestPointOnRecSurface(rec rectangle, v2 point, Dir2Ex_t* dirOut = nullptr);
	bool CircleVsRec(Circle_t circle, rec rectangle, CircleVsRecResult_t* result);
	v3 GetClosestPointInBox(box boundingBox, v3 point, Dir3Ex_t* dirOut = nullptr);
	v3 GetClosestPointOnBoxSurface(box boundingBox, v3 point, Dir3Ex_t* dirOut = nullptr);
	bool SphereVsBox(Sphere_t sphere, box boundingBox, SphereVsBoxResult_t* result);
#else

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
#if defined(_GY_PRIMITIVES_H) && defined(_GY_RECTANGLES_H)
// TODO: bool FrustumVsAABB(Frustum_t frustum, AABB3_t aabb)
#endif

// TODO: bool SeperatingAxisCheck(u32 numNormals, const v2* normals, u32 numVerts1, const v2* verts1, u32 numVerts2, const v2* verts2, r32* depthOut = nullptr, v2* sepAxisOut = nullptr, r32* sepOffsetOut = nullptr)

bool RayVsRectangle2D(Ray2_t ray, rec rectangle, RayVsRectangle2DResult_t* result, bool giveNegativeTimes = false)
{
	NotNull(result);
	ClearPointer(result);
	
	if (ray.direction.x == 0 && ray.direction.y == 0) // Empty Ray
	{
		if (IsInsideRec(rectangle, ray.origin))
		{
			result->intersects = true;
			result->enterTime = 0;
			result->enterPos = ray.origin;
			result->enterSide = Dir2_None;
			result->exitTime = 0;
			result->exitPos = ray.origin;
			result->exitSide = Dir2_None;
		}
	}
	else if (ray.direction.x == 0) //Vertical Ray
	{
		if (ray.origin.x >= rectangle.x && ray.origin.x < rectangle.x + rectangle.width)
		{
			r32 enterDist = rectangle.y - ray.origin.y;
			r32 exitDist = (rectangle.y + rectangle.height) - ray.origin.y;
			r32 enterTime = (enterDist / ray.direction.y);
			r32 exitTime = (exitDist / ray.direction.y);
			if (enterTime > exitTime)
			{
				SWAP_VARIABLES(r32, enterDist, exitDist);
				SWAP_VARIABLES(r32, enterTime, exitTime);
			}
			
			if (exitTime >= 0)
			{
				if (!giveNegativeTimes) { enterTime = MaxR32(0, enterTime); }
				result->intersects = true;
				result->enterTime = enterTime;
				result->exitTime = exitTime;
				result->enterPos = ray.origin + (ray.direction * enterTime);
				result->exitPos = ray.origin + (ray.direction * exitTime);
				result->enterSide = (ray.direction.y > 0) ? Dir2_Up : Dir2_Down;
				result->exitSide = (ray.direction.y > 0) ? Dir2_Down : Dir2_Up;
			}
		}
	}
	else if (ray.direction.y == 0) //Horizontal Ray
	{
		if (ray.origin.y >= rectangle.y && ray.origin.y < rectangle.y + rectangle.height)
		{
			r32 enterDist = rectangle.x - ray.origin.x;
			r32 exitDist = (rectangle.x + rectangle.width) - ray.origin.x;
			r32 enterTime = (enterDist / ray.direction.x);
			r32 exitTime = (exitDist / ray.direction.x);
			if (enterTime > exitTime)
			{
				SWAP_VARIABLES(r32, enterDist, exitDist);
				SWAP_VARIABLES(r32, enterTime, exitTime);
			}
			
			if (exitTime >= 0)
			{
				if (!giveNegativeTimes) { enterTime = MaxR32(0, enterTime); }
				result->intersects = true;
				result->enterTime = enterTime;
				result->exitTime = exitTime;
				result->enterPos = ray.origin + (ray.direction * enterTime);
				result->exitPos = ray.origin + (ray.direction * exitTime);
				result->enterSide = (ray.direction.x > 0) ? Dir2_Left : Dir2_Right;
				result->exitSide = (ray.direction.x > 0) ? Dir2_Right : Dir2_Left;
			}
		}
	}
	else //Diagonal Ray
	{
		r32 horiEnterDist = rectangle.x - ray.origin.x;
		r32 horiExitDist = (rectangle.x + rectangle.width) - ray.origin.x;
		r32 horiEnterTime = (horiEnterDist / ray.direction.x);
		r32 horiExitTime = (horiExitDist / ray.direction.x);
		if (horiEnterTime > horiExitTime)
		{
			SWAP_VARIABLES(r32, horiEnterDist, horiExitDist);
			SWAP_VARIABLES(r32, horiEnterTime, horiExitTime);
		}
		
		r32 vertEnterDist = rectangle.y - ray.origin.y;
		r32 vertExitDist = (rectangle.y + rectangle.height) - ray.origin.y;
		r32 vertEnterTime = (vertEnterDist / ray.direction.y);
		r32 vertExitTime = (vertExitDist / ray.direction.y);
		if (vertEnterTime > vertExitTime)
		{
			SWAP_VARIABLES(r32, vertEnterDist, vertExitDist);
			SWAP_VARIABLES(r32, vertEnterTime, vertExitTime);
		}
		
		//Make sure the regions overlap and the overlap doesn't end before the ray starts (negative time values)
		if (horiExitTime >= 0 && vertExitTime >= 0 && horiExitTime >= vertEnterTime && vertExitTime >= horiEnterTime)
		{
			if (horiEnterTime >= vertEnterTime) //we enter last on horizontal-axis
			{
				if (!giveNegativeTimes) { horiEnterTime = MaxR32(0, horiEnterTime); }
				result->intersects = true;
				result->enterTime = horiEnterTime;
				result->enterPos = ray.origin + (ray.direction * horiEnterTime);
				result->enterSide = (ray.direction.x > 0) ? Dir2_Left : Dir2_Right;
			}
			else //we enter last on vertical-axis
			{
				if (!giveNegativeTimes) { vertEnterTime = MaxR32(0, vertEnterTime); }
				result->intersects = true;
				result->enterTime = vertEnterTime;
				result->enterPos = ray.origin + (ray.direction * vertEnterTime);
				result->enterSide = (ray.direction.y > 0) ? Dir2_Up : Dir2_Down;
			}
			
			if (horiExitTime <= vertExitTime) //we exit first on horizontal-axis
			{
				result->exitTime = horiExitTime;
				result->exitPos = ray.origin + (ray.direction * horiExitTime);
				result->exitSide = (ray.direction.x > 0) ? Dir2_Right : Dir2_Left;
			}
			else //we exit first on vertical-axis
			{
				result->exitTime = vertExitTime;
				result->exitPos = ray.origin + (ray.direction * vertExitTime);
				result->exitSide = (ray.direction.y > 0) ? Dir2_Down : Dir2_Up;
			}
		}
	}
	
	return result->intersects;
}

bool RayVsObb2D(Ray2_t ray, obb2 boundingBox, RayVsObb2DResult_t* result, bool giveNegativeTimes = false)
{
	NotNull(result);
	ClearPointer(result);
	
	Ray2_t alignedRay;
	alignedRay.origin = Vec2Rotate(ray.origin - boundingBox.center, -boundingBox.rotation);
	alignedRay.direction = Vec2Rotate(ray.direction, -boundingBox.rotation);
	rec alignedRec = NewRecCentered(Vec2_Zero, boundingBox.size);
	
	RayVsRectangle2DResult_t recResult;
	if (RayVsRectangle2D(alignedRay, alignedRec, &recResult, giveNegativeTimes))
	{
		result->intersects = recResult.intersects;
		result->enterTime = recResult.enterTime;
		result->exitTime = recResult.exitTime;
		result->enterPos = boundingBox.center + Vec2Rotate(recResult.enterPos, boundingBox.rotation);
		result->exitPos = boundingBox.center + Vec2Rotate(recResult.exitPos, boundingBox.rotation);
		result->enterSide = recResult.enterSide;
		result->exitSide = recResult.exitSide;
		result->enterSideNormal = Vec2FromAngle(boundingBox.rotation + GetDir2AngleR32(recResult.enterSide));
		result->exitSideNormal = Vec2FromAngle(boundingBox.rotation + GetDir2AngleR32(recResult.exitSide));
		return true;
	}
	else
	{
		return false;
	}
}

bool RayVsBox(Ray3_t ray, box boundingBox, RayVsBoxResult_t* result, bool giveNegativeTimes = false, bool inclusive = true)
{
	NotNull(result);
	ClearPointer(result);
	result->intersects = false;
	
	const r32 dumbInfinite = 10000; //TODO: Get rid of the need for this
	if (ray.direction == Vec3_Zero)
	{
		if (IsInsideBox(boundingBox, ray.origin))
		{
			result->intersects = true;
			result->enterTime = 0;
			result->exitTime = dumbInfinite;
			result->enterPos = ray.origin;
			result->exitPos = ray.origin;
			result->enterSide = Dir3_None;
			result->exitSide = Dir3_None;
			return true;
		}
	}
	
	//don't start inside on an axis and not moving on that axis. Will never intersect
	if (ray.direction.x == 0 && (ray.origin.x < boundingBox.x || ray.origin.x > boundingBox.x + boundingBox.width  || (!inclusive && ray.origin.x == boundingBox.x + boundingBox.width)))  { return false; }
	if (ray.direction.y == 0 && (ray.origin.y < boundingBox.y || ray.origin.y > boundingBox.y + boundingBox.height || (!inclusive && ray.origin.y == boundingBox.y + boundingBox.height))) { return false; }
	if (ray.direction.z == 0 && (ray.origin.z < boundingBox.z || ray.origin.z > boundingBox.z + boundingBox.depth  || (!inclusive && ray.origin.z == boundingBox.z + boundingBox.depth)))  { return false; }
	
	v3 positiveRayDir = NewVec3(AbsR32(ray.direction.x), AbsR32(ray.direction.y), AbsR32(ray.direction.z));
	r32 startX = (ray.direction.x >= 0) ? (boundingBox.x - ray.origin.x)                        : ((boundingBox.x + boundingBox.width) - ray.origin.x);
	r32 endX   = (ray.direction.x >= 0) ? ((boundingBox.x + boundingBox.width) - ray.origin.x)  : (boundingBox.x - ray.origin.x);
	r32 startY = (ray.direction.y >= 0) ? (boundingBox.y - ray.origin.y)                        : ((boundingBox.y + boundingBox.height) - ray.origin.y);
	r32 endY   = (ray.direction.y >= 0) ? ((boundingBox.y + boundingBox.height) - ray.origin.y) : (boundingBox.y - ray.origin.y);
	r32 startZ = (ray.direction.z >= 0) ? (boundingBox.z - ray.origin.z)                        : ((boundingBox.z + boundingBox.depth) - ray.origin.z);
	r32 endZ   = (ray.direction.z >= 0) ? ((boundingBox.z + boundingBox.depth) - ray.origin.z)  : (boundingBox.z - ray.origin.z);
	
	r32 startTimeX = (ray.direction.x != 0) ? (startX / ray.direction.x) : -dumbInfinite;
	r32 endTimeX   = (ray.direction.x != 0) ? (endX   / ray.direction.x) :  dumbInfinite;
	r32 startTimeY = (ray.direction.y != 0) ? (startY / ray.direction.y) : -dumbInfinite;
	r32 endTimeY   = (ray.direction.y != 0) ? (endY   / ray.direction.y) :  dumbInfinite;
	r32 startTimeZ = (ray.direction.z != 0) ? (startZ / ray.direction.z) : -dumbInfinite;
	r32 endTimeZ   = (ray.direction.z != 0) ? (endZ   / ray.direction.z) :  dumbInfinite;
	
	//our last start time is greater than our first end time. No intersection
	if (ray.direction.x != 0 && ((ray.direction.y != 0 && endTimeX <= startTimeY) || (ray.direction.z != 0 && endTimeX <= startTimeZ)))
	{
		return false;
	}
	if (ray.direction.y != 0 && ((ray.direction.x != 0 && endTimeY <= startTimeX) || (ray.direction.z != 0 && endTimeY <= startTimeZ)))
	{
		return false;
	}
	if (ray.direction.z != 0 && ((ray.direction.x != 0 && endTimeZ <= startTimeX) || (ray.direction.y != 0 && endTimeZ <= startTimeY)))
	{
		return false;
	}
	
	r32 startTime = 0.0f;
	Dir3_t startSide = Dir3_None;
	if (startTimeX != dumbInfinite && (startTimeY == dumbInfinite || startTimeX >= startTimeY) && (startTimeZ == dumbInfinite || startTimeX >= startTimeZ))
	{
		startTime = startTimeX;
		startSide = (ray.direction.x > 0) ? Dir3_Left : Dir3_Right;
	}
	else if (startTimeY != dumbInfinite && (startTimeZ == dumbInfinite || startTimeY >= startTimeZ))
	{
		startTime = startTimeY;
		startSide = (ray.direction.y > 0) ? Dir3_Down : Dir3_Up;
	}
	else if (startTimeZ != dumbInfinite)
	{
		startTime = startTimeZ;
		startSide = (ray.direction.z > 0) ? Dir3_Backward : Dir3_Forward;
	}
	else { Assert(false); return false; }
	if (!giveNegativeTimes && startTime < 0) { startTime = 0; }
	
	r32 endTime = 0.0f;
	Dir3_t endSide = Dir3_None;
	if (endTimeX != dumbInfinite && (endTimeY == dumbInfinite || endTimeX <= endTimeY) && (endTimeZ == dumbInfinite || endTimeX <= endTimeZ))
	{
		endTime = endTimeX;
		endSide = (ray.direction.x > 0) ? Dir3_Right : Dir3_Left;
	}
	else if (endTimeY != dumbInfinite && (endTimeZ == dumbInfinite || endTimeY <= endTimeZ))
	{
		endTime = endTimeY;
		endSide = (ray.direction.y > 0) ? Dir3_Up : Dir3_Down;
	}
	else if (endTimeZ != dumbInfinite)
	{
		endTime = endTimeZ;
		endSide = (ray.direction.z > 0) ? Dir3_Forward : Dir3_Backward;
	}
	else { Assert(false); return false; }
	if (endTime < 0) { return false; } //intersection happened in opposite direction (negative time)
	
	result->intersects = true;
	result->enterTime = startTime;
	result->exitTime = endTime;
	result->enterPos = ray.origin + (ray.direction * startTime);
	result->exitPos = ray.origin + (ray.direction * endTime);
	result->enterSide = startSide;
	result->exitSide = endSide;
	return true;
}

//Returns Dir2Ex_None when point is inside rectangle
v2 GetClosestPointInRec(rec rectangle, v2 point, Dir2Ex_t* dirOut = nullptr)
{
	if (point.x < rectangle.x + rectangle.width)
	{
		if (point.x > rectangle.x)
		{
			if (point.y < rectangle.y)
			{
				SetOptionalOutPntr(dirOut, Dir2Ex_Up);
				return NewVec2(point.x, rectangle.y);
			}
			else if (point.y > rectangle.y + rectangle.height)
			{
				SetOptionalOutPntr(dirOut, Dir2Ex_Down);
				return NewVec2(point.x, rectangle.y + rectangle.height);
			}
			else
			{
				SetOptionalOutPntr(dirOut, Dir2Ex_None);
				return point;
			}
		}
		else
		{
			if (point.y < rectangle.y + rectangle.height)
			{
				if (point.y > rectangle.y)
				{
					SetOptionalOutPntr(dirOut, Dir2Ex_Left);
					return NewVec2(rectangle.x, point.y);
				}
				else
				{
					SetOptionalOutPntr(dirOut, Dir2Ex_TopLeft);
					return NewVec2(rectangle.x, rectangle.y);
				}
			}
			else
			{
				SetOptionalOutPntr(dirOut, Dir2Ex_BottomLeft);
				return NewVec2(rectangle.x, rectangle.y + rectangle.height);
			}
		}
	}
	else
	{
		if (point.y < rectangle.y + rectangle.height)
		{
			if (point.y > rectangle.y)
			{
				SetOptionalOutPntr(dirOut, Dir2Ex_Right);
				return NewVec2(rectangle.x + rectangle.width, point.y);
			}
			else
			{
				SetOptionalOutPntr(dirOut, Dir2Ex_TopRight);
				return NewVec2(rectangle.x + rectangle.width, rectangle.y);
			}
		}
		else
		{
			SetOptionalOutPntr(dirOut, Dir2Ex_BottomRight);
			return NewVec2(rectangle.x + rectangle.width, rectangle.y + rectangle.height);
		}
	}
}
//Never returns Dir2Ex_None
v2 GetClosestPointOnRecSurface(rec rectangle, v2 point, Dir2Ex_t* dirOut = nullptr)
{
	Dir2Ex_t recSide = Dir2Ex_None;
	v2 result = GetClosestPointInRec(rectangle, point, &recSide);
	if (recSide == Dir2Ex_None)
	{
		r32 resolveX = rectangle.x + ((point.x > (rectangle.x + rectangle.width/2))  ? rectangle.width  : 0) - point.x;
		r32 resolveY = rectangle.y + ((point.y > (rectangle.y + rectangle.height/2)) ? rectangle.height : 0) - point.y;
		if (AbsR32(resolveX) < AbsR32(resolveY))
		{
			recSide = (resolveX >= 0) ? Dir2Ex_Right : Dir2Ex_Left;
			result.x += resolveX;
		}
		else
		{
			recSide = (resolveY >= 0) ? Dir2Ex_Down : Dir2Ex_Up;
			result.y += resolveY;
		}
	}
	SetOptionalOutPntr(dirOut, recSide);
	return result;
}

bool CircleVsRec(Circle_t circle, rec rectangle, CircleVsRecResult_t* result)
{
	NotNull(result);
	ClearPointer(result);
	
	result->closestPoint = GetClosestPointInRec(rectangle, circle.center, &result->recSide);
	
	r32 distSquared = Vec2LengthSquared(circle.center - result->closestPoint);
	result->intersects = (distSquared < Square(circle.radius));
	
	if (result->closestPoint == circle.center)
	{
		r32 resolveX = rectangle.x + ((circle.x > (rectangle.x + rectangle.width/2))  ? rectangle.width  + circle.radius : -circle.radius) - circle.x;
		r32 resolveY = rectangle.y + ((circle.y > (rectangle.y + rectangle.height/2)) ? rectangle.height + circle.radius : -circle.radius) - circle.y;
		if (AbsR32(resolveX) < AbsR32(resolveY))
		{
			result->circleResolveVec = NewVec2(resolveX, 0);
			result->recSide = (resolveX >= 0) ? Dir2Ex_Right : Dir2Ex_Left;
		}
		else
		{
			result->circleResolveVec = NewVec2(0, resolveY);
			result->recSide = (resolveY >= 0) ? Dir2Ex_Down : Dir2Ex_Up;
		}
	}
	else
	{
		result->circleResolveVec = result->closestPoint - (circle.center + (Vec2Normalize(result->closestPoint - circle.center) * circle.radius));
	}
	
	return result->intersects;
}
bool CircleVsRec(Circle_t circle, rec rectangle)
{
	CircleVsRecResult_t result;
	return CircleVsRec(circle, rectangle, &result);
}

//Returns Dir3Ex_None when point is inside the box
v3 GetClosestPointInBox(box boundingBox, v3 point, Dir3Ex_t* dirOut = nullptr)
{
	if (point.x < boundingBox.x + boundingBox.width)
	{
		if (point.x > boundingBox.x)
		{
			if (point.y < boundingBox.y + boundingBox.height)
			{
				if (point.y > boundingBox.y)
				{
					if (point.z < boundingBox.z + boundingBox.depth)
					{
						if (point.z > boundingBox.z)
						{
							//inside
							SetOptionalOutPntr(dirOut, Dir3Ex_None);
							return point;
						}
						else
						{
							SetOptionalOutPntr(dirOut, Dir3Ex_Backward);
							return NewVec3(point.x, point.y, boundingBox.z);
						}
					}
					else
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_Forward);
						return NewVec3(point.x, point.y, boundingBox.z + boundingBox.depth);
					}
				}
				else
				{
					//bottom
					if (point.z < boundingBox.z + boundingBox.depth)
					{
						if (point.z > boundingBox.z)
						{
							SetOptionalOutPntr(dirOut, Dir3Ex_Down);
							return NewVec3(point.x, boundingBox.y, point.z);
						}
						else
						{
							SetOptionalOutPntr(dirOut, Dir3Ex_BottomBack);
							return NewVec3(point.x, boundingBox.y, boundingBox.z);
						}
					}
					else
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_BottomFront);
						return NewVec3(point.x, boundingBox.y, boundingBox.z + boundingBox.depth);
					}
				}
			}
			else
			{
				//top
				if (point.z < boundingBox.z + boundingBox.depth)
				{
					if (point.z > boundingBox.z)
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_Up);
						return NewVec3(point.x, boundingBox.y + boundingBox.height, point.z);
					}
					else
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_TopBack);
						return NewVec3(point.x, boundingBox.y + boundingBox.height, boundingBox.z);
					}
				}
				else
				{
					SetOptionalOutPntr(dirOut, Dir3Ex_TopFront);
					return NewVec3(point.x, boundingBox.y + boundingBox.height, boundingBox.z + boundingBox.depth);
				}
			}
		}
		else
		{
			//left
			if (point.y < boundingBox.y + boundingBox.height)
			{
				if (point.y > boundingBox.y)
				{
					if (point.z < boundingBox.z + boundingBox.depth)
					{
						if (point.z > boundingBox.z)
						{
							SetOptionalOutPntr(dirOut, Dir3Ex_Left);
							return NewVec3(boundingBox.x, point.y, point.z);
						}
						else
						{
							SetOptionalOutPntr(dirOut, Dir3Ex_BottomFront);
							return NewVec3(boundingBox.x, point.y, boundingBox.z);
						}
					}
					else
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_LeftFront);
						return NewVec3(boundingBox.x, point.y, boundingBox.z + boundingBox.depth);
					}
				}
				else
				{
					//bottom
					if (point.z < boundingBox.z + boundingBox.depth)
					{
						if (point.z > boundingBox.z)
						{
							SetOptionalOutPntr(dirOut, Dir3Ex_BottomLeft);
							return NewVec3(boundingBox.x, boundingBox.y, point.z);
						}
						else
						{
							SetOptionalOutPntr(dirOut, Dir3Ex_BottomLeftBack);
							return NewVec3(boundingBox.x, boundingBox.y, boundingBox.z);
						}
					}
					else
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_BottomLeftFront);
						return NewVec3(boundingBox.x, boundingBox.y, boundingBox.z + boundingBox.depth);
					}
				}
			}
			else
			{
				//top
				if (point.z < boundingBox.z + boundingBox.depth)
				{
					if (point.z > boundingBox.z)
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_TopLeft);
						return NewVec3(boundingBox.x, boundingBox.y + boundingBox.height, point.z);
					}
					else
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_TopLeftBack);
						return NewVec3(boundingBox.x, boundingBox.y + boundingBox.height, boundingBox.z);
					}
				}
				else
				{
					SetOptionalOutPntr(dirOut, Dir3Ex_TopLeftFront);
					return NewVec3(boundingBox.x, boundingBox.y + boundingBox.height, boundingBox.z + boundingBox.depth);
				}
			}
		}
	}
	else
	{
		//right
		if (point.y < boundingBox.y + boundingBox.height)
		{
			if (point.y > boundingBox.y)
			{
				if (point.z < boundingBox.z + boundingBox.depth)
				{
					if (point.z > boundingBox.z)
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_Right);
						return NewVec3(boundingBox.x + boundingBox.width, point.y, point.z);
					}
					else
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_RightBack);
						return NewVec3(boundingBox.x + boundingBox.width, point.y, boundingBox.z);
					}
				}
				else
				{
					SetOptionalOutPntr(dirOut, Dir3Ex_RightFront);
					return NewVec3(boundingBox.x + boundingBox.width, point.y, boundingBox.z + boundingBox.depth);
				}
			}
			else
			{
				//bottom
				if (point.z < boundingBox.z + boundingBox.depth)
				{
					if (point.z > boundingBox.z)
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_BottomRight);
						return NewVec3(boundingBox.x + boundingBox.width, boundingBox.y, point.z);
					}
					else
					{
						SetOptionalOutPntr(dirOut, Dir3Ex_BottomRightBack);
						return NewVec3(boundingBox.x + boundingBox.width, boundingBox.y, boundingBox.z);
					}
				}
				else
				{
					SetOptionalOutPntr(dirOut, Dir3Ex_BottomRightFront);
					return NewVec3(boundingBox.x + boundingBox.width, boundingBox.y, boundingBox.z + boundingBox.depth);
				}
			}
		}
		else
		{
			//top
			if (point.z < boundingBox.z + boundingBox.depth)
			{
				if (point.z > boundingBox.z)
				{
					SetOptionalOutPntr(dirOut, Dir3Ex_TopRight);
					return NewVec3(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height, point.z);
				}
				else
				{
					SetOptionalOutPntr(dirOut, Dir3Ex_TopRightBack);
					return NewVec3(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height, boundingBox.z);
				}
			}
			else
			{
				SetOptionalOutPntr(dirOut, Dir3Ex_TopRightFront);
				return NewVec3(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height, boundingBox.z + boundingBox.depth);
			}
		}
	}
}
//Never returns Dir3Ex_None
v3 GetClosestPointOnBoxSurface(box boundingBox, v3 point, Dir3Ex_t* dirOut = nullptr)
{
	Dir3Ex_t boxSide = Dir3Ex_None;
	v3 result = GetClosestPointInBox(boundingBox, point, &boxSide);
	if (boxSide == Dir3Ex_None)
	{
		r32 resolveX = boundingBox.x + ((point.x > (boundingBox.x + boundingBox.width/2))  ? boundingBox.width  : 0) - point.x;
		r32 resolveY = boundingBox.y + ((point.y > (boundingBox.y + boundingBox.height/2)) ? boundingBox.height : 0) - point.y;
		r32 resolveZ = boundingBox.z + ((point.z > (boundingBox.z + boundingBox.depth/2))  ? boundingBox.depth  : 0) - point.z;
		if (AbsR32(resolveX) < AbsR32(resolveY) && AbsR32(resolveX) < AbsR32(resolveZ))
		{
			boxSide = (resolveX >= 0) ? Dir3Ex_Right : Dir3Ex_Left;
			result.x += resolveX;
		}
		else if (AbsR32(resolveY) < AbsR32(resolveZ))
		{
			boxSide = (resolveY >= 0) ? Dir3Ex_Up : Dir3Ex_Down;
			result.y += resolveY;
		}
		else
		{
			boxSide = (resolveZ >= 0) ? Dir3Ex_Forward : Dir3Ex_Backward;
			result.z += resolveZ;
		}
	}
	SetOptionalOutPntr(dirOut, boxSide);
	return result;
}

bool SphereVsBox(Sphere_t sphere, box boundingBox, SphereVsBoxResult_t* result)
{
	NotNull(result);
	ClearPointer(result);
	
	result->closestPoint = GetClosestPointInBox(boundingBox, sphere.center, &result->boxSide);
	
	r32 distSquared = Vec3LengthSquared(sphere.center - result->closestPoint);
	result->intersects = (distSquared < Square(sphere.radius));
	
	if (result->closestPoint == sphere.center)
	{
		r32 resolveX = boundingBox.x + ((sphere.x > (boundingBox.x + boundingBox.width/2))  ? boundingBox.width  + sphere.radius : -sphere.radius) - sphere.x;
		r32 resolveY = boundingBox.y + ((sphere.y > (boundingBox.y + boundingBox.height/2)) ? boundingBox.height + sphere.radius : -sphere.radius) - sphere.y;
		r32 resolveZ = boundingBox.z + ((sphere.z > (boundingBox.z + boundingBox.depth/2))  ? boundingBox.depth  + sphere.radius : -sphere.radius) - sphere.z;
		if (AbsR32(resolveX) < AbsR32(resolveY) && AbsR32(resolveX) < AbsR32(resolveZ))
		{
			result->sphereResolveVec = NewVec3(resolveX, 0, 0);
			result->boxSide = (resolveX >= 0) ? Dir3Ex_Right : Dir3Ex_Left;
		}
		else if (AbsR32(resolveY) < AbsR32(resolveZ))
		{
			result->sphereResolveVec = NewVec3(0, resolveY, 0);
			result->boxSide = (resolveY >= 0) ? Dir3Ex_Up : Dir3Ex_Down;
		}
		else
		{
			result->sphereResolveVec = NewVec3(0, 0, resolveZ);
			result->boxSide = (resolveZ >= 0) ? Dir3Ex_Forward : Dir3Ex_Backward;
		}
	}
	else
	{
		result->sphereResolveVec = result->closestPoint - (sphere.center + (Vec3Normalize(result->closestPoint - sphere.center) * sphere.radius));
	}
	
	return result->intersects;
}
bool SphereVsBox(Sphere_t sphere, box boundingBox)
{
	SphereVsBoxResult_t result;
	return SphereVsBox(sphere, boundingBox, &result);
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_COLLISION_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
RayVsRectangle2DResult_t
RayVsObb2DResult_t
RayVsBoxResult_t
CircleVsRecResult_t
@Functions
bool RayVsRectangle2D(Ray2_t ray, rec rectangle, RayVsRectangle2DResult_t* result, bool giveNegativeTimes = false)
bool RayVsObb2D(Ray2_t ray, obb2 boundingBox, RayVsObb2DResult_t* result, bool giveNegativeTimes = false)
bool RayVsBox(Ray3_t ray, box boundingBox, RayVsBoxResult_t* result, bool giveNegativeTimes = false, bool inclusive = true)
v2 GetClosestPointInRec(rec rectangle, v2 point, Dir2Ex_t* dirOut = nullptr)
v2 GetClosestPointOnRecSurface(rec rectangle, v2 point, Dir2Ex_t* dirOut = nullptr)
bool CircleVsRec(Circle_t circle, rec rectangle, CircleVsRecResult_t* result = nullptr)
v3 GetClosestPointInBox(box boundingBox, v3 point, Dir3Ex_t* dirOut = nullptr)
v3 GetClosestPointOnBoxSurface(box boundingBox, v3 point, Dir3Ex_t* dirOut = nullptr)
bool SphereVsBox(Sphere_t sphere, box boundingBox, SphereVsBoxResult_t* result = nullptr)
*/
