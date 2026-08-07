/*
File:   gy_quaternions.h
Author: Taylor Robbins
Date:   11\02\2021
Description:
	** Holds the Quaternion_t (quat) structure that specifies a rotation in 3D space
*/

#ifndef _GY_QUATERNIONS_H
#define _GY_QUATERNIONS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"
#include "gy_matrices.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
union Quaternion_t
{
	r32 values[4];
	v4 vec4;
	struct
	{
		v3 axis;
		r32 angle; //this is actually cos(angle)/2
	};
	struct
	{
		r32 x, y, z, w;
	};
};

typedef Quaternion_t quat;

enum EulerOrder_t
{
	EulerOrder_XYZ = 0,
	EulerOrder_ZYX,
	EulerOrder_XZY,
	EulerOrder_ZXY,
	EulerOrder_YXZ,
	EulerOrder_YZX,
	EulerOrder_NumOrders,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetEulerOrderStr(EulerOrder_t order);
#else
const char* GetEulerOrderStr(EulerOrder_t order)
{
	switch (order)
	{
		case EulerOrder_XYZ: return "XYZ";
		case EulerOrder_ZYX: return "ZYX";
		case EulerOrder_XZY: return "XZY";
		case EulerOrder_ZXY: return "ZXY";
		case EulerOrder_YXZ: return "YXZ";
		case EulerOrder_YZX: return "YZX";
		default: return "Unknown";
	}
}
#endif

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define Quat_Identity NewQuat(0, 0, 0, 1, false)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	quat QuatNormalize(const quat& quaternion);
	quat NewQuat(v3 axis, r32 angle, bool normalize = true);
	quat NewQuat(v4 vec4, bool normalize = true);
	quat NewQuat(r32 x, r32 y, r32 z, r32 w, bool normalize = true);
	quat ToQuat(mat4 matrix, bool assertOnFailure = true);
	quat ToQuat(Basis_t basis, bool assertOnFailure = true);
	r32 QuatGetAngle(const quat& quaternion);
	v3 QuatGetAxis(const quat& quaternion);
	quat QuatEquivalent(const quat& quaternion);
	quat QuatOpposite(const quat& quaternion);
	quat QuatLerp(const quat& start, const quat& end, r32 amount, bool linearly = true, bool normalizeResult = true);
	quat QuatMult(const quat& left, const quat& right, bool normalize = true);
	mat4 Mat4Quaternion(quat q);
	quat QuatLocalRot(quat quaternion, v3 axis, r32 angle);
	quat QuatGlobalRot(quat quaternion, v3 axis, r32 angle);
	quat NewQuatFromEuler(v3 eulerAngles, EulerOrder_t order = EulerOrder_XYZ);
	quat NewQuatFromEuler(r32 eulerX, r32 eulerY, r32 eulerZ, EulerOrder_t order = EulerOrder_XYZ);
	v3 QuatGetRightVec(quat quaternion);
	v3 QuatGetUpVec(quat quaternion);
	v3 QuatGetForwardVec(quat quaternion);
	v3 QuatGetAxisVec(quat quaternion, Axis_t axis);
	Basis_t QuatGetBasis(quat quaternion);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
quat QuatNormalize(const quat& quaternion)
{
	//TODO: Is this actually what needs to happen?
	quat result = {};
	result.vec4 = Vec4Normalize(quaternion.vec4);
	return result;
}
quat NewQuat(v3 axis, r32 angle, bool normalize = true)
{
	quat result = {};
	result.axis = Vec3Normalize(axis) * SinR32(angle) / 2;
	result.angle = CosR32(angle) / 2;
	if (normalize) { result = QuatNormalize(result); }
	return result;
}
quat NewQuat(v4 vec4, bool normalize = true) //NOTE: This is not the same as assigning the vec4 member directly!
{
	quat result = {};
	result.vec4 = vec4;
	if (normalize) { result = QuatNormalize(result); }
	return result;
}
quat NewQuat(r32 x, r32 y, r32 z, r32 w, bool normalize = true)
{
	quat result = {};
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	if (normalize) { result = QuatNormalize(result); }
	return result;
}

quat ToQuat(mat4 matrix, bool assertOnFailure = true)
{
	quat result;
	
	// The determinant of a purely rotation matrix should be 1
	r32 determinant = Mat4Determinant(matrix);
	if (!BasicallyEqualR32(determinant, 1.0f))
	{
		AssertIfMsg(assertOnFailure, false, "Determinant of matrix was not 1.0 when doing matrix to quaternion conversion!");
		return NewQuat(0, 0, 0, 1, false); //identity
	}
	
	r32 trace = matrix.r0c0 + matrix.r1c1 + matrix.r2c2;
	if (trace > 0)
	{
		r32 S = SqrtR32(trace + 1.0f) * 2; // S=4*result.w 
		result.w = 0.25f * S;
		result.x = (matrix.r2c1 - matrix.r1c2) / S;
		result.y = (matrix.r0c2 - matrix.r2c0) / S; 
		result.z = (matrix.r1c0 - matrix.r0c1) / S; 
	}
	else if ((matrix.r0c0 > matrix.r1c1) && (matrix.r0c0 > matrix.r2c2))
	{
		r32 S = SqrtR32(1.0f + matrix.r0c0 - matrix.r1c1 - matrix.r2c2) * 2; // S=4*result.x 
		result.w = (matrix.r2c1 - matrix.r1c2) / S;
		result.x = 0.25f * S;
		result.y = (matrix.r0c1 + matrix.r1c0) / S; 
		result.z = (matrix.r0c2 + matrix.r2c0) / S; 
	}
	else if (matrix.r1c1 > matrix.r2c2)
	{
		r32 S = SqrtR32(1.0f + matrix.r1c1 - matrix.r0c0 - matrix.r2c2) * 2; // S=4*result.y
		result.w = (matrix.r0c2 - matrix.r2c0) / S;
		result.x = (matrix.r0c1 + matrix.r1c0) / S; 
		result.y = 0.25f * S;
		result.z = (matrix.r1c2 + matrix.r2c1) / S; 
	}
	else
	{
		r32 S = SqrtR32(1.0f + matrix.r2c2 - matrix.r0c0 - matrix.r1c1) * 2; // S=4*result.z
		result.w = (matrix.r1c0 - matrix.r0c1) / S;
		result.x = (matrix.r0c2 + matrix.r2c0) / S;
		result.y = (matrix.r1c2 + matrix.r2c1) / S;
		result.z = 0.25f * S;
	}
	
	return result;
}
quat ToQuat(Basis_t basis, bool assertOnFailure = true)
{
	if (!IsBasisLinearIndependent(basis))
	{
		AssertIfMsg(assertOnFailure, false, "Basis vectors were not linearly independent when converting basis to quaternion!");
		return NewQuat(0, 0, 0, 1, false); //identity
	}
	mat4 basisMat = NewMat4(
		basis.right.x, basis.up.x, basis.forward.x, 0,
		basis.right.y, basis.up.y, basis.forward.y, 0,
		basis.right.z, basis.up.z, basis.forward.z, 0,
		            0,          0,               0, 1
	);
	return ToQuat(basisMat, assertOnFailure);
}

// +--------------------------------------------------------------+
// |                    Basic Math Operations                     |
// +--------------------------------------------------------------+
r32 QuatGetAngle(const quat& quaternion)
{
	//TODO: Implement me better?
	return AcosR32(2*quaternion.angle);
}
v3 QuatGetAxis(const quat& quaternion)
{
	v3 result = quaternion.axis;
	r32 resultLength = Vec3Length(result);
	if (resultLength == 0) { return Vec3_Right; }
	else { return result / resultLength; }
}

quat QuatEquivalent(const quat& quaternion)
{
	return NewQuat(-quaternion.vec4);
}

quat QuatOpposite(const quat& quaternion)
{
	r32 squareAll = Square(quaternion.x) + Square(quaternion.y) + Square(quaternion.z) + Square(quaternion.w);
	quat result;
	result.x = -quaternion.x / squareAll;
	result.y = -quaternion.y / squareAll;
	result.z = -quaternion.z / squareAll;
	result.w = quaternion.w / squareAll;
	result = QuatNormalize(result);
	return result;
}

quat QuatLerp(const quat& start, const quat& end, r32 amount, bool linearly = true, bool normalizeResult = true)
{
	if (amount >= 1.0f) { return end; }
	if (amount <= 0.0f) { return start; }
	quat result;
	if (linearly)
	{
		// The "intuitive" way I did it, is a little less efficient, but I'll keep it around for reference. Both work
		// quat oppositeEnd = QuatEquivalent(end);
		// if (Vec4Length(end.vec4 - start.vec4) <= Vec4Length(oppositeEnd.vec4 - start.vec4))
		
		if (Vec4Dot(start.vec4, end.vec4) >= 0)
		{
			result = NewQuat(Vec4Lerp(start.vec4, end.vec4, amount));
		}
		else
		{
			result = NewQuat(Vec4Lerp(start.vec4, QuatEquivalent(end).vec4, amount));
		}
	}
	else
	{
		result = start; //TODO: Implement this variant!
	}
	if (normalizeResult) { result = QuatNormalize(result); }
	return result;
}

quat QuatMult(const quat& left, const quat& right, bool normalize = true)
{
	quat result;
	result.angle = left.angle*right.angle - Vec3Dot(left.axis, right.axis);
	result.axis = left.angle*right.axis + right.angle*left.axis + Vec3Cross(left.axis, right.axis);
	if (normalize) { result = QuatNormalize(result); }
	return result;
}

// +--------------------------------------------------------------+
// |                       Matrix Functions                       |
// +--------------------------------------------------------------+
//NOTE: Quaternion should be pre-normalized
mat4 Mat4Quaternion(quat q)
{
	return NewMat4(
		1 - 2*q.y*q.y - 2*q.z*q.z,     2*q.x*q.y - 2*q.z*q.w,     2*q.x*q.z + 2*q.y*q.w, 0,
		    2*q.x*q.y + 2*q.z*q.w, 1 - 2*q.x*q.x - 2*q.z*q.z,     2*q.y*q.z - 2*q.x*q.w, 0,
		    2*q.x*q.z - 2*q.y*q.w,     2*q.y*q.z + 2*q.x*q.w, 1 - 2*q.x*q.x - 2*q.y*q.y, 0,
		                        0,                         0,                         0, 1
	);
}

quat QuatLocalRot(quat quaternion, v3 axis, r32 angle)
{
	quat deltaQuat = NewQuat(axis, angle);
	return QuatMult(quaternion, deltaQuat);
}

quat QuatGlobalRot(quat quaternion, v3 axis, r32 angle)
{
	v3 localAxis = Mat4MultiplyRightVec3(axis, Mat4Quaternion(quaternion));
	quat deltaQuat = NewQuat(localAxis, angle);
	return QuatMult(quaternion, deltaQuat);
}

// +--------------------------------------------------------------+
// |                       Euler Functions                        |
// +--------------------------------------------------------------+
quat NewQuatFromEuler(v3 eulerAngles, EulerOrder_t order = EulerOrder_XYZ)
{
	quat result = Quat_Identity;
	Axis_t axisOrder[3] = {};
	switch (order)
	{
		case EulerOrder_XYZ: axisOrder[0] = Axis_X; axisOrder[1] = Axis_Y; axisOrder[2] = Axis_Z; break;
		case EulerOrder_ZYX: axisOrder[0] = Axis_Z; axisOrder[1] = Axis_Y; axisOrder[2] = Axis_X; break;
		case EulerOrder_XZY: axisOrder[0] = Axis_X; axisOrder[1] = Axis_Z; axisOrder[2] = Axis_Y; break;
		case EulerOrder_ZXY: axisOrder[0] = Axis_Z; axisOrder[1] = Axis_X; axisOrder[2] = Axis_Y; break;
		case EulerOrder_YXZ: axisOrder[0] = Axis_Y; axisOrder[1] = Axis_X; axisOrder[2] = Axis_Z; break;
		case EulerOrder_YZX: axisOrder[0] = Axis_Y; axisOrder[1] = Axis_Z; axisOrder[2] = Axis_X; break;
		default: break;
	}
	for (u8 aIndex = 0; aIndex < 3; aIndex++)
	{
		switch (axisOrder[aIndex])
		{
			//TODO: Remove the need for division by 2 here!
			case Axis_X: result = QuatGlobalRot(result, Vec3_Right,   eulerAngles.x/2); break;
			case Axis_Y: result = QuatGlobalRot(result, Vec3_Up,      eulerAngles.y/2); break;
			case Axis_Z: result = QuatGlobalRot(result, Vec3_Forward, eulerAngles.z/2); break;
			default: break;
		}
	}
	return result;
}
quat NewQuatFromEuler(r32 eulerX, r32 eulerY, r32 eulerZ, EulerOrder_t order = EulerOrder_XYZ)
{
	return NewQuatFromEuler(NewVec3(eulerX, eulerY, eulerZ), order);
}

// +--------------------------------------------------------------+
// |                 Basic Information Functions                  |
// +--------------------------------------------------------------+
v3 QuatGetRightVec(quat quaternion)
{
	mat4 quaternionMatrix = Mat4Quaternion(quaternion);
	v3 result = Mat4MultiplyVec3(quaternionMatrix, Vec3_Right, false);
	return result;
}
v3 QuatGetUpVec(quat quaternion)
{
	mat4 quaternionMatrix = Mat4Quaternion(quaternion);
	v3 result = Mat4MultiplyVec3(quaternionMatrix, Vec3_Up, false);
	return result;
}
v3 QuatGetForwardVec(quat quaternion)
{
	mat4 quaternionMatrix = Mat4Quaternion(quaternion);
	v3 result = Mat4MultiplyVec3(quaternionMatrix, Vec3_Forward, false);
	return result;
}
v3 QuatGetAxisVec(quat quaternion, Axis_t axis)
{
	mat4 quaternionMatrix = Mat4Quaternion(quaternion);
	v3 result = Mat4MultiplyVec3(quaternionMatrix, ToVec3(axis), false);
	return result;
}
Basis_t QuatGetBasis(quat quaternion)
{
	return NewBasis(QuatGetRightVec(quaternion), QuatGetUpVec(quaternion), QuatGetForwardVec(quaternion), false);
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_QUATERNIONS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
Quat_Identity
@Types
Quaternion_t
quat
@Functions
quat QuatNormalize(const quat& quaternion)
quat NewQuat(r32 x, r32 y, r32 z, r32 w, bool normalize = true)
quat ToQuat(mat4 matrix, bool assertOnFailure = true)
quat NewQuatFromBasis(Basis_t basis)
r32 QuatGetAngle(const quat& quaternion)
v3 QuatGetAxis(const quat& quaternion)
quat QuatEquivalent(const quat& quaternion)
quat QuatOpposite(const quat& quaternion)
quat QuatLerp(const quat& start, const quat& end, r32 amount, bool linearly = true, bool normalizeResult = true)
quat QuatMult(const quat& left, const quat& right, bool normalize = true)
mat4 Mat4Quaternion(quat q)
quat QuatLocalRot(quat q, v3 axis, r32 angle)
quat QuatGlobalRot(quat q, v3 axis, r32 angle)
quat NewQuatFromEuler(v3 eulerAngles, EulerOrder_t order = EulerOrder_XYZ)
v3 QuatGetRightVec(quat quaternion)
v3 QuatGetUpVec(quat quaternion)
v3 QuatGetForwardVec(quat quaternion)
v3 QuatGetAxisVec(quat quaternion, Axis_t axis)
Basis_t QuatGetBasis(quat quaternion)
*/
