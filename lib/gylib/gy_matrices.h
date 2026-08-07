/*
File:   gy_matrices.h
Author: Taylor Robbins
Date:   10\06\2021
Description:
	** Contains the definitions and related functions for mat4 (Matrix4x4_t)
*/

#ifndef _GY_MATRICES_H
#define _GY_MATRICES_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
union Matrix4x4_t
{
	//         C  R
	r32 values[4][4];
	struct
	{
		v4 col0;
		v4 col1;
		v4 col2;
		v4 col3;
	};
	struct
	{
		r32 r0c0, r1c0, r2c0, r3c0;
		r32 r0c1, r1c1, r2c1, r3c1;
		r32 r0c2, r1c2, r2c2, r3c2;
		r32 r0c3, r1c3, r2c3, r3c3;
	};
	struct
	{
		r32 row0col0, row1col0, row2col0, row3col0;
		r32 row0col1, row1col1, row2col1, row3col1;
		r32 row0col2, row1col2, row2col2, row3col2;
		r32 row0col3, row1col3, row2col3, row3col3;
	};
	struct
	{
		r32 a, e, i, m;
		r32 b, f, j, n;
		r32 c, g, k, o;
		r32 d, h, l, p;
	};
};

typedef Matrix4x4_t mat4;

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define Matrix4x4_Identity NewMat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f)
#define Mat4_Identity Matrix4x4_Identity

#define Mat3Determinant(a, b, c, d, e, f, g, h, i) (((a) * ((e)*(i) - (h)*(f))) - ((b) * ((d)*(i) - (g)*(f))) + ((c) * ((d)*(h) - (g)*(e))))

#define Mat4ApplyLeft(matrix, transformation) (matrix) = Mat4Multiply((transformation), (matrix))
#define Mat4Transform(matrix, transformation) Mat4ApplyLeft((matrix), (transformation))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	mat4 NewMat4(
		r32 r0c0, r32 r0c1, r32 r0c2, r32 r0c3,
		r32 r1c0, r32 r1c1, r32 r1c2, r32 r1c3,
		r32 r2c0, r32 r2c1, r32 r2c2, r32 r2c3,
		r32 r3c0, r32 r3c1, r32 r3c2, r32 r3c3);
	mat4 Mat4Fill(r32 all);
	mat4 Mat4Diagonal(r32 r0c0, r32 r1c1, r32 r2c2, r32 r3c3, r32 other = 0.0f);
	mat4 Mat4FromBasis(v3 basisX, v3 basisY, v3 basisZ);
	r32 Mat4Determinant(mat4 matrix);
	mat4 Mat4Transpose(mat4 matrix);
	mat4 Mat4Cofactor(mat4 matrix);
	mat4 Mat4Adjoint(mat4 matrix);
	mat4 Mat4Inverse(mat4 matrix, bool* successOut = nullptr);
	mat4 Mat4Multiply(mat4 left, mat4 right);
	mat4 Mat4Multiply(mat4 outer, mat4 left, mat4 right);
	v2 Mat4MultiplyVec2(mat4 matrix, v2 vector, bool includeTranslation = true);
	v3 Mat4MultiplyVec3(mat4 matrix, v3 vector, bool includeTranslation = true, r32* wOut = nullptr);
	v3 Mat4MultiplyRightVec3(v3 vector, mat4 matrix, r32* wOut = nullptr);
	v4 Mat4MultiplyVec4(mat4 matrix, v4 vector, bool divideByW = true);
	mat4 operator * (const mat4& left, const mat4& right);
	bool operator == (mat4 left, mat4 right);
	bool operator != (mat4 left, mat4 right);
	mat4 Mat4Translate3(v3 translation);
	mat4 Mat4Translate3(r32 x, r32 y, r32 z);
	mat4 Mat4Translate2(v2 translation);
	mat4 Mat4Translate2(r32 x, r32 y);
	mat4 Mat4TranslateX(r32 x);
	mat4 Mat4TranslateY(r32 y);
	mat4 Mat4TranslateZ(r32 z);
	mat4 Mat4ScaleX(r32 scale);
	mat4 Mat4ScaleY(r32 scale);
	mat4 Mat4ScaleZ(r32 scale);
	mat4 Mat4ScaleW(r32 scale);
	mat4 Mat4Scale3(v3 scale);
	mat4 Mat4Scale3(r32 x, r32 y, r32 z);
	mat4 Mat4Scale2(v2 scale);
	mat4 Mat4Scale2(r32 x, r32 y);
	mat4 Mat4RotateX(r32 angle);
	mat4 Mat4RotateY(r32 angle);
	mat4 Mat4RotateZ(r32 angle);
	mat4 Mat4Rotate(v3 axis, r32 angle);
	mat4 Mat4LookAt(v3 position, v3 lookAt, v3 upVector, bool rightHanded = false);
	mat4 Mat4Perspective(r32 fovy, r32 aspectRatio, r32 zNear, r32 zFar, bool rightHanded = false);
	mat4 Mat4Orthographic(r32 left, r32 right, r32 top, r32 bottom, r32 zNear, r32 zFar);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
