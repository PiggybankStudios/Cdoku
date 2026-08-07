/*
File:   gy_vectors.h
Author: Taylor Robbins
Date:   09\14\2021
Description:
	** Contains the definitions and related functions for v2, v2i, v3, v3i, v4, and v4i
*/

#ifndef _GY_VECTORS_H
#define _GY_VECTORS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
//TODO: Can we move the += and -= operator overloads to the bottom of the file??
union Vector2_t
{
	r32 values[2];
	struct { r32 x, y; };
	struct { r32 width, height; };
	struct { r32 horizontal, vertical; };
	
	inline Vector2_t& operator += (const Vector2_t& rightSide)
	{
		this->x += rightSide.x;
		this->y += rightSide.y;
		return *this;
	}
	inline Vector2_t& operator -= (const Vector2_t& rightSide)
	{
		this->x -= rightSide.x;
		this->y -= rightSide.y;
		return *this;
	}
};
union Vector3_t
{
	r32 values[3];
	struct { r32 x, y, z; };
	struct { r32 width, height, depth; };
	struct { r32 r, g, b; };
	struct { r32 red, green, blue; };
	
	inline Vector3_t& operator += (const Vector3_t& rightSide)
	{
		this->x += rightSide.x;
		this->y += rightSide.y;
		this->z += rightSide.z;
		return *this;
	}
	inline Vector3_t& operator -= (const Vector3_t& rightSide)
	{
		this->x -= rightSide.x;
		this->y -= rightSide.y;
		this->z -= rightSide.z;
		return *this;
	}
};
union Vector4_t
{
	r32 values[4];
	struct { r32 x, y, z, w; };
	struct { r32 width, height, depth, time; };
	struct { r32 r, g, b, a; };
	struct { r32 red, green, blue, alpha; };
	
	inline Vector4_t& operator += (const Vector4_t& rightSide)
	{
		this->x += rightSide.x;
		this->y += rightSide.y;
		this->z += rightSide.z;
		this->w += rightSide.w;
		return *this;
	}
	inline Vector4_t& operator -= (const Vector4_t& rightSide)
	{
		this->x -= rightSide.x;
		this->y -= rightSide.y;
		this->z -= rightSide.z;
		this->w -= rightSide.w;
		return *this;
	}
};

union Vector2i_t
{
	i32 values[2];
	struct { i32 x, y; };
	struct { i32 width, height; };
	struct { i32 horizontal, vertical; };
	struct { i32 columns, rows; };
	
	inline Vector2i_t& operator += (const Vector2i_t& rightSide)
	{
		this->x += rightSide.x;
		this->y += rightSide.y;
		return *this;
	}
	inline Vector2i_t& operator -= (const Vector2i_t& rightSide)
	{
		this->x -= rightSide.x;
		this->y -= rightSide.y;
		return *this;
	}
};
union Vector3i_t
{
	i32 values[3];
	struct { i32 x, y, z; };
	struct { i32 width, height, depth; };
	
	inline Vector3i_t& operator += (const Vector3i_t& rightSide)
	{
		this->x += rightSide.x;
		this->y += rightSide.y;
		this->z += rightSide.z;
		return *this;
	}
	inline Vector3i_t& operator -= (const Vector3i_t& rightSide)
	{
		this->x -= rightSide.x;
		this->y -= rightSide.y;
		this->z -= rightSide.z;
		return *this;
	}
};
union Vector4i_t
{
	i32 values[4];
	struct { i32 x, y, z, w; };
	struct { i32 width, height, depth, time; };
	
	inline Vector4i_t& operator += (const Vector4i_t& rightSide)
	{
		this->x += rightSide.x;
		this->y += rightSide.y;
		this->z += rightSide.z;
		this->w += rightSide.w;
		return *this;
	}
	inline Vector4i_t& operator -= (const Vector4i_t& rightSide)
	{
		this->x -= rightSide.x;
		this->y -= rightSide.y;
		this->z -= rightSide.z;
		this->w -= rightSide.w;
		return *this;
	}
};

typedef Vector2_t v2;
typedef Vector3_t v3;
typedef Vector4_t v4;

typedef Vector2i_t v2i;
typedef Vector3i_t v3i;
typedef Vector4i_t v4i;

// +--------------------------------------------------------------+
// |                   Simple Value Definitions                   |
// +--------------------------------------------------------------+
#define Vec2_Zero      NewVec2( 0.0f,  0.0f)
#define Vec2_One       NewVec2( 1.0f,  1.0f)
#define Vec2_Half      NewVec2( 0.5f,  0.5f)
#define Vec2_Left      NewVec2(-1.0f,  0.0f)
#define Vec2_Right     NewVec2( 1.0f,  0.0f)
#define Vec2_Up        NewVec2( 0.0f, -1.0f)
#define Vec2_Down      NewVec2( 0.0f,  1.0f)

#define Vec2i_Zero     NewVec2i( 0,  0)
#define Vec2i_One      NewVec2i( 1,  1)
#define Vec2i_Left     NewVec2i(-1,  0)
#define Vec2i_Right    NewVec2i( 1,  0)
#define Vec2i_Up       NewVec2i( 0, -1)
#define Vec2i_Down     NewVec2i( 0,  1)

#define Vec3_Zero      NewVec3( 0.0f,  0.0f,  0.0f)
#define Vec3_One       NewVec3( 1.0f,  1.0f,  1.0f)
#define Vec3_Half      NewVec3( 0.5f,  0.5f,  0.5f)
#define Vec3_Left      NewVec3(-1.0f,  0.0f,  0.0f)
#define Vec3_Right     NewVec3( 1.0f,  0.0f,  0.0f)
#define Vec3_Bottom    NewVec3( 0.0f, -1.0f,  0.0f)
#define Vec3_Top       NewVec3( 0.0f,  1.0f,  0.0f)
#define Vec3_Back      NewVec3( 0.0f,  0.0f, -1.0f)
#define Vec3_Front     NewVec3( 0.0f,  0.0f,  1.0f)
#define Vec3_Down      NewVec3( 0.0f, -1.0f,  0.0f)
#define Vec3_Up        NewVec3( 0.0f,  1.0f,  0.0f)
#define Vec3_Backward  NewVec3( 0.0f,  0.0f, -1.0f)
#define Vec3_Forward   NewVec3( 0.0f,  0.0f,  1.0f)

#define Vec3i_Zero     NewVec3i( 0,  0,  0)
#define Vec3i_One      NewVec3i( 1,  1,  1)
#define Vec3i_Left     NewVec3i(-1,  0,  0)
#define Vec3i_Right    NewVec3i( 1,  0,  0)
#define Vec3i_Bottom   NewVec3i( 0, -1,  0)
#define Vec3i_Top      NewVec3i( 0,  1,  0)
#define Vec3i_Back     NewVec3i( 0,  0, -1)
#define Vec3i_Front    NewVec3i( 0,  0,  1)
#define Vec3i_Down     NewVec3i( 0, -1,  0)
#define Vec3i_Up       NewVec3i( 0,  1,  0)
#define Vec3i_Backward NewVec3i( 0,  0, -1)
#define Vec3i_Forward  NewVec3i( 0,  0,  1)

#define Vec4_Zero      NewVec4( 0.0f,  0.0f,  0.0f, 0.0f)
#define Vec4_One       NewVec4( 1.0f,  1.0f,  1.0f, 1.0f)
#define Vec4_Half      NewVec4( 0.5f,  0.5f,  0.5f, 0.5f)
#define Vec4_Left      NewVec4(-1.0f,  0.0f,  0.0f, 0.0f)
#define Vec4_Right     NewVec4( 1.0f,  0.0f,  0.0f, 0.0f)
#define Vec4_Bottom    NewVec4( 0.0f, -1.0f,  0.0f, 0.0f)
#define Vec4_Top       NewVec4( 0.0f,  1.0f,  0.0f, 0.0f)
#define Vec4_Back      NewVec4( 0.0f,  0.0f, -1.0f, 0.0f)
#define Vec4_Front     NewVec4( 0.0f,  0.0f,  1.0f, 0.0f)
#define Vec4_Down      NewVec4( 0.0f, -1.0f,  0.0f, 0.0f)
#define Vec4_Up        NewVec4( 0.0f,  1.0f,  0.0f, 0.0f)
#define Vec4_Backward  NewVec4( 0.0f,  0.0f, -1.0f, 0.0f)
#define Vec4_Forward   NewVec4( 0.0f,  0.0f,  1.0f, 0.0f)
#define Vec4_Prevward  NewVec4( 0.0f,  0.0f,  0.0f,-1.0f)
#define Vec4_Nextward  NewVec4( 0.0f,  0.0f,  0.0f, 1.0f)

