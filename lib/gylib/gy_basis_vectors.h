/*
File:   gy_basis_vectors.h
Author: Taylor Robbins
Date:   10\17\2022
Description:
	** Holds functions that help us deal with vector spasis in terms of "basis vectors"
	** i.e. the default basis vectors are (1, 0, 0) (0, 1, 0) and (0, 0, 1) but we could have
	** any 3 mutually perpendicular vectors act as basis vectors.
	** Converting between basis vectors is very handy to simplify various calculations!
*/

#ifndef _GY_BASIS_VECTORS_H
#define _GY_BASIS_VECTORS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"
#include "gy_directions.h"

// +--------------------------------------------------------------+
// |                          Structure                           |
// +--------------------------------------------------------------+
union Basis_t
{
	v3 vectors[3];
	struct
	{
		v3 right;
		v3 up;
		v3 forward;
	};
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define Basis_Default NewBasis(Vec3_Right, Vec3_Up, Vec3_Forward, false)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	Basis_t BasisNormalize(Basis_t basis);
	void BasisNormalize(Basis_t* basisPntr);
	Basis_t NewBasis(v3 right, v3 up, v3 forward, bool normalize = true);
	Basis_t NewBasisForwardUp(v3 forward, v3 up, bool normalize = true);
	Basis_t NewBasisRightUp(v3 right, v3 up, bool normalize = true);
	bool IsBasisLinearIndependent(Basis_t basis, r32 tolerance = 0.001f);
	bool AreBasisEqual(Basis_t left, Basis_t right, r32 tolerance = 0.001f);
	v3 ApplyBasis(Basis_t toBasis, v3 vector);
	v3 RemoveBasis(Basis_t fromBasis, v3 vector);
	v3 ChangeBasis(Basis_t fromBasis, Basis_t toBasis, v3 vector);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Basis_t BasisNormalize(Basis_t basis)
{
	basis.right   = Vec3Normalize(basis.right);
	basis.up      = Vec3Normalize(basis.up);
	basis.forward = Vec3Normalize(basis.forward);
	return basis;
}
void BasisNormalize(Basis_t* basisPntr)
{
	basisPntr->right   = Vec3Normalize(basisPntr->right);
	basisPntr->up      = Vec3Normalize(basisPntr->up);
	basisPntr->forward = Vec3Normalize(basisPntr->forward);
}
Basis_t NewBasis(v3 right, v3 up, v3 forward, bool normalize = true)
{
	Basis_t result;
	result.right   = right;
	result.up      = up;
	result.forward = forward;
	if (normalize) { BasisNormalize(&result); }
	return result;
}
Basis_t NewBasisForwardUp(v3 forward, v3 up, bool normalize = true)
{
	if (normalize)
	{
		forward = Vec3Normalize(forward);
		up = Vec3Normalize(up);
	}
	v3 right = Vec3Cross(forward, up);
	return NewBasis(right, up, forward, false);
}
Basis_t NewBasisRightUp(v3 right, v3 up, bool normalize = true)
{
	if (normalize)
	{
		right = Vec3Normalize(right);
		up = Vec3Normalize(up);
	}
	v3 forward = Vec3Cross(up, right);
	return NewBasis(right, up, forward, false);
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
bool IsBasisLinearIndependent(Basis_t basis, r32 tolerance = 0.001f)
{
	r32 dotForwardRight = Vec3Dot(basis.forward, basis.right);
	r32 dotForwardUp = Vec3Dot(basis.forward, basis.up);
	r32 dotRightUp = Vec3Dot(basis.right, basis.up);
	if (!BasicallyEqualR32(dotForwardRight, 0.0f, tolerance)) { return false; }
	if (!BasicallyEqualR32(dotForwardUp,    0.0f, tolerance)) { return false; }
	if (!BasicallyEqualR32(dotRightUp,      0.0f, tolerance)) { return false; }
	return true;
}

bool AreBasisEqual(Basis_t left, Basis_t right, r32 tolerance = 0.001f)
{
	if (!Vec3BasicallyEqual(left.right,   right.right,   tolerance)) { return false; }
	if (!Vec3BasicallyEqual(left.up,      right.up,      tolerance)) { return false; }
	if (!Vec3BasicallyEqual(left.forward, right.forward, tolerance)) { return false; }
	return true;
}

v3 ApplyBasis(Basis_t toBasis, v3 vector)
{
	v3 result;
	result.x = Vec3Dot(vector, toBasis.right);
	result.y = Vec3Dot(vector, toBasis.up);
	result.z = Vec3Dot(vector, toBasis.forward);
	return result;
}

v3 RemoveBasis(Basis_t fromBasis, v3 vector)
{
	v3 result = Vec3_Zero;
	result += vector.x * fromBasis.right;
	result += vector.y * fromBasis.up;
	result += vector.z * fromBasis.forward;
	return result;
}

v3 ChangeBasis(Basis_t fromBasis, Basis_t toBasis, v3 vector)
{
	v3 result = RemoveBasis(fromBasis, vector);
	result = ApplyBasis(toBasis, vector);
	return result;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_BASIS_VECTORS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
Basis_Default
@Types
Basis_t
@Functions
Basis_t BasisNormalize(Basis_t basis)
Basis_t NewBasis(v3 right, v3 up, v3 forward, bool normalize = true)
Basis_t NewBasisForwardUp(v3 forward, v3 up, bool normalize = true)
Basis_t NewBasisRightUp(v3 right, v3 up, bool normalize = true)
bool IsBasisLinearIndependent(Basis_t basis, r32 tolerance = 0.001f)
bool AreBasisEqual(Basis_t left, Basis_t right, r32 tolerance = 0.001f)
v3 ApplyBasis(Basis_t toBasis, v3 vector)
v3 RemoveBasis(Basis_t fromBasis, v3 vector)
v3 ChangeBasis(Basis_t fromBasis, Basis_t toBasis, v3 vector)
*/