mat4 NewMat4(
	r32 r0c0, r32 r0c1, r32 r0c2, r32 r0c3,
	r32 r1c0, r32 r1c1, r32 r1c2, r32 r1c3,
	r32 r2c0, r32 r2c1, r32 r2c2, r32 r2c3,
	r32 r3c0, r32 r3c1, r32 r3c2, r32 r3c3)
{
	mat4 result = {
		r0c0, r1c0, r2c0, r3c0,
		r0c1, r1c1, r2c1, r3c1,
		r0c2, r1c2, r2c2, r3c2,
		r0c3, r1c3, r2c3, r3c3
	};
	return result;
}
mat4 Mat4Fill(r32 all)
{
	mat4 result = {
		all, all, all, all,
		all, all, all, all,
		all, all, all, all,
		all, all, all, all
	};
	return result;
}
mat4 Mat4Diagonal(r32 r0c0, r32 r1c1, r32 r2c2, r32 r3c3, r32 other = 0.0f)
{
	mat4 result = {
		 r0c0, other, other, other,
		other,  r1c1, other, other,
		other, other,  r2c2, other,
		other, other, other,  r3c3
	};
	return result;
}
mat4 Mat4FromBasis(v3 basisX, v3 basisY, v3 basisZ)
{
	return NewMat4(
		basisX.x, basisY.x, basisZ.x, 0,
		basisX.y, basisY.y, basisZ.y, 0,
		basisX.z, basisY.z, basisZ.z, 0,
		0,        0,        0,        1
	);
}

// +--------------------------------------------------------------+
// |                    Transpose and Inverse                     |
// +--------------------------------------------------------------+
r32 Mat4Determinant(mat4 matrix)
{
	r32 result = 0;
	result += matrix.r0c0 * ((matrix.r1c1*matrix.r2c2*matrix.r3c3) + (matrix.r1c2*matrix.r2c3*matrix.r3c1) + (matrix.r1c3*matrix.r2c1*matrix.r3c2) - (matrix.r1c3*matrix.r2c2*matrix.r3c1) - (matrix.r1c2*matrix.r2c1*matrix.r3c3) - (matrix.r1c1*matrix.r2c3*matrix.r3c2));
	result -= matrix.r1c0 * ((matrix.r0c1*matrix.r2c2*matrix.r3c3) + (matrix.r0c2*matrix.r2c3*matrix.r3c1) + (matrix.r0c3*matrix.r2c1*matrix.r3c2) - (matrix.r0c3*matrix.r2c2*matrix.r3c1) - (matrix.r0c2*matrix.r2c1*matrix.r3c3) - (matrix.r0c1*matrix.r2c3*matrix.r3c2));
	result += matrix.r2c0 * ((matrix.r0c1*matrix.r1c2*matrix.r3c3) + (matrix.r0c2*matrix.r1c3*matrix.r3c1) + (matrix.r0c3*matrix.r1c1*matrix.r3c2) - (matrix.r0c3*matrix.r1c2*matrix.r3c1) - (matrix.r0c2*matrix.r1c1*matrix.r3c3) - (matrix.r0c1*matrix.r1c3*matrix.r3c2));
	result -= matrix.r3c0 * ((matrix.r0c1*matrix.r1c2*matrix.r2c3) + (matrix.r0c2*matrix.r1c3*matrix.r2c1) + (matrix.r0c3*matrix.r1c1*matrix.r2c2) - (matrix.r0c3*matrix.r1c2*matrix.r2c1) - (matrix.r0c2*matrix.r1c1*matrix.r2c3) - (matrix.r0c1*matrix.r1c3*matrix.r2c2));
	return result;
}