#define Vec4i_Zero      NewVec4i( 0,  0,  0, 0)
#define Vec4i_One       NewVec4i( 1,  1,  1, 0)
#define Vec4i_Left      NewVec4i(-1,  0,  0, 0)
#define Vec4i_Right     NewVec4i( 1,  0,  0, 0)
#define Vec4i_Bottom    NewVec4i( 0, -1,  0, 0)
#define Vec4i_Top       NewVec4i( 0,  1,  0, 0)
#define Vec4i_Back      NewVec4i( 0,  0, -1, 0)
#define Vec4i_Front     NewVec4i( 0,  0,  1, 0)
#define Vec4i_Down      NewVec4i( 0, -1,  0, 0)
#define Vec4i_Up        NewVec4i( 0,  1,  0, 0)
#define Vec4i_Backward  NewVec4i( 0,  0, -1, 0)
#define Vec4i_Forward   NewVec4i( 0,  0,  1, 0)
#define Vec4i_Prevward  NewVec4i( 0,  0,  0,-1)
#define Vec4i_Nextward  NewVec4i( 0,  0,  0, 1)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	Vector2_t NewVec2(r32 x, r32 y);
	Vector3_t NewVec3(r32 x, r32 y, r32 z);
	Vector4_t NewVec4(r32 x, r32 y, r32 z, r32 w);
	Vector2i_t NewVec2i(i32 x, i32 y);
	Vector3i_t NewVec3i(i32 x, i32 y, i32 z);
	Vector4i_t NewVec4i(i32 x, i32 y, i32 z, i32 w);
	v2 ToVec2(v2i vector);
	v3 ToVec3(v3i vector);
	v4 ToVec4(v4i vector);
	v4 ToVec4(v3 vector, r32 w);
	v2 Vec3_xy(v3 vector);
	v2 Vec3_xz(v3 vector);
	v2 Vec3_yz(v3 vector);
	v2 Vec2Floor(v2 vector);
	v2i Vec2Floori(v2 vector);
	v3 Vec3Floor(v3 vector);
	v3i Vec3Floori(v3 vector);
	v4 Vec4Floor(v4 vector);
	v4i Vec4Floori(v4 vector);
	v2 Vec2Ceil(v2 vector);
	v2i Vec2Ceili(v2 vector);
	v3 Vec3Ceil(v3 vector);
	v3i Vec3Ceili(v3 vector);
	v4 Vec4Ceil(v4 vector);
	v4i Vec4Ceili(v4 vector);
	v2 Vec2Round(v2 vector);
	v2i Vec2Roundi(v2 vector);
	v3 Vec3Round(v3 vector);
	v3i Vec3Roundi(v3 vector);
	v4 Vec4Round(v4 vector);
	v4i Vec4Roundi(v4 vector);
	v2 Vec2Max(v2 left, v2 right);
	v2i Vec2iMax(v2i left, v2i right);
	v3 Vec3Max(v3 left, v3 right);
	v3i Vec3iMax(v3i left, v3i right);
	v4 Vec4Max(v4 left, v4 right);
	v4i Vec4iMax(v4i left, v4i right);
	v2 Vec2Min(v2 left, v2 right);
	v2i Vec2iMin(v2i left, v2i right);
	v3 Vec3Min(v3 left, v3 right);
	v3i Vec3iMin(v3i left, v3i right);
	v4 Vec4Min(v4 left, v4 right);
	v4i Vec4iMin(v4i left, v4i right);
	v2i Vec2iAbs(v2i vector);
	v2 Vec2Abs(v2 vector);
	v3i Vec3iAbs(v3i vector);
	v3 Vec3Abs(v3 vector);
	v4i Vec4iAbs(v4i vector);
	v4 Vec4Abs(v4 vector);
	v2 Vec2Fill(r32 value);
	v2i Vec2iFill(i32 value);
	v3 Vec3Fill(r32 value);
	v3i Vec3iFill(i32 value);
	v4 Vec4Fill(r32 value);
	v4i Vec4iFill(i32 value);
	v3 Vec3FromVec2(v2 vector, r32 z);
	v3i Vec3iFromVec2i(v2i vector, i32 z);
	v4 Vec4FromVec3(v3 vector, r32 w);
	v4i Vec4iFromVec3i(v3i vector, i32 w);
	v2 Vec2Add(v2 left, v2 right);
	v2 Vec2Subtract(v2 left, v2 right);
	v2 Vec2Scale(v2 vector, r32 scalar);
	v2 Vec2Multiply(v2 left, v2 right);
	v2 Vec2Shrink(v2 vector, r32 divisor);
	v2 Vec2Divide(v2 left, v2 right);
	bool Vec2BasicallyEqual(v2 left, v2 right, r32 tolerance = 0.001f);
	v2i Vec2iAdd(v2i left, v2i right);
	v2i Vec2iSubtract(v2i left, v2i right);
	v2i Vec2iScale(v2i vector, i32 scalar);
	v2i Vec2iMultiply(v2i left, v2i right);
	bool Vec2iEqual(v2i left, v2i right);
	v3 Vec3Add(v3 left, v3 right);
	v3 Vec3Subtract(v3 left, v3 right);
	v3 Vec3Scale(v3 vector, r32 scalar);
	v3 Vec3Multiply(v3 left, v3 right);
	v3 Vec3Shrink(v3 vector, r32 divisor);
	v3 Vec3Divide(v3 left, v3 right);
	bool Vec3BasicallyEqual(v3 left, v3 right, r32 tolerance = 0.001f);
	v3i Vec3iAdd(v3i left, v3i right);
	v3i Vec3iSubtract(v3i left, v3i right);
	v3i Vec3iScale(v3i vector, i32 scalar);
	v3i Vec3iMultiply(v3i left, v3i right);
	bool Vec3iEqual(v3i left, v3i right);
	v4 Vec4Add(v4 left, v4 right);
	v4 Vec4Subtract(v4 left, v4 right);
	v4 Vec4Scale(v4 vector, r32 scalar);
	v4 Vec4Multiply(v4 left, v4 right);
	v4 Vec4Shrink(v4 vector, r32 divisor);
	v4 Vec4Divide(v4 left, v4 right);
	bool Vec4BasicallyEqual(v4 left, v4 right, r32 tolerance = 0.001f);
	v4i Vec4iAdd(v4i left, v4i right);
	v4i Vec4iSubtract(v4i left, v4i right);
	v4i Vec4iScale(v4i vector, i32 scalar);
	v4i Vec4iMultiply(v4i left, v4i right);
	bool Vec4iEqual(v4i left, v4i right);
	r32 Vec2Dot(v2 left, v2 right);
	r32 Vec2Length(v2 vector);
	r32 Vec2LengthSquared(v2 vector);
	v2 Vec2Normalize(v2 vector);
	v2 Vec2PerpRight(v2 vector);
	v2 Vec2PerpLeft(v2 vector);
	r32 Vec2Determinant(v2 left, v2 right);
	r32 Vec2Inner(v2 left, v2 right);
	v2 Vec2Clamp(v2 vector, v2 min, v2 max);
	v2 Vec2Lerp(v2 start, v2 end, r32 amount);
	v2 Vec2Rotate(v2 vector, r32 angle);
	v2 Vec2FromAngle(r32 angle, r32 radius = 1.0f);
	r32 Vec2AngleBetween(v2 vecLeft, v2 vecRight);
	i32 Vec2iDot(v2i left, v2i right);
	r32 Vec2iLength(v2i vector);
	v2 Vec2iNormalize(v2i vector);
	v2i Vec2iPerpRight(v2i vector);
	v2i Vec2iPerpLeft(v2i vector);
	i32 Vec2iDeterminant(v2i left, v2i right);
	v2i Vec2iClamp(v2i vector, v2i min, v2i max);
	r32 Vec3Dot(v3 left, v3 right);
	r32 Vec3Length(v3 vector);
	r32 Vec3LengthSquared(v3 vector);
	v3 Vec3Normalize(v3 vector);
	r32 Vec3Determinant(v3 left, v3 right);
	r32 Vec3Inner(v3 left, v3 right);
	v3 Vec3Clamp(v3 vector, v3 min, v3 max);
	v3 Vec3Lerp(v3 start, v3 end, r32 amount);
	v3 Vec3Cross(v3 left, v3 right);
	r32 Vec3AngleBetween(v3 vecLeft, v3 vecRight);
	v3 Vec3From2Angles(r32 facingDirection, r32 rotationUpDown, r32 radius = 1.0f);
	i32 Vec3iDot(v3i left, v3i right);
	r32 Vec3iLength(v3i vector);
	v3 Vec3iNormalize(v3i vector);
	i32 Vec3iDeterminant(v3i left, v3i right);
	v3i Vec3iClamp(v3i vector, v3i min, v3i max);
	v3i Vec3iCross(v3i left, v3i right);
	r32 Vec4Dot(v4 left, v4 right);
	r32 Vec4Length(v4 vector);
	v4 Vec4Normalize(v4 vector);
	r32 Vec4Determinant(v4 left, v4 right);
	r32 Vec4Inner(v4 left, v4 right);
	v4 Vec4Clamp(v4 vector, v4 min, v4 max);
	v4 Vec4Lerp(v4 start, v4 end, r32 amount);
	i32 Vec4iDot(v4i left, v4i right);
	r32 Vec4iLength(v4i vector);
	v4 Vec4iNormalize(v4i vector);
	i32 Vec4iDeterminant(v4i left, v4i right);
	v4i Vec4iClamp(v4i vector, v4i min, v4i max);
	void Vec2Align(v2* vectorOut, r32 alignmentScale = 1.0f);
	void Vec3Align(v3* vectorOut, r32 alignmentScale = 1.0f);
	void Vec4Align(v4* vectorOut, r32 alignmentScale = 1.0f);
	v2 operator - (v2 vector);
	v2 operator + (v2 left, v2 right);
	v2 operator - (v2 left, v2 right);
	v2 operator * (v2 vector, r32 scalar);
	v2 operator * (r32 scalar, v2 vector);
	v2 operator / (v2 vector, r32 scalar);
	bool operator == (v2 left, v2 right);
	bool operator != (v2 left, v2 right);
	v2i operator - (v2i vector);
	v2i operator + (v2i left, v2i right);
	v2i operator - (v2i left, v2i right);
	v2i operator * (v2i vector, i32 scalar);
	v2i operator * (i32 scalar, v2i vector);
	bool operator == (v2i left, v2i right);
	bool operator != (v2i left, v2i right);
	v3 operator - (v3 vector);
	v3 operator + (v3 left, v3 right);
	v3 operator - (v3 left, v3 right);
	v3 operator * (v3 vector, r32 scalar);
	v3 operator * (r32 scalar, v3 vector);
	v3 operator / (v3 vector, r32 scalar);
	bool operator == (v3 left, v3 right);
	bool operator != (v3 left, v3 right);
	v3i operator - (v3i vector);
	v3i operator + (v3i left, v3i right);
	v3i operator - (v3i left, v3i right);
	v3i operator * (v3i vector, i32 scalar);
	v3i operator * (i32 scalar, v3i vector);
	bool operator == (v3i left, v3i right);
	bool operator != (v3i left, v3i right);
	v4 operator - (v4 vector);
	v4 operator + (v4 left, v4 right);
	v4 operator - (v4 left, v4 right);
	v4 operator * (v4 vector, r32 scalar);
	v4 operator * (r32 scalar, v4 vector);
	v4 operator / (v4 vector, r32 scalar);
	bool operator == (v4 left, v4 right);
	bool operator != (v4 left, v4 right);
	v4i operator - (v4i vector);
	v4i operator + (v4i left, v4i right);
	v4i operator - (v4i left, v4i right);
	v4i operator * (v4i vector, i32 scalar);
	v4i operator * (i32 scalar, v4i vector);
	bool operator == (v4i left, v4i right);
	bool operator != (v4i left, v4i right);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Vector2_t NewVec2(r32 x, r32 y)
{
	Vector2_t result;
	result.x = x;
	result.y = y;
	return result;
}
Vector3_t NewVec3(r32 x, r32 y, r32 z)
{
	Vector3_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}
Vector4_t NewVec4(r32 x, r32 y, r32 z, r32 w)
{
	Vector4_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}

Vector2i_t NewVec2i(i32 x, i32 y)
{
	Vector2i_t result;
	result.x = x;
	result.y = y;
	return result;
}
Vector3i_t NewVec3i(i32 x, i32 y, i32 z)
{
	Vector3i_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}
Vector4i_t NewVec4i(i32 x, i32 y, i32 z, i32 w)
{
	Vector4i_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}

// +--------------------------------------------------------------+
// |                 Simple Conversions and Casts                 |
// +--------------------------------------------------------------+
// +==============================+
// |            ToVec             |
// +==============================+
v2 ToVec2(v2i vector)
{
	v2 result;
	result.x = (r32)vector.x;
	result.y = (r32)vector.y;
	return result;
}
v3 ToVec3(v3i vector)
{
	v3 result;
	result.x = (r32)vector.x;
	result.y = (r32)vector.y;
	result.z = (r32)vector.z;
	return result;
}
v4 ToVec4(v4i vector)
{
	v4 result;
	result.x = (r32)vector.x;
	result.y = (r32)vector.y;
	result.z = (r32)vector.z;
	result.w = (r32)vector.w;
	return result;
}
v4 ToVec4(v3 vector, r32 w)
{
	v4 result;
	result.x = vector.x;
	result.y = vector.y;
	result.z = vector.z;
	result.w = w;
	return result;
}
//ToVec4(Color_t) defined in gy_colors.h

v2 Vec3_xy(v3 vector)
{
	v2 result;
	result.x = vector.x;
	result.y = vector.y;
	return result;
}
v2 Vec3_xz(v3 vector)
{
	v2 result;
	result.x = vector.x;
	result.y = vector.z;
	return result;
}
v2 Vec3_yz(v3 vector)
{
	v2 result;
	result.x = vector.y;
	result.y = vector.z;
	return result;
}

// +==============================+
// |            Floor             |
// +==============================+
v2 Vec2Floor(v2 vector)
{
	v2 result;
	result.x = FloorR32(vector.x);
	result.y = FloorR32(vector.y);
	return result;
}
v2i Vec2Floori(v2 vector)
{
	v2i result;
	result.x = FloorR32i(vector.x);
	result.y = FloorR32i(vector.y);
	return result;
}
v3 Vec3Floor(v3 vector)
{
	v3 result;
	result.x = FloorR32(vector.x);
	result.y = FloorR32(vector.y);
	result.z = FloorR32(vector.z);
	return result;
}
v3i Vec3Floori(v3 vector)
{
	v3i result;
	result.x = FloorR32i(vector.x);
	result.y = FloorR32i(vector.y);
	result.z = FloorR32i(vector.z);
	return result;
}
v4 Vec4Floor(v4 vector)
{
	v4 result;
	result.x = FloorR32(vector.x);
	result.y = FloorR32(vector.y);
	result.z = FloorR32(vector.z);
	result.w = FloorR32(vector.w);
	return result;
}
v4i Vec4Floori(v4 vector)
{
	v4i result;
	result.x = FloorR32i(vector.x);
	result.y = FloorR32i(vector.y);
	result.z = FloorR32i(vector.z);
	result.w = FloorR32i(vector.w);
	return result;
}