mat4 Mat4Transpose(mat4 matrix)
{
	mat4 result = {};
	result.r0c0 = matrix.r0c0; result.r1c0 = matrix.r0c1; result.r2c0 = matrix.r0c2; result.r3c0 = matrix.r0c3;
	result.r0c1 = matrix.r1c0; result.r1c1 = matrix.r1c1; result.r2c1 = matrix.r1c2; result.r3c1 = matrix.r1c3;
	result.r0c2 = matrix.r2c0; result.r1c2 = matrix.r2c1; result.r2c2 = matrix.r2c2; result.r3c2 = matrix.r2c3;
	result.r0c3 = matrix.r3c0; result.r1c3 = matrix.r3c1; result.r2c3 = matrix.r3c2; result.r3c3 = matrix.r3c3;
	return result;
}

mat4 Mat4Cofactor(mat4 matrix)
{
	mat4 result;
	
	result.r0c0 =  Mat3Determinant(matrix.r1c1, matrix.r1c2, matrix.r1c3, matrix.r2c1, matrix.r2c2, matrix.r2c3, matrix.r3c1, matrix.r3c2, matrix.r3c3);
	result.r0c1 = -Mat3Determinant(matrix.r1c0, matrix.r1c2, matrix.r1c3, matrix.r2c0, matrix.r2c2, matrix.r2c3, matrix.r3c0, matrix.r3c2, matrix.r3c3);
	result.r0c2 =  Mat3Determinant(matrix.r1c0, matrix.r1c1, matrix.r1c3, matrix.r2c0, matrix.r2c1, matrix.r2c3, matrix.r3c0, matrix.r3c1, matrix.r3c3);
	result.r0c3 = -Mat3Determinant(matrix.r1c0, matrix.r1c1, matrix.r1c2, matrix.r2c0, matrix.r2c1, matrix.r2c2, matrix.r3c0, matrix.r3c1, matrix.r3c2);
	
	result.r1c0 = -Mat3Determinant(matrix.r0c1, matrix.r0c2, matrix.r0c3, matrix.r2c1, matrix.r2c2, matrix.r2c3, matrix.r3c1, matrix.r3c2, matrix.r3c3);
	result.r1c1 =  Mat3Determinant(matrix.r0c0, matrix.r0c2, matrix.r0c3, matrix.r2c0, matrix.r2c2, matrix.r2c3, matrix.r3c0, matrix.r3c2, matrix.r3c3);
	result.r1c2 = -Mat3Determinant(matrix.r0c0, matrix.r0c1, matrix.r0c3, matrix.r2c0, matrix.r2c1, matrix.r2c3, matrix.r3c0, matrix.r3c1, matrix.r3c3);
	result.r1c3 =  Mat3Determinant(matrix.r0c0, matrix.r0c1, matrix.r0c2, matrix.r2c0, matrix.r2c1, matrix.r2c2, matrix.r3c0, matrix.r3c1, matrix.r3c2);
	
	result.r2c0 =  Mat3Determinant(matrix.r0c1, matrix.r0c2, matrix.r0c3, matrix.r1c1, matrix.r1c2, matrix.r1c3, matrix.r3c1, matrix.r3c2, matrix.r3c3);
	result.r2c1 = -Mat3Determinant(matrix.r0c0, matrix.r0c2, matrix.r0c3, matrix.r1c0, matrix.r1c2, matrix.r1c3, matrix.r3c0, matrix.r3c2, matrix.r3c3);
	result.r2c2 =  Mat3Determinant(matrix.r0c0, matrix.r0c1, matrix.r0c3, matrix.r1c0, matrix.r1c1, matrix.r1c3, matrix.r3c0, matrix.r3c1, matrix.r3c3);
	result.r2c3 = -Mat3Determinant(matrix.r0c0, matrix.r0c1, matrix.r0c2, matrix.r1c0, matrix.r1c1, matrix.r1c2, matrix.r3c0, matrix.r3c1, matrix.r3c2);
	
	result.r3c0 = -Mat3Determinant(matrix.r0c1, matrix.r0c2, matrix.r0c3, matrix.r1c1, matrix.r1c2, matrix.r1c3, matrix.r2c1, matrix.r2c2, matrix.r2c3);
	result.r3c1 =  Mat3Determinant(matrix.r0c0, matrix.r0c2, matrix.r0c3, matrix.r1c0, matrix.r1c2, matrix.r1c3, matrix.r2c0, matrix.r2c2, matrix.r2c3);
	result.r3c2 = -Mat3Determinant(matrix.r0c0, matrix.r0c1, matrix.r0c3, matrix.r1c0, matrix.r1c1, matrix.r1c3, matrix.r2c0, matrix.r2c1, matrix.r2c3);
	result.r3c3 =  Mat3Determinant(matrix.r0c0, matrix.r0c1, matrix.r0c2, matrix.r1c0, matrix.r1c1, matrix.r1c2, matrix.r2c0, matrix.r2c1, matrix.r2c2);
	
	return result;
}