// +==============================+
// |             Ceil             |
// +==============================+
v2 Vec2Ceil(v2 vector)
{
	v2 result;
	result.x = CeilR32(vector.x);
	result.y = CeilR32(vector.y);
	return result;
}
v2i Vec2Ceili(v2 vector)
{
	v2i result;
	result.x = CeilR32i(vector.x);
	result.y = CeilR32i(vector.y);
	return result;
}
v3 Vec3Ceil(v3 vector)
{
	v3 result;
	result.x = CeilR32(vector.x);
	result.y = CeilR32(vector.y);
	result.z = CeilR32(vector.z);
	return result;
}
v3i Vec3Ceili(v3 vector)
{
	v3i result;
	result.x = CeilR32i(vector.x);
	result.y = CeilR32i(vector.y);
	result.z = CeilR32i(vector.z);
	return result;
}
v4 Vec4Ceil(v4 vector)
{
	v4 result;
	result.x = CeilR32(vector.x);
	result.y = CeilR32(vector.y);
	result.z = CeilR32(vector.z);
	result.w = CeilR32(vector.w);
	return result;
}
v4i Vec4Ceili(v4 vector)
{
	v4i result;
	result.x = CeilR32i(vector.x);
	result.y = CeilR32i(vector.y);
	result.z = CeilR32i(vector.z);
	result.w = CeilR32i(vector.w);
	return result;
}

// +==============================+
// |            Round             |
// +==============================+
v2 Vec2Round(v2 vector)
{
	v2 result;
	result.x = RoundR32(vector.x);
	result.y = RoundR32(vector.y);
	return result;
}
v2i Vec2Roundi(v2 vector)
{
	v2i result;
	result.x = RoundR32i(vector.x);
	result.y = RoundR32i(vector.y);
	return result;
}
v3 Vec3Round(v3 vector)
{
	v3 result;
	result.x = RoundR32(vector.x);
	result.y = RoundR32(vector.y);
	result.z = RoundR32(vector.z);
	return result;
}
v3i Vec3Roundi(v3 vector)
{
	v3i result;
	result.x = RoundR32i(vector.x);
	result.y = RoundR32i(vector.y);
	result.z = RoundR32i(vector.z);
	return result;
}
v4 Vec4Round(v4 vector)
{
	v4 result;
	result.x = RoundR32(vector.x);
	result.y = RoundR32(vector.y);
	result.z = RoundR32(vector.z);
	result.w = RoundR32(vector.w);
	return result;
}
v4i Vec4Roundi(v4 vector)
{
	v4i result;
	result.x = RoundR32i(vector.x);
	result.y = RoundR32i(vector.y);
	result.z = RoundR32i(vector.z);
	result.w = RoundR32i(vector.w);
	return result;
}

// +==============================+
// |             Max              |
// +==============================+
v2 Vec2Max(v2 left, v2 right)
{
	v2 result;
	result.x = MaxR32(left.x, right.x);
	result.y = MaxR32(left.y, right.y);
	return result;
}
v2i Vec2iMax(v2i left, v2i right)
{
	v2i result;
	result.x = MaxI32(left.x, right.x);
	result.y = MaxI32(left.y, right.y);
	return result;
}
v3 Vec3Max(v3 left, v3 right)
{
	v3 result;
	result.x = MaxR32(left.x, right.x);
	result.y = MaxR32(left.y, right.y);
	result.z = MaxR32(left.z, right.z);
	return result;
}
v3i Vec3iMax(v3i left, v3i right)
{
	v3i result;
	result.x = MaxI32(left.x, right.x);
	result.y = MaxI32(left.y, right.y);
	result.z = MaxI32(left.z, right.z);
	return result;
}
v4 Vec4Max(v4 left, v4 right)
{
	v4 result;
	result.x = MaxR32(left.x, right.x);
	result.y = MaxR32(left.y, right.y);
	result.z = MaxR32(left.z, right.z);
	result.w = MaxR32(left.w, right.w);
	return result;
}
v4i Vec4iMax(v4i left, v4i right)
{
	v4i result;
	result.x = MaxI32(left.x, right.x);
	result.y = MaxI32(left.y, right.y);
	result.z = MaxI32(left.z, right.z);
	result.w = MaxI32(left.w, right.w);
	return result;
}

// +==============================+
// |             Min              |
// +==============================+
v2 Vec2Min(v2 left, v2 right)
{
	v2 result;
	result.x = MinR32(left.x, right.x);
	result.y = MinR32(left.y, right.y);
	return result;
}
v2i Vec2iMin(v2i left, v2i right)
{
	v2i result;
	result.x = MinI32(left.x, right.x);
	result.y = MinI32(left.y, right.y);
	return result;
}
v3 Vec3Min(v3 left, v3 right)
{
	v3 result;
	result.x = MinR32(left.x, right.x);
	result.y = MinR32(left.y, right.y);
	result.z = MinR32(left.z, right.z);
	return result;
}
v3i Vec3iMin(v3i left, v3i right)
{
	v3i result;
	result.x = MinI32(left.x, right.x);
	result.y = MinI32(left.y, right.y);
	result.z = MinI32(left.z, right.z);
	return result;
}
v4 Vec4Min(v4 left, v4 right)
{
	v4 result;
	result.x = MinR32(left.x, right.x);
	result.y = MinR32(left.y, right.y);
	result.z = MinR32(left.z, right.z);
	result.w = MinR32(left.w, right.w);
	return result;
}
v4i Vec4iMin(v4i left, v4i right)
{
	v4i result;
	result.x = MinI32(left.x, right.x);
	result.y = MinI32(left.y, right.y);
	result.z = MinI32(left.z, right.z);
	result.w = MinI32(left.w, right.w);
	return result;
}

// +==============================+
// |             Abs              |
// +==============================+
v2i Vec2iAbs(v2i vector)
{
	return NewVec2i(AbsI32(vector.x), AbsI32(vector.y));
}
v2 Vec2Abs(v2 vector)
{
	return NewVec2(AbsR32(vector.x), AbsR32(vector.y));
}
v3i Vec3iAbs(v3i vector)
{
	return NewVec3i(AbsI32(vector.x), AbsI32(vector.y), AbsI32(vector.z));
}
v3 Vec3Abs(v3 vector)
{
	return NewVec3(AbsR32(vector.x), AbsR32(vector.y), AbsR32(vector.z));
}
v4i Vec4iAbs(v4i vector)
{
	return NewVec4i(AbsI32(vector.x), AbsI32(vector.y), AbsI32(vector.z), AbsI32(vector.w));
}
v4 Vec4Abs(v4 vector)
{
	return NewVec4(AbsR32(vector.x), AbsR32(vector.y), AbsR32(vector.z), AbsR32(vector.w));
}

// +==============================+
// |             Fill             |
// +==============================+
v2 Vec2Fill(r32 value)
{
	v2 result;
	result.x = value;
	result.y = value;
	return result;
}
v2i Vec2iFill(i32 value)
{
	v2i result;
	result.x = value;
	result.y = value;
	return result;
}
v3 Vec3Fill(r32 value)
{
	v3 result;
	result.x = value;
	result.y = value;
	result.z = value;
	return result;
}
v3i Vec3iFill(i32 value)
{
	v3i result;
	result.x = value;
	result.y = value;
	result.z = value;
	return result;
}
v4 Vec4Fill(r32 value)
{
	v4 result;
	result.x = value;
	result.y = value;
	result.z = value;
	result.w = value;
	return result;
}
v4i Vec4iFill(i32 value)
{
	v4i result;
	result.x = value;
	result.y = value;
	result.z = value;
	result.w = value;
	return result;
}

// +==============================+
// |          VecFromVec          |
// +==============================+
v3 Vec3FromVec2(v2 vector, r32 z)
{
	v3 result;
	result.x = vector.x;
	result.y = vector.y;
	result.z = z;
	return result;
}
v3i Vec3iFromVec2i(v2i vector, i32 z)
{
	v3i result;
	result.x = vector.x;
	result.y = vector.y;
	result.z = z;
	return result;
}
v4 Vec4FromVec3(v3 vector, r32 w)
{
	v4 result;
	result.x = vector.x;
	result.y = vector.y;
	result.z = vector.z;
	result.w = w;
	return result;
}
v4i Vec4iFromVec3i(v3i vector, i32 w)
{
	v4i result;
	result.x = vector.x;
	result.y = vector.y;
	result.z = vector.z;
	result.w = w;
	return result;
}

// +--------------------------------------------------------------+
// |                Operator Overload Equivalents                 |
// +--------------------------------------------------------------+
// +==============================+
// |             Vec2             |
// +==============================+
v2 Vec2Add(v2 left, v2 right)
{
	v2 result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	return result;
}
v2 Vec2Subtract(v2 left, v2 right)
{
	v2 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}