mat4 Mat4Adjoint(mat4 matrix)
{
	mat4 result = Mat4Cofactor(matrix);
	result = Mat4Transpose(result);
	return result;
}

mat4 Mat4Inverse(mat4 matrix, bool* successOut = nullptr)
{
	r32 determinant = Mat4Determinant(matrix);
	if (determinant == 0)
	{
		if (successOut != nullptr) { *successOut = false; }
		return Mat4_Identity;
	}
	
	mat4 result = Mat4Adjoint(matrix);
	result.r0c0 = result.r0c0 / determinant;
	result.r0c1 = result.r0c1 / determinant;
	result.r0c2 = result.r0c2 / determinant;
	result.r0c3 = result.r0c3 / determinant;
	result.r1c0 = result.r1c0 / determinant;
	result.r1c1 = result.r1c1 / determinant;
	result.r1c2 = result.r1c2 / determinant;
	result.r1c3 = result.r1c3 / determinant;
	result.r2c0 = result.r2c0 / determinant;
	result.r2c1 = result.r2c1 / determinant;
	result.r2c2 = result.r2c2 / determinant;
	result.r2c3 = result.r2c3 / determinant;
	result.r3c0 = result.r3c0 / determinant;
	result.r3c1 = result.r3c1 / determinant;
	result.r3c2 = result.r3c2 / determinant;
	result.r3c3 = result.r3c3 / determinant;
	if (successOut != nullptr) { *successOut = true; }
	return result;
}

// +--------------------------------------------------------------+
// |                Operator Overload Equivalents                 |
// +--------------------------------------------------------------+
mat4 Mat4Multiply(mat4 left, mat4 right)
{
	mat4 result = {
		left.r0c0*right.r0c0 + left.r0c1*right.r1c0 + left.r0c2*right.r2c0 + left.r0c3*right.r3c0, //r0c0
		left.r1c0*right.r0c0 + left.r1c1*right.r1c0 + left.r1c2*right.r2c0 + left.r1c3*right.r3c0, //r1c0
		left.r2c0*right.r0c0 + left.r2c1*right.r1c0 + left.r2c2*right.r2c0 + left.r2c3*right.r3c0, //r2c0
		left.r3c0*right.r0c0 + left.r3c1*right.r1c0 + left.r3c2*right.r2c0 + left.r3c3*right.r3c0, //r3c0
		
		left.r0c0*right.r0c1 + left.r0c1*right.r1c1 + left.r0c2*right.r2c1 + left.r0c3*right.r3c1, //r0c1
		left.r1c0*right.r0c1 + left.r1c1*right.r1c1 + left.r1c2*right.r2c1 + left.r1c3*right.r3c1, //r1c1
		left.r2c0*right.r0c1 + left.r2c1*right.r1c1 + left.r2c2*right.r2c1 + left.r2c3*right.r3c1, //r2c1
		left.r3c0*right.r0c1 + left.r3c1*right.r1c1 + left.r3c2*right.r2c1 + left.r3c3*right.r3c1, //r3c1
		
		left.r0c0*right.r0c2 + left.r0c1*right.r1c2 + left.r0c2*right.r2c2 + left.r0c3*right.r3c2, //r0c2
		left.r1c0*right.r0c2 + left.r1c1*right.r1c2 + left.r1c2*right.r2c2 + left.r1c3*right.r3c2, //r1c2
		left.r2c0*right.r0c2 + left.r2c1*right.r1c2 + left.r2c2*right.r2c2 + left.r2c3*right.r3c2, //r2c2
		left.r3c0*right.r0c2 + left.r3c1*right.r1c2 + left.r3c2*right.r2c2 + left.r3c3*right.r3c2, //r3c2
		
		left.r0c0*right.r0c3 + left.r0c1*right.r1c3 + left.r0c2*right.r2c3 + left.r0c3*right.r3c3, //r0c3
		left.r1c0*right.r0c3 + left.r1c1*right.r1c3 + left.r1c2*right.r2c3 + left.r1c3*right.r3c3, //r1c3
		left.r2c0*right.r0c3 + left.r2c1*right.r1c3 + left.r2c2*right.r2c3 + left.r2c3*right.r3c3, //r2c3
		left.r3c0*right.r0c3 + left.r3c1*right.r1c3 + left.r3c2*right.r2c3 + left.r3c3*right.r3c3  //r3c3
	};
	return result;
}
//NOTE: This will multiply left*right FIRST and then multiply by outer
//		It's the same as outer * (left * right)
mat4 Mat4Multiply(mat4 outer, mat4 left, mat4 right)
{
	return Mat4Multiply(outer, Mat4Multiply(left, right));
}