v2 Vec2Scale(v2 vector, r32 scalar)
{
	v2 result;
	result.x = vector.x * scalar;
	result.y = vector.y * scalar;
	return result;
}
v2 Vec2Multiply(v2 left, v2 right)
{
	v2 result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	return result;
}
v2 Vec2Shrink(v2 vector, r32 divisor)
{
	v2 result;
	result.x = vector.x / divisor;
	result.y = vector.y / divisor;
	return result;
}
v2 Vec2Divide(v2 left, v2 right)
{
	v2 result;
	result.x = left.x / right.x;
	result.y = left.y / right.y;
	return result;
}
bool Vec2BasicallyEqual(v2 left, v2 right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x, right.x, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.y, right.y, tolerance)) { return false; }
	return true;
}

// +==============================+
// |            Vec2i             |
// +==============================+
v2i Vec2iAdd(v2i left, v2i right)
{
	v2i result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	return result;
}
v2i Vec2iSubtract(v2i left, v2i right)
{
	v2i result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}
v2i Vec2iScale(v2i vector, i32 scalar)
{
	v2i result;
	result.x = vector.x * scalar;
	result.y = vector.y * scalar;
	return result;
}
v2i Vec2iMultiply(v2i left, v2i right)
{
	v2i result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	return result;
}
bool Vec2iEqual(v2i left, v2i right)
{
	if (left.x != right.x) { return false; }
	if (left.y != right.y) { return false; }
	return true;
}

// +==============================+
// |             Vec3             |
// +==============================+
v3 Vec3Add(v3 left, v3 right)
{
	v3 result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	return result;
}
v3 Vec3Subtract(v3 left, v3 right)
{
	v3 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	return result;
}
v3 Vec3Scale(v3 vector, r32 scalar)
{
	v3 result;
	result.x = vector.x * scalar;
	result.y = vector.y * scalar;
	result.z = vector.z * scalar;
	return result;
}
v3 Vec3Multiply(v3 left, v3 right)
{
	v3 result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	result.z = left.z * right.z;
	return result;
}
v3 Vec3Shrink(v3 vector, r32 divisor)
{
	v3 result;
	result.x = vector.x / divisor;
	result.y = vector.y / divisor;
	result.z = vector.z / divisor;
	return result;
}
v3 Vec3Divide(v3 left, v3 right)
{
	v3 result;
	result.x = left.x / right.x;
	result.y = left.y / right.y;
	result.z = left.z / right.z;
	return result;
}
bool Vec3BasicallyEqual(v3 left, v3 right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x, right.x, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.y, right.y, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.z, right.z, tolerance)) { return false; }
	return true;
}

// +==============================+
// |            Vec3i             |
// +==============================+
v3i Vec3iAdd(v3i left, v3i right)
{
	v3i result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	return result;
}
v3i Vec3iSubtract(v3i left, v3i right)
{
	v3i result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	return result;
}
v3i Vec3iScale(v3i vector, i32 scalar)
{
	v3i result;
	result.x = vector.x * scalar;
	result.y = vector.y * scalar;
	result.z = vector.z * scalar;
	return result;
}
v3i Vec3iMultiply(v3i left, v3i right)
{
	v3i result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	result.z = left.z * right.z;
	return result;
}
bool Vec3iEqual(v3i left, v3i right)
{
	if (left.x != right.x) { return false; }
	if (left.y != right.y) { return false; }
	if (left.z != right.z) { return false; }
	return true;
}

// +==============================+
// |             Vec4             |
// +==============================+
v4 Vec4Add(v4 left, v4 right)
{
	v4 result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	result.w = left.w + right.w;
	return result;
}
v4 Vec4Subtract(v4 left, v4 right)
{
	v4 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	result.w = left.w - right.w;
	return result;
}
v4 Vec4Scale(v4 vector, r32 scalar)
{
	v4 result;
	result.x = vector.x * scalar;
	result.y = vector.y * scalar;
	result.z = vector.z * scalar;
	result.w = vector.w * scalar;
	return result;
}
v4 Vec4Multiply(v4 left, v4 right)
{
	v4 result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	result.z = left.z * right.z;
	result.w = left.w * right.w;
	return result;
}
v4 Vec4Shrink(v4 vector, r32 divisor)
{
	v4 result;
	result.x = vector.x / divisor;
	result.y = vector.y / divisor;
	result.z = vector.z / divisor;
	result.w = vector.w / divisor;
	return result;
}
v4 Vec4Divide(v4 left, v4 right)
{
	v4 result;
	result.x = left.x / right.x;
	result.y = left.y / right.y;
	result.z = left.z / right.z;
	result.w = left.w / right.w;
	return result;
}
bool Vec4BasicallyEqual(v4 left, v4 right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.x, right.x, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.y, right.y, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.z, right.z, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.w, right.w, tolerance)) { return false; }
	return true;
}

// +==============================+
// |            Vec4i             |
// +==============================+
v4i Vec4iAdd(v4i left, v4i right)
{
	v4i result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	result.w = left.w + right.w;
	return result;
}
v4i Vec4iSubtract(v4i left, v4i right)
{
	v4i result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	result.w = left.w - right.w;
	return result;
}
v4i Vec4iScale(v4i vector, i32 scalar)
{
	v4i result;
	result.x = vector.x * scalar;
	result.y = vector.y * scalar;
	result.z = vector.z * scalar;
	result.w = vector.w * scalar;
	return result;
}
v4i Vec4iMultiply(v4i left, v4i right)
{
	v4i result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	result.z = left.z * right.z;
	result.w = left.w * right.w;
	return result;
}
bool Vec4iEqual(v4i left, v4i right)
{
	if (left.x != right.x) { return false; }
	if (left.y != right.y) { return false; }
	if (left.z != right.z) { return false; }
	if (left.w != right.w) { return false; }
	return true;
}

// +--------------------------------------------------------------+
// |                      Other Vector Math                       |
// +--------------------------------------------------------------+
r32 Vec2Dot(v2 left, v2 right)
{
	return left.x*right.x + left.y*right.y;
}
r32 Vec2Length(v2 vector)
{
	return SqrtR32(Square(vector.x) + Square(vector.y));
}
r32 Vec2LengthSquared(v2 vector)
{
	return Square(vector.x) + Square(vector.y);
}
v2 Vec2Normalize(v2 vector)
{
	return Vec2Shrink(vector, Vec2Length(vector));
}
v2 Vec2PerpRight(v2 vector)
{
	v2 result;
	result.x = -vector.y;
	result.y = vector.x;
	return result;
}
v2 Vec2PerpLeft(v2 vector)
{
	v2 result;
	result.x = vector.y;
	result.y = -vector.x;
	return result;
}
r32 Vec2Determinant(v2 left, v2 right)
{
	return left.x*right.y + left.y*right.x; //TODO: Is this right?? Seems like maybe it should be lx*ry - ly*rx
}
r32 Vec2Inner(v2 left, v2 right)
{
	r32 dot = Vec2Dot(left, right);
	r32 det = Vec2Determinant(left, right);
	r32 result = AtanR32(det, dot);
	return result;
}
v2 Vec2Clamp(v2 vector, v2 min, v2 max)
{
	v2 result;
	result.x = ClampR32(vector.x, min.x, max.x);
	result.y = ClampR32(vector.y, min.y, max.y);
	return result;
}
v2 Vec2Lerp(v2 start, v2 end, r32 amount)
{
	v2 result;
	result.x = LerpR32(start.x, end.x, amount);
	result.y = LerpR32(start.y, end.y, amount);
	return result;
}
v2 Vec2Rotate(v2 vector, r32 angle)
{
	v2 result;
	result.x = CosR32(angle) * vector.x - SinR32(angle) * vector.y;
	result.y = SinR32(angle) * vector.x + CosR32(angle) * vector.y;
	return result;
}
v2 Vec2FromAngle(r32 angle, r32 radius = 1.0f)
{
	v2 result;
	result.x = CosR32(angle) * radius;
	result.y = SinR32(angle) * radius;
	return result;
}
r32 Vec2AngleBetween(v2 vecLeft, v2 vecRight)
{
	r32 dotProduct = Vec2Dot(vecLeft, vecRight);
	if (dotProduct == -1) { return Pi32; }
	return SignOfR32(vecLeft.x * vecRight.y - vecLeft.y * vecRight.x) * AcosR32(dotProduct / (Vec2Length(vecLeft) * Vec2Length(vecRight)));
}