//TODO: These functions should really be checked to make sure they do what is mathematically correct
v2 Mat4MultiplyVec2(mat4 matrix, v2 vector, bool includeTranslation = true)
{
	v2 result = NewVec2(
		matrix.r0c0*vector.x + matrix.r0c1*vector.y + matrix.r0c2*0.0f + matrix.r0c3*(r32)(includeTranslation ? 1 : 0),
		matrix.r1c0*vector.x + matrix.r1c1*vector.y + matrix.r1c2*0.0f + matrix.r1c3*(r32)(includeTranslation ? 1 : 0)
	);
	//TODO: Do we need to divide by w result?
	
	return  result;
}
v3 Mat4MultiplyVec3(mat4 matrix, v3 vector, bool includeTranslation = true, r32* wOut = nullptr)
{
	v3 result = NewVec3(
		matrix.r0c0*vector.x + matrix.r0c1*vector.y + matrix.r0c2*vector.z + matrix.r0c3*(r32)(includeTranslation ? 1 : 0),
		matrix.r1c0*vector.x + matrix.r1c1*vector.y + matrix.r1c2*vector.z + matrix.r1c3*(r32)(includeTranslation ? 1 : 0),
		matrix.r2c0*vector.x + matrix.r2c1*vector.y + matrix.r2c2*vector.z + matrix.r2c3*(r32)(includeTranslation ? 1 : 0)
	);
	
	r32 wValue = matrix.r3c0*vector.x + matrix.r3c1*vector.y + matrix.r3c2*vector.z + matrix.r3c3*1.0f;
	if (wOut != nullptr) { *wOut = wValue; }
	
	result = (1.0f / wValue) * result;
	
	return  result;
}
v3 Mat4MultiplyRightVec3(v3 vector, mat4 matrix, r32* wOut = nullptr)
{
	v3 result = NewVec3(
		matrix.r0c0*vector.x + matrix.r1c0*vector.y + matrix.r2c0*vector.z + matrix.r3c0*1.0f,
		matrix.r0c1*vector.x + matrix.r1c1*vector.y + matrix.r2c1*vector.z + matrix.r3c1*1.0f,
		matrix.r0c2*vector.x + matrix.r1c2*vector.y + matrix.r2c2*vector.z + matrix.r3c2*1.0f
	);
	
	r32 wValue = matrix.r0c3*vector.x + matrix.r1c3*vector.y + matrix.r2c3*vector.z + matrix.r3c3*1.0f;
	if (wOut != nullptr) { *wOut = wValue; }
	
	result = (1.0f / wValue) * result;
	
	return  result;
}
v4 Mat4MultiplyVec4(mat4 matrix, v4 vector, bool divideByW = true)
{
	v4 result = NewVec4(
		matrix.r0c0*vector.x + matrix.r0c1*vector.y + matrix.r0c2*vector.z + matrix.r0c3*vector.w,
		matrix.r1c0*vector.x + matrix.r1c1*vector.y + matrix.r1c2*vector.z + matrix.r1c3*vector.w,
		matrix.r2c0*vector.x + matrix.r2c1*vector.y + matrix.r2c2*vector.z + matrix.r2c3*vector.w,
		matrix.r3c0*vector.x + matrix.r3c1*vector.y + matrix.r3c2*vector.z + matrix.r3c3*vector.w
	);
	if (divideByW) { result = (1.0f / result.w) * result; }
	return  result;
}

// +--------------------------------------------------------------+
// |                      Operator Overloads                      |
// +--------------------------------------------------------------+
mat4 operator * (const mat4& left, const mat4& right) { return Mat4Multiply(left, right); }

bool operator == (mat4 left, mat4 right)
{
	return (
		left.r0c0 == right.r0c0 && left.r0c1 == right.r0c1 && left.r0c2 == right.r0c2 && left.r0c3 == right.r0c3 &&
		left.r1c0 == right.r1c0 && left.r1c1 == right.r1c1 && left.r1c2 == right.r1c2 && left.r1c3 == right.r1c3 &&
		left.r2c0 == right.r2c0 && left.r2c1 == right.r2c1 && left.r2c2 == right.r2c2 && left.r2c3 == right.r2c3 &&
		left.r3c0 == right.r3c0 && left.r3c1 == right.r3c1 && left.r3c2 == right.r3c2 && left.r3c3 == right.r3c3
	);
}
bool operator != (mat4 left, mat4 right)
{
	return (
		left.r0c0 != right.r0c0 || left.r0c1 != right.r0c1 || left.r0c2 != right.r0c2 || left.r0c3 != right.r0c3 ||
		left.r1c0 != right.r1c0 || left.r1c1 != right.r1c1 || left.r1c2 != right.r1c2 || left.r1c3 != right.r1c3 ||
		left.r2c0 != right.r2c0 || left.r2c1 != right.r2c1 || left.r2c2 != right.r2c2 || left.r2c3 != right.r2c3 ||
		left.r3c0 != right.r3c0 || left.r3c1 != right.r3c1 || left.r3c2 != right.r3c2 || left.r3c3 != right.r3c3
	);
}

// +--------------------------------------------------------------+
// |            Specialized Matrix Creation Functions             |
// +--------------------------------------------------------------+
mat4 Mat4Translate3(v3 translation)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f, translation.x,
		0.0f, 1.0f, 0.0f, translation.y,
		0.0f, 0.0f, 1.0f, translation.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}
mat4 Mat4Translate3(r32 x, r32 y, r32 z)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

mat4 Mat4Translate2(v2 translation)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f, translation.x,
		0.0f, 1.0f, 0.0f, translation.y,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}
mat4 Mat4Translate2(r32 x, r32 y)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

mat4 Mat4TranslateX(r32 x)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}
mat4 Mat4TranslateY(r32 y)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}
mat4 Mat4TranslateZ(r32 z)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

mat4 Mat4ScaleX(r32 scale)
{
	return NewMat4(
		scale, 0.0f, 0.0f, 0.0f,
		0.0f,  1.0f, 0.0f, 0.0f,
		0.0f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.0f, 0.0f, 1.0f
	);
}
mat4 Mat4ScaleY(r32 scale)
{
	return NewMat4(
		1.0f, 0.0f,  0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f,  1.0f, 0.0f,
		0.0f, 0.0f,  0.0f, 1.0f
	);
}
mat4 Mat4ScaleZ(r32 scale)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f,  0.0f,
		0.0f, 1.0f, 0.0f,  0.0f,
		0.0f, 0.0f, scale, 0.0f,
		0.0f, 0.0f, 0.0f,  1.0f
	);
}
mat4 Mat4ScaleW(r32 scale)
{
	return NewMat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, scale
	);
}