i32 Vec2iDot(v2i left, v2i right)
{
	return left.x*right.x + left.y*right.y;
}
r32 Vec2iLength(v2i vector)
{
	return SqrtR32((r32)Square(vector.x) + (r32)Square(vector.y));
}
v2 Vec2iNormalize(v2i vector)
{
	return Vec2Shrink(ToVec2(vector), Vec2iLength(vector));
}
v2i Vec2iPerpRight(v2i vector)
{
	v2i result;
	result.x = -vector.y;
	result.y = vector.x;
	return result;
}
v2i Vec2iPerpLeft(v2i vector)
{
	v2i result;
	result.x = vector.y;
	result.y = -vector.x;
	return result;
}
i32 Vec2iDeterminant(v2i left, v2i right)
{
	return left.x*right.y + left.y*right.x; //TODO: Is this right?? Seems like maybe it should be lx*ry - ly*rx
}
v2i Vec2iClamp(v2i vector, v2i min, v2i max)
{
	v2i result;
	result.x = ClampI32(vector.x, min.x, max.x);
	result.y = ClampI32(vector.y, min.y, max.y);
	return result;
}

r32 Vec3Dot(v3 left, v3 right)
{
	return left.x*right.x + left.y*right.y + left.z*right.z;
}
r32 Vec3Length(v3 vector)
{
	return SqrtR32(Square(vector.x) + Square(vector.y) + Square(vector.z));
}
r32 Vec3LengthSquared(v3 vector)
{
	return (Square(vector.x) + Square(vector.y) + Square(vector.z));
}
v3 Vec3Normalize(v3 vector)
{
	return Vec3Shrink(vector, Vec3Length(vector));
}
r32 Vec3Determinant(v3 left, v3 right)
{
	UNUSED(left);
	UNUSED(right);
	//TODO: Implement me
	return 0;
}
r32 Vec3Inner(v3 left, v3 right)
{
	r32 dot = Vec3Dot(left, right);
	r32 det = Vec3Determinant(left, right);
	r32 result = AtanR32(det, dot);
	return result;
}
v3 Vec3Clamp(v3 vector, v3 min, v3 max)
{
	v3 result;
	result.x = ClampR32(vector.x, min.x, max.x);
	result.y = ClampR32(vector.y, min.y, max.y);
	result.z = ClampR32(vector.z, min.z, max.z);
	return result;
}
v3 Vec3Lerp(v3 start, v3 end, r32 amount)
{
	v3 result;
	result.x = LerpR32(start.x, end.x, amount);
	result.y = LerpR32(start.y, end.y, amount);
	result.z = LerpR32(start.z, end.z, amount);
	return result;
}
// v3 Vec3RotateX(v3 vector, r32 angle)
// {
// 	v3 result;
// 	//TODO: Implement me!
// 	return result;
// }
// v3 Vec3RotateY(v3 vector, r32 angle)
// {
// 	v3 result;
// 	//TODO: Implement me!
// 	return result;
// }
// v3 Vec3RotateZ(v3 vector, r32 angle)
// {
// 	v3 result;
// 	//TODO: Implement me!
// 	return result;
// }
v3 Vec3Cross(v3 left, v3 right)
{
	v3 result;
	result.x = left.y*right.z - left.z*right.y;
	result.y = left.z*right.x - left.x*right.z;
	result.z = left.x*right.y - left.y*right.x;
	return result;
}
r32 Vec3AngleBetween(v3 vecLeft, v3 vecRight)
{
	return AcosR32(Vec3Dot(vecLeft, vecRight) / (Vec3Length(vecLeft) * Vec3Length(vecRight)));
}
//This Assumes +y is up and is phrased in terms of a first person camera (facingDirection y rotation, and rotationUpDown is z rotation)
v3 Vec3From2Angles(r32 facingDirection, r32 rotationUpDown, r32 radius = 1.0f)
{
	v3 result;
	r32 circleRadius = CosR32(rotationUpDown) * radius;
	result.x = CosR32(facingDirection) * circleRadius;
	result.z = SinR32(facingDirection) * circleRadius;
	result.y = SinR32(rotationUpDown);
	return result;
}

i32 Vec3iDot(v3i left, v3i right)
{
	return left.x*right.x + left.y*right.y + left.z*right.z;
}
r32 Vec3iLength(v3i vector)
{
	return SqrtR32((r32)Square(vector.x) + (r32)Square(vector.y) + (r32)Square(vector.z));
}
v3 Vec3iNormalize(v3i vector)
{
	return Vec3Shrink(ToVec3(vector), Vec3iLength(vector));
}
i32 Vec3iDeterminant(v3i left, v3i right)
{
	UNUSED(left);
	UNUSED(right);
	//TODO: Implement me
	return 0;
}
v3i Vec3iClamp(v3i vector, v3i min, v3i max)
{
	v3i result;
	result.x = ClampI32(vector.x, min.x, max.x);
	result.y = ClampI32(vector.y, min.y, max.y);
	result.z = ClampI32(vector.z, min.z, max.z);
	return result;
}
v3i Vec3iCross(v3i left, v3i right)
{
	v3i result;
	result.x = left.y*right.z - left.z*right.y;
	result.y = left.z*right.x - left.x*right.z;
	result.z = left.x*right.y - left.y*right.x;
	return result;
}

r32 Vec4Dot(v4 left, v4 right)
{
	return left.x*right.x + left.y*right.y + left.z*right.z + left.w*right.w;
}
r32 Vec4Length(v4 vector)
{
	return SqrtR32(Square(vector.x) + Square(vector.y) + Square(vector.z) + Square(vector.w));
}
v4 Vec4Normalize(v4 vector)
{
	return Vec4Shrink(vector, Vec4Length(vector));
}
r32 Vec4Determinant(v4 left, v4 right)
{
	UNUSED(left);
	UNUSED(right);
	//TODO: Implement me
	return 0;
}
r32 Vec4Inner(v4 left, v4 right)
{
	r32 dot = Vec4Dot(left, right);
	r32 det = Vec4Determinant(left, right);
	r32 result = AtanR32(det, dot);
	return result;
}
v4 Vec4Clamp(v4 vector, v4 min, v4 max)
{
	v4 result;
	result.x = ClampR32(vector.x, min.x, max.x);
	result.y = ClampR32(vector.y, min.y, max.y);
	result.z = ClampR32(vector.z, min.z, max.z);
	result.w = ClampR32(vector.w, min.w, max.w);
	return result;
}
v4 Vec4Lerp(v4 start, v4 end, r32 amount)
{
	v4 result;
	result.x = LerpR32(start.x, end.x, amount);
	result.y = LerpR32(start.y, end.y, amount);
	result.z = LerpR32(start.z, end.z, amount);
	result.w = LerpR32(start.w, end.w, amount);
	return result;
}
// v4 Vec4RotateX(v4 vector, r32 angle)
// {
// 	v4 result;
// 	//TODO: Implement me!
// 	return result;
// }
// v4 Vec4RotateY(v4 vector, r32 angle)
// {
// 	v4 result;
// 	//TODO: Implement me!
// 	return result;
// }
// v4 Vec4RotateZ(v4 vector, r32 angle)
// {
// 	v4 result;
// 	//TODO: Implement me!
// 	return result;
// }
// v4 Vec4RotateW(v4 vector, r32 angle)
// {
// 	v4 result;
// 	//TODO: Implement me!
// 	return result;
// }

i32 Vec4iDot(v4i left, v4i right)
{
	return left.x*right.x + left.y*right.y + left.z*right.z + left.w*right.w;
}
r32 Vec4iLength(v4i vector)
{
	return SqrtR32((r32)Square(vector.x) + (r32)Square(vector.y) + (r32)Square(vector.z) + (r32)Square(vector.w));
}
v4 Vec4iNormalize(v4i vector)
{
	return Vec4Shrink(ToVec4(vector), Vec4iLength(vector));
}
i32 Vec4iDeterminant(v4i left, v4i right)
{
	UNUSED(left);
	UNUSED(right);
	//TODO: Implement me
	return 0;
}
v4i Vec4iClamp(v4i vector, v4i min, v4i max)
{
	v4i result;
	result.x = ClampI32(vector.x, min.x, max.x);
	result.y = ClampI32(vector.y, min.y, max.y);
	result.z = ClampI32(vector.z, min.z, max.z);
	result.w = ClampI32(vector.w, min.w, max.w);
	return result;
}