mat4 Mat4Scale3(v3 scale)
{
	return NewMat4(
		scale.x,  0.0f,     0.0f,     0.0f,
		0.0f,     scale.y,  0.0f,     0.0f,
		0.0f,     0.0f,     scale.z,  0.0f,
		0.0f,     0.0f,     0.0f,     1.0f
	);
}
mat4 Mat4Scale3(r32 x, r32 y, r32 z)
{
	return NewMat4(
		x,    0.0f, 0.0f, 0.0f,
		0.0f, y,    0.0f, 0.0f,
		0.0f, 0.0f, z,    0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

mat4 Mat4Scale2(v2 scale)
{
	return NewMat4(
		scale.x, 0.0f,    0.0f, 0.0f,
		0.0f,    scale.y, 0.0f, 0.0f,
		0.0f,    0.0f,    1.0f, 0.0f,
		0.0f,    0.0f,    0.0f, 1.0f
	);
}
mat4 Mat4Scale2(r32 x, r32 y)
{
	return NewMat4(
		x,    0.0f, 0.0f, 0.0f,
		0.0f, y,    0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

mat4 Mat4RotateX(r32 angle)
{
	r32 s = SinR32(angle);
	r32 c = CosR32(angle);
	
	return NewMat4(
		1, 0, 0, 0,
		0, c,-s, 0,
		0, s, c, 0,
		0, 0, 0, 1
	);
}
mat4 Mat4RotateY(r32 angle)
{
	r32 s = SinR32(angle);
	r32 c = CosR32(angle);
	
	return NewMat4(
		 c, 0,-s, 0,
		 0, 1, 0, 0,
		 s, 0, c, 0,
		 0, 0, 0, 1
	);
}
mat4 Mat4RotateZ(r32 angle)
{
	r32 s = SinR32(angle);
	r32 c = CosR32(angle);
	
	return NewMat4(
		c,-s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

mat4 Mat4Rotate(v3 axis, r32 angle)
{
	r32 c = CosR32(angle);
	r32 s = SinR32(angle);
	r32 t = 1.0f - c;
	
	v3 norm = Vec3Normalize(axis);
	r32 x = norm.x;
	r32 y = norm.y;
	r32 z = norm.z;
	
	return NewMat4(
		1+t*(x*x-1), -z*s+t*x*y,  y*s+t*x*z,   0.0f,
		z*s+t*x*y,   1+t*(y*y-1), -x*s+t*y*z,  0.0f,
		-y*s+t*x*z,  x*s+t*y*z,   1+t*(z*z-1), 0.0f,
		0.0f,        0.0f,        0.0f,        1.0f
	);
}

mat4 Mat4LookAt(v3 position, v3 lookAt, v3 upVector, bool rightHanded = false)
{
	// http://perry.cz/articles/ProjectionMatrix.xhtml
	#if 1
	v3 look  = Vec3Normalize(rightHanded ? (position - lookAt) : (lookAt - position));
	v3 right = Vec3Normalize(Vec3Cross(upVector, look));
	v3 up    = Vec3Normalize(Vec3Cross(look, right));
	r32 rightDot = (rightHanded ? 1.0f : -1.0f) * Vec3Dot(right, position);
	r32 upDot    = (rightHanded ? 1.0f : -1.0f) * Vec3Dot(up,    position);
	r32 lookDot  = (rightHanded ? 1.0f : -1.0f) * Vec3Dot(look,  position);
	// return NewMat4(
	// 	right.x,  up.x,  look.x,  0.0f,
	// 	right.y,  up.y,  look.y,  0.0f,
	// 	right.z,  up.z,  look.z,  0.0f,
	// 	rightDot, upDot, lookDot, 1.0f
	// );
	return NewMat4(
		right.x, right.y, right.z, rightDot,
		   up.x,    up.y,    up.z, upDot,
		 look.x,  look.y,  look.z, lookDot,
		0.0f,    0.0f,    0.0f,    1.0f
	);
	#else //Old system (right handed?)
	v3 look = Vec3Normalize(lookAt - position);
	v3 right = Vec3Normalize(Vec3Cross(look, upVector));
	v3 up = Vec3Normalize(Vec3Cross(right, look));
	r32 rightDot = -Vec3Dot(right, position);
	r32 upDot = -Vec3Dot(up, position);
	r32 lookDot = -Vec3Dot(look, position);
	return NewMat4(
		right.x, right.y, right.z, rightDot,
		up.x   , up.y   , up.z   , upDot   ,
		look.x , look.y , look.z , lookDot ,
		0.0f   , 0.0f   , 0.0f   , 1.0f
	);
	#endif
	
}

mat4 Mat4Perspective(r32 fovy, r32 aspectRatio, r32 zNear, r32 zFar, bool rightHanded = false)
{
	#if 1 //TODO: This currently assumes a OpenGL like depth axis [-1,1] when DirectX and maybe other APIs work on [0,1]
	Assert(zFar > zNear);
	Assert(aspectRatio > 0);
	
	r32 inverseFovTan = (1 / TanR32(fovy * 0.5f));
	r32 zDepth = (zFar - zNear);
	
	r32 A = aspectRatio * inverseFovTan;
	r32 B = inverseFovTan;
	r32 C = -(zFar + zNear) / zDepth;
	r32 D = (rightHanded ? -1.0f : 1.0f);
	r32 E = ((rightHanded ? -2.0f : 2.0f) * zFar * zNear) / zDepth;
	
	return NewMat4(
		 A,   0.0f, 0.0f, 0.0f,
		0.0f, B,    0.0f, 0.0f,
		0.0f, 0.0f, C,    E,
		0.0f, 0.0f, D,    0.0f
	);
	#else //Old Way (this is left-handed I guess?)
	Assert(zFar > zNear);
	Assert(aspectRatio > 0);
	r32 uh = 1.0f / TanR32(fovy / 2.0f);
	r32 uw = uh / aspectRatio;
	r32 depth = zFar - zNear;
	r32 z1 = zFar / depth;
	r32 z2 = (-zFar * zNear) / depth;
	return NewMat4(
		 uw , 0.0f, 0.0f, 0.0f,
		0.0f,  uh , 0.0f, 0.0f,
		0.0f, 0.0f,  z1 ,  z2 ,
		0.0f, 0.0f, 1.0f, 0.0f
	);
	#endif
}

mat4 Mat4Orthographic(r32 left, r32 right, r32 top, r32 bottom, r32 zNear, r32 zFar)
{
	return NewMat4(
		2.0f / (right-left)       , 0.0f                      , 0.0f                      , 0.0f,
		0.0f                      , 2.0f / (top-bottom)       , 0.0f                      , 0.0f,
		0.0f                      , 0.0f                      , -2.0f / (zFar-zNear)      , 0.0f,
		-(right+left)/(right-left), -(top+bottom)/(top-bottom), -(zFar+zNear)/(zFar-zNear), 1.0f
	);
}

//TODO: Add support for quaternion conversion to matrix

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_MATRICES_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
Matrix4x4_Identity
Mat4_Identity
@Types
Matrix4x4_t
mat4
@Functions
mat4 NewMat4(r32 r0c0, r32 r0c1, r32 r0c2, r32 r0c3, r32 r1c0, r32 r1c1, r32 r1c2, r32 r1c3, r32 r2c0, r32 r2c1, r32 r2c2, r32 r2c3, r32 r3c0, r32 r3c1, r32 r3c2, r32 r3c3)
mat4 Mat4Fill(r32 all)
mat4 Mat4Diagonal(r32 r0c0, r32 r1c1, r32 r2c2, r32 r3c3, r32 other = 0.0f)
mat4 Mat4FromBasis(v3 basisX, v3 basisY, v3 basisZ)
r32 Mat4Determinant(mat4 matrix)
mat4 Mat4Transpose(mat4 matrix)
#define Mat3Determinant(a, b, c, d, e, f, g, h, i)
mat4 Mat4Cofactor(mat4 matrix)
mat4 Mat4Adjoint(mat4 matrix)
mat4 Mat4Inverse(mat4 matrix, bool* successOut = nullptr)
mat4 Mat4Multiply(mat4 left, mat4 right)
v2 Mat4MultiplyVec2(mat4 matrix, v2 vector, bool includeTranslation = true)
v3 Mat4MultiplyVec3(mat4 matrix, v3 vector, bool includeTranslation = true, r32* wOut = nullptr)
v3 Mat4MultiplyRightVec3(v3 vector, mat4 matrix, r32* wOut = nullptr)
v4 Mat4MultiplyVec4(mat4 matrix, v4 vector)
#define Mat4ApplyLeft(matrix, transformation)
#define Mat4Transform(matrix, transformation)
mat4 Mat4Translate3(r32 x, r32 y, r32 z)
mat4 Mat4Translate2(r32 x, r32 y)
mat4 Mat4TranslateX(r32 x)
mat4 Mat4TranslateY(r32 y)
mat4 Mat4TranslateZ(r32 z)
mat4 Mat4ScaleX(r32 scale)
mat4 Mat4ScaleY(r32 scale)
mat4 Mat4ScaleZ(r32 scale)
mat4 Mat4ScaleW(r32 scale)
mat4 Mat4Scale3(r32 x, r32 y, r32 z)
mat4 Mat4Scale2(r32 x, r32 y)
mat4 Mat4RotateX(r32 angle)
mat4 Mat4RotateY(r32 angle)
mat4 Mat4RotateZ(r32 angle)
mat4 Mat4Rotate(v3 axis, r32 angle)
mat4 Mat4LookAt(v3 position, v3 lookAt, v3 upVector)
mat4 Mat4Perspective(r32 fovy, r32 aspectRatio, r32 zNear, r32 zFar)
mat4 Mat4Orthographic(r32 left, r32 right, r32 top, r32 bottom, r32 zNear, r32 zFar)
*/