// +--------------------------------------------------------------+
// |             Quickhand Functions for Layout Code              |
// +--------------------------------------------------------------+
void Vec2Align(v2* vectorOut, r32 alignmentScale = 1.0f)
{
	DebugAssert_(vectorOut != nullptr);
	if (alignmentScale > 0)
	{
		vectorOut->x = RoundR32(vectorOut->x * alignmentScale) / alignmentScale;
		vectorOut->y = RoundR32(vectorOut->y * alignmentScale) / alignmentScale;
	}
}
void Vec3Align(v3* vectorOut, r32 alignmentScale = 1.0f)
{
	DebugAssert_(vectorOut != nullptr);
	if (alignmentScale > 0)
	{
		vectorOut->x = RoundR32(vectorOut->x * alignmentScale) / alignmentScale;
		vectorOut->y = RoundR32(vectorOut->y * alignmentScale) / alignmentScale;
		vectorOut->z = RoundR32(vectorOut->z * alignmentScale) / alignmentScale;
	}
}
void Vec4Align(v4* vectorOut, r32 alignmentScale = 1.0f)
{
	DebugAssert_(vectorOut != nullptr);
	if (alignmentScale > 0)
	{
		vectorOut->x = RoundR32(vectorOut->x * alignmentScale) / alignmentScale;
		vectorOut->y = RoundR32(vectorOut->y * alignmentScale) / alignmentScale;
		vectorOut->z = RoundR32(vectorOut->z * alignmentScale) / alignmentScale;
		vectorOut->w = RoundR32(vectorOut->w * alignmentScale) / alignmentScale;
	}
}

// +--------------------------------------------------------------+
// |                      Operator Overloads                      |
// +--------------------------------------------------------------+
v2 operator - (v2 vector)               { return NewVec2(-vector.x, -vector.y); }
v2 operator + (v2 left, v2 right)       { return Vec2Add(left, right); }
v2 operator - (v2 left, v2 right)       { return Vec2Subtract(left, right); }
v2 operator * (v2 vector, r32 scalar)   { return Vec2Scale(vector, scalar); }
v2 operator * (r32 scalar, v2 vector)   { return Vec2Scale(vector, scalar); }
v2 operator / (v2 vector, r32 scalar)   { return Vec2Shrink(vector, scalar); }
bool operator == (v2 left, v2 right)    { return (left.x == right.x && left.y == right.y); }
bool operator != (v2 left, v2 right)    { return (left.x != right.x || left.y != right.y); }

v2i operator - (v2i vector)             { return NewVec2i(-vector.x, -vector.y); }
v2i operator + (v2i left, v2i right)    { return Vec2iAdd(left, right); }
v2i operator - (v2i left, v2i right)    { return Vec2iSubtract(left, right); }
v2i operator * (v2i vector, i32 scalar) { return Vec2iScale(vector, scalar); }
v2i operator * (i32 scalar, v2i vector) { return Vec2iScale(vector, scalar); }
bool operator == (v2i left, v2i right)  { return (left.x == right.x && left.y == right.y); }
bool operator != (v2i left, v2i right)  { return (left.x != right.x || left.y != right.y); }

v3 operator - (v3 vector)               { return NewVec3(-vector.x, -vector.y, -vector.z); }
v3 operator + (v3 left, v3 right)       { return Vec3Add(left, right); }
v3 operator - (v3 left, v3 right)       { return Vec3Subtract(left, right); }
v3 operator * (v3 vector, r32 scalar)   { return Vec3Scale(vector, scalar); }
v3 operator * (r32 scalar, v3 vector)   { return Vec3Scale(vector, scalar); }
v3 operator / (v3 vector, r32 scalar)   { return Vec3Shrink(vector, scalar); }
bool operator == (v3 left, v3 right)    { return (left.x == right.x && left.y == right.y && left.z == right.z); }
bool operator != (v3 left, v3 right)    { return (left.x != right.x || left.y != right.y || left.z != right.z); }

v3i operator - (v3i vector)             { return NewVec3i(-vector.x, -vector.y, -vector.z); }
v3i operator + (v3i left, v3i right)    { return Vec3iAdd(left, right); }
v3i operator - (v3i left, v3i right)    { return Vec3iSubtract(left, right); }
v3i operator * (v3i vector, i32 scalar) { return Vec3iScale(vector, scalar); }
v3i operator * (i32 scalar, v3i vector) { return Vec3iScale(vector, scalar); }
bool operator == (v3i left, v3i right)  { return (left.x == right.x && left.y == right.y && left.z == right.z); }
bool operator != (v3i left, v3i right)  { return (left.x != right.x || left.y != right.y || left.z != right.z); }

v4 operator - (v4 vector)               { return NewVec4(-vector.x, -vector.y, -vector.z, -vector.w); }
v4 operator + (v4 left, v4 right)       { return Vec4Add(left, right); }
v4 operator - (v4 left, v4 right)       { return Vec4Subtract(left, right); }
v4 operator * (v4 vector, r32 scalar)   { return Vec4Scale(vector, scalar); }
v4 operator * (r32 scalar, v4 vector)   { return Vec4Scale(vector, scalar); }
v4 operator / (v4 vector, r32 scalar)   { return Vec4Shrink(vector, scalar); }
bool operator == (v4 left, v4 right)    { return (left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w); }
bool operator != (v4 left, v4 right)    { return (left.x != right.x || left.y != right.y || left.z != right.z || left.w != right.w); }

v4i operator - (v4i vector)             { return NewVec4i(-vector.x, -vector.y, -vector.z, -vector.w); }
v4i operator + (v4i left, v4i right)    { return Vec4iAdd(left, right); }
v4i operator - (v4i left, v4i right)    { return Vec4iSubtract(left, right); }
v4i operator * (v4i vector, i32 scalar) { return Vec4iScale(vector, scalar); }
v4i operator * (i32 scalar, v4i vector) { return Vec4iScale(vector, scalar); }
bool operator == (v4i left, v4i right)  { return (left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w); }
bool operator != (v4i left, v4i right)  { return (left.x != right.x || left.y != right.y || left.z != right.z || left.w != right.w); }

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_VECTORS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
Vec2_Zero
Vec2_One
Vec2_Half
Vec2_Left
Vec2_Right
Vec2_Up
Vec2_Down
Vec2i_Zero
Vec2i_One
Vec2i_Left
Vec2i_Right
Vec2i_Up
Vec2i_Down
Vec3_Zero
Vec3_One
Vec3_Half
Vec3_Left
Vec3_Right
Vec3_Bottom
Vec3_Top
Vec3_Back
Vec3_Front
Vec3_Down
Vec3_Up
Vec3_Backward
Vec3_Forward
Vec3i_Zero
Vec3i_One
Vec3i_Left
Vec3i_Right
Vec3i_Bottom
Vec3i_Top
Vec3i_Back
Vec3i_Front
Vec3i_Down
Vec3i_Up
Vec3i_Backward
Vec3i_Forward
Vec4_Zero
Vec4_One
Vec4_Half
Vec4_Left
Vec4_Right
Vec4_Bottom
Vec4_Top
Vec4_Back
Vec4_Front
Vec4_Down
Vec4_Up
Vec4_Backward
Vec4_Forward
Vec4_Prevward
Vec4_Nextward
Vec4i_Zero
Vec4i_One
Vec4i_Left
Vec4i_Right
Vec4i_Bottom
Vec4i_Top
Vec4i_Back
Vec4i_Front
Vec4i_Down
Vec4i_Up
Vec4i_Backward
Vec4i_Forward
Vec4i_Prevward
Vec4i_Nextward
@Types
Vector2_t
Vector3_t
Vector4_t
Vector2i_t
Vector3i_t
Vector4i_t
v2
v3
v4
v2i
v3i
v4i
@Functions
Vector2_t NewVec2(r32 x, r32 y)
Vector3_t NewVec3(r32 x, r32 y, r32 z)
Vector4_t NewVec4(r32 x, r32 y, r32 z, r32 w)
Vector2i_t NewVec2i(i32 x, i32 y)
Vector3i_t NewVec3i(i32 x, i32 y, i32 z)
Vector4i_t NewVec4i(i32 x, i32 y, i32 z, i32 w)
v2 ToVec2(v2i vector)
v3 ToVec3(v3i vector)
v4 ToVec4(v4i vector)
v2 Vec2Floor(v2 vector)
v2i Vec2Floori(v2 vector)
v3 Vec3Floor(v3 vector)
v3i Vec3Floori(v3 vector)
v4 Vec4Floor(v4 vector)
v4i Vec4Floori(v4 vector)
v2 Vec2Ceil(v2 vector)
v2i Vec2Ceili(v2 vector)
v3 Vec3Ceil(v3 vector)
v3i Vec3Ceili(v3 vector)
v4 Vec4Ceil(v4 vector)
v4i Vec4Ceili(v4 vector)
v2 Vec2Round(v2 vector)
v2i Vec2Roundi(v2 vector)
v3 Vec3Round(v3 vector)
v3i Vec3Roundi(v3 vector)
v4 Vec4Round(v4 vector)
v4i Vec4Roundi(v4 vector)
v2 Vec2Max(v2 left, v2 right)
v2i Vec2iMax(v2i left, v2i right)
v3 Vec3Max(v3 left, v3 right)
v3i Vec3iMax(v3i left, v3i right)
v4 Vec4Max(v4 left, v4 right)
v4i Vec4iMax(v4i left, v4i right)
v2 Vec2Min(v2 left, v2 right)
v2i Vec2iMin(v2i left, v2i right)
v3 Vec3Min(v3 left, v3 right)
v3i Vec3iMin(v3i left, v3i right)
v4 Vec4Min(v4 left, v4 right)
v4i Vec4iMin(v4i left, v4i right)
v2i Vec2iAbs(v2i vector)
v2 Vec2Abs(v2 vector)
v3i Vec3iAbs(v3i vector)
v3 Vec3Abs(v3 vector)
v4i Vec4iAbs(v4i vector)
v4 Vec4Abs(v4 vector)
v2 Vec2Fill(r32 value)
v2i Vec2iFill(i32 value)
v3 Vec3Fill(r32 value)
v3i Vec3iFill(i32 value)
v4 Vec4Fill(r32 value)
v4i Vec4iFill(i32 value)
v3 Vec3FromVec2(v2 vector, r32 z)
v3i Vec3iFromVec2i(v2i vector, i32 z)
v4 Vec4FromVec3(v3 vector, r32 w)
v4i Vec4iFromVec3i(v3i vector, i32 w)
v2 Vec2Add(v2 left, v2 right)
v2 Vec2Subtract(v2 left, v2 right)
v2 Vec2Scale(v2 vector, r32 scalar)
v2 Vec2Multiply(v2 left, v2 right)
v2 Vec2Shrink(v2 vector, r32 divisor)
v2 Vec2Divide(v2 left, v2 right)
bool Vec2BasicallyEqual(v2 left, v2 right, r32 tolerance = 0.001f)
v2i Vec2iAdd(v2i left, v2i right)
v2i Vec2iSubtract(v2i left, v2i right)
v2i Vec2iScale(v2i vector, i32 scalar)
v2i Vec2iMultiply(v2i left, v2i right)
bool Vec2iEqual(v2i left, v2i right)
v3 Vec3Add(v3 left, v3 right)
v3 Vec3Subtract(v3 left, v3 right)
v3 Vec3Scale(v3 vector, r32 scalar)
v3 Vec3Multiply(v3 left, v3 right)
v3 Vec3Shrink(v3 vector, r32 divisor)
v3 Vec3Divide(v3 left, v3 right)
bool Vec3BasicallyEqual(v3 left, v3 right, r32 tolerance = 0.001f)
v3i Vec3iAdd(v3i left, v3i right)
v3i Vec3iSubtract(v3i left, v3i right)
v3i Vec3iScale(v3i vector, i32 scalar)
v3i Vec3iMultiply(v3i left, v3i right)
bool Vec3iEqual(v3i left, v3i right)
v4 Vec4Add(v4 left, v4 right)
v4 Vec4Subtract(v4 left, v4 right)
v4 Vec4Scale(v4 vector, r32 scalar)
v4 Vec4Multiply(v4 left, v4 right)
v4 Vec4Shrink(v4 vector, r32 divisor)
v4 Vec4Divide(v4 left, v4 right)
bool Vec4BasicallyEqual(v4 left, v4 right, r32 tolerance = 0.001f)
v4i Vec4iAdd(v4i left, v4i right)
v4i Vec4iSubtract(v4i left, v4i right)
v4i Vec4iScale(v4i vector, i32 scalar)
v4i Vec4iMultiply(v4i left, v4i right)
bool Vec4iEqual(v4i left, v4i right)
r32 Vec2Dot(v2 left, v2 right)
r32 Vec2Length(v2 vector)
r32 Vec2LengthSquared(v2 vector)
v2 Vec2Normalize(v2 vector)
v2 Vec2PerpRight(v2 vector)
v2 Vec2PerpLeft(v2 vector)
r32 Vec2Determinant(v2 left, v2 right)
r32 Vec2Inner(v2 left, v2 right)
v2 Vec2Clamp(v2 vector, v2 min, v2 max)
v2 Vec2Lerp(v2 start, v2 end, r32 amount)
v2 Vec2Rotate(v2 vector, r32 angle)
v2 Vec2FromAngle(r32 angle, r32 radius = 1.0f)
r32 Vec2AngleBetween(v2 vecLeft, v2 vecRight)
i32 Vec2iDot(v2i left, v2i right)
r32 Vec2iLength(v2i vector)
v2 Vec2iNormalize(v2i vector)
v2i Vec2iPerpRight(v2i vector)
v2i Vec2iPerpLeft(v2i vector)
i32 Vec2iDeterminant(v2i left, v2i right)
v2i Vec2iClamp(v2i vector, v2i min, v2i max)
r32 Vec3Dot(v3 left, v3 right)
r32 Vec3Length(v3 vector)
r32 Vec3LengthSquared(v3 vector)
v3 Vec3Normalize(v3 vector)
r32 Vec3Determinant(v3 left, v3 right)
r32 Vec3Inner(v3 left, v3 right)
v3 Vec3Clamp(v3 vector, v3 min, v3 max)
v3 Vec3Lerp(v3 start, v3 end, r32 amount)
v3 Vec3Cross(v3 left, v3 right)
r32 Vec3AngleBetween(v3 vecLeft, v3 vecRight)
v3 Vec3From2Angles(r32 facingDirection, r32 rotationUpDown, r32 radius = 1.0f)
i32 Vec3iDot(v3i left, v3i right)
r32 Vec3iLength(v3i vector)
v3 Vec3iNormalize(v3i vector)
i32 Vec3iDeterminant(v3i left, v3i right)
v3i Vec3iClamp(v3i vector, v3i min, v3i max)
v3i Vec3iCross(v3i left, v3i right)
r32 Vec4Dot(v4 left, v4 right)
r32 Vec4Length(v4 vector)
v4 Vec4Normalize(v4 vector)
r32 Vec4Determinant(v4 left, v4 right)
r32 Vec4Inner(v4 left, v4 right)
v4 Vec4Clamp(v4 vector, v4 min, v4 max)
v4 Vec4Lerp(v4 start, v4 end, r32 amount)
i32 Vec4iDot(v4i left, v4i right)
r32 Vec4iLength(v4i vector)
v4 Vec4iNormalize(v4i vector)
i32 Vec4iDeterminant(v4i left, v4i right)
v4i Vec4iClamp(v4i vector, v4i min, v4i max)
void Vec2Align(v2* vectorOut, r32 alignmentScale = 1.0f)
void Vec3Align(v3* vectorOut, r32 alignmentScale = 1.0f)
void Vec4Align(v4* vectorOut, r32 alignmentScale = 1.0f)
*/
