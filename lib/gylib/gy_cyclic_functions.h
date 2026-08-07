/*
File:   gy_cyclic_functions.h
Author: Taylor Robbins
Date:   09\05\2022
Description:
	** Cyclic Functions are functions like Sin and Cos which repeat on a regular basis
	** A CyclicFunc_t is a combination of an enum to define which function is used and
	** 4 constant floats that can scale and translate that function.
	** Because we often want to use 2 cyclic functions together, CyclicFunc2D_t is a way
	** to combine 2 cyclic functions of the same type with separate parameters.
	** This allows the "custom" variant to get both parameters at once, so we can do
	** stuff like simplex noise and still fit in the CyclicFunc2D_t mold
*/

#ifndef _GY_CYCLIC_FUNCTIONS_H
#define _GY_CYCLIC_FUNCTIONS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"

// +--------------------------------------------------------------+
// |                      Enum and Structure                      |
// +--------------------------------------------------------------+
#define CYCLIC_FUNC_CALLBACK_DEF(functionName) r32 functionName(struct CyclicFunc_t func, r32 t)
typedef CYCLIC_FUNC_CALLBACK_DEF(CyclicFuncCallback_f);
#define CYCLIC_FUNC2D_CALLBACK_DEF(functionName) r32 functionName(struct CyclicFunc2D_t func, v2 t)
typedef CYCLIC_FUNC2D_CALLBACK_DEF(CyclicFunc2DCallback_f);

enum CyclicFuncType_t
{
	CyclicFuncType_None = 0,
	CyclicFuncType_Custom,
	CyclicFuncType_Constant,
	CyclicFuncType_Sine,
	CyclicFuncType_Cosine,
	CyclicFuncType_Tangent,
	CyclicFuncType_Saw,
	CyclicFuncType_Square,
	CyclicFuncType_NumTypes,
};

#ifdef GYLIB_HEADER_ONLY
const char* GetCyclicFuncTypeStr(CyclicFuncType_t funcType);
#else
const char* GetCyclicFuncTypeStr(CyclicFuncType_t funcType)
{
	switch (funcType)
	{
		case CyclicFuncType_None:     return "None";
		case CyclicFuncType_Custom:   return "Custom";
		case CyclicFuncType_Constant: return "Constant";
		case CyclicFuncType_Sine:     return "Sine";
		case CyclicFuncType_Cosine:   return "Cosine";
		case CyclicFuncType_Tangent:  return "Tangent";
		case CyclicFuncType_Saw:      return "Saw";
		case CyclicFuncType_Square:   return "Square";
		default: return "Unknown";
	}
}
#endif

struct CyclicFunc_t
{
	CyclicFuncType_t type;
	CyclicFuncCallback_f* callback;
	union { r32 period;    r32 scaleX; };
	union { r32 amplitude; r32 scaleY; }; //this is technically double the amplitude because things like sine go from +1 to -1 so the implicit amplitude is 2
	union { r32 offset;    r32 translateX; };
	union { r32 constant;  r32 translateY; };
};

struct CyclicFunc2D_t
{
	CyclicFuncType_t type;
	CyclicFunc2DCallback_f* callback;
	union { v2 period;    v2 scaleX;     };
	union { v2 amplitude; v2 scaleY;     }; //this is technically double the amplitude because things like sine go from +1 to -1 so the implicit amplitude is 2
	union { v2 offset;    v2 translateX; };
	union { v2 constant;  v2 translateY; };
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define CyclicFunc_Default       NewCyclicFunc(CyclicFuncType_Constant)
#define CyclicFunc_NormalSine    NewCyclicFunc(CyclicFuncType_Sine)
#define CyclicFunc_NormalCosine  NewCyclicFunc(CyclicFuncType_Cosine)
#define CyclicFunc_NormalTangent NewCyclicFunc(CyclicFuncType_Tangent)
#define CyclicFunc_NormalSaw     NewCyclicFunc(CyclicFuncType_Saw)
#define CyclicFunc_NormalSquare  NewCyclicFunc(CyclicFuncType_Square)

#define CyclicFunc2D_Default       NewCyclicFunc2D(CyclicFuncType_Constant)
#define CyclicFunc2D_NormalSine    NewCyclicFunc2D(CyclicFuncType_Sine)
#define CyclicFunc2D_NormalCosine  NewCyclicFunc2D(CyclicFuncType_Cosine)
#define CyclicFunc2D_NormalTangent NewCyclicFunc2D(CyclicFuncType_Tangent)
#define CyclicFunc2D_NormalSaw     NewCyclicFunc2D(CyclicFuncType_Saw)
#define CyclicFunc2D_NormalSquare  NewCyclicFunc2D(CyclicFuncType_Square)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	CyclicFunc_t NewCyclicFunc(CyclicFuncType_t type, r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f);
	CyclicFunc_t NewCyclicFunc(CyclicFuncType_t type, v4 values);
	CyclicFunc_t NewCyclicFuncCustom(CyclicFuncCallback_f* callback);
	CyclicFunc_t NewCyclicFuncConstant(r32 constant = 0.0f);
	CyclicFunc_t NewCyclicFuncSine(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f);
	CyclicFunc_t NewCyclicFuncCosine(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f);
	CyclicFunc_t NewCyclicFuncTangent(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f);
	CyclicFunc_t NewCyclicFuncSaw(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f);
	CyclicFunc_t NewCyclicFuncSquare(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f);
	CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period, v2 amplitude, v2 offset, v2 constant);
	CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period, v2 amplitude, v2 offset);
	CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period, v2 amplitude);
	CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period);
	CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type);
	CyclicFunc2D_t NewCyclicFunc2DCustom(CyclicFunc2DCallback_f* callback);
	CyclicFunc2D_t NewCyclicFunc2DConstant(v2 constant);
	CyclicFunc2D_t NewCyclicFunc2DSine(v2 period, v2 amplitude, v2 offset, v2 constant);
	CyclicFunc2D_t NewCyclicFunc2DCosine(v2 period, v2 amplitude, v2 offset, v2 constant);
	CyclicFunc2D_t NewCyclicFunc2DTangent(v2 period, v2 amplitude, v2 offset, v2 constant);
	CyclicFunc2D_t NewCyclicFunc2DSaw(v2 period, v2 amplitude, v2 offset, v2 constant);
	CyclicFunc2D_t NewCyclicFunc2DSquare(v2 period, v2 amplitude, v2 offset, v2 constant);
	r32 CyclicFuncGetValue(CyclicFunc_t func, r32 t);
	r32 CyclicFunc2DGetValue(CyclicFunc2D_t func, v2 t);
#else

// +--------------------------------------------------------------+
// |                             New                              |
// +--------------------------------------------------------------+
CyclicFunc_t NewCyclicFunc(CyclicFuncType_t type, r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
{
	CyclicFunc_t result;
	result.type      = type;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc_t NewCyclicFunc(CyclicFuncType_t type, v4 values)
{
	CyclicFunc_t result;
	result.type      = type;
	result.period    = values.x;
	result.amplitude = values.y;
	result.offset    = values.z;
	result.constant  = values.w;
	return result;
}

CyclicFunc_t NewCyclicFuncCustom(CyclicFuncCallback_f* callback)
{
	CyclicFunc_t result;
	result.type      = CyclicFuncType_Custom;
	result.callback  = callback;
	result.period    = 1.0f;
	result.amplitude = 1.0f;
	result.offset    = 0.0f;
	result.constant  = 0.0f;
	return result;
}
CyclicFunc_t NewCyclicFuncConstant(r32 constant = 0.0f)
{
	CyclicFunc_t result;
	result.type = CyclicFuncType_Constant;
	result.period    = 1.0f;
	result.amplitude = 1.0f;
	result.offset    = 0.0f;
	result.constant  = constant;
	return result;
}
CyclicFunc_t NewCyclicFuncSine(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
{
	CyclicFunc_t result;
	result.type = CyclicFuncType_Sine;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc_t NewCyclicFuncCosine(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
{
	CyclicFunc_t result;
	result.type = CyclicFuncType_Cosine;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc_t NewCyclicFuncTangent(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
{
	CyclicFunc_t result;
	result.type = CyclicFuncType_Tangent;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc_t NewCyclicFuncSaw(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
{
	CyclicFunc_t result;
	result.type = CyclicFuncType_Saw;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc_t NewCyclicFuncSquare(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
{
	CyclicFunc_t result;
	result.type = CyclicFuncType_Square;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}

CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period, v2 amplitude, v2 offset, v2 constant)
{
	CyclicFunc2D_t result;
	result.type      = type;
	result.callback  = nullptr;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period, v2 amplitude, v2 offset)
{
	return NewCyclicFunc2D(type, period, amplitude, offset, NewVec2(0.0f, 0.0f));
}
CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period, v2 amplitude)
{
	return NewCyclicFunc2D(type, period, amplitude, NewVec2(0.0f, 0.0f), NewVec2(0.0f, 0.0f));
}
CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period)
{
	return NewCyclicFunc2D(type, period, NewVec2(1.0f, 1.0f), NewVec2(0.0f, 0.0f), NewVec2(0.0f, 0.0f));
}
CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type)
{
	return NewCyclicFunc2D(type, NewVec2(1.0f, 1.0f), NewVec2(1.0f, 1.0f), NewVec2(0.0f, 0.0f), NewVec2(0.0f, 0.0f));
}

CyclicFunc2D_t NewCyclicFunc2DCustom(CyclicFunc2DCallback_f* callback)
{
	CyclicFunc2D_t result;
	result.type      = CyclicFuncType_Custom;
	result.callback  = callback;
	result.period    = Vec2_One;
	result.amplitude = Vec2_One;
	result.offset    = Vec2_Zero;
	result.constant  = Vec2_Zero;
	return result;
}
CyclicFunc2D_t NewCyclicFunc2DConstant(v2 constant)
{
	CyclicFunc2D_t result;
	result.type = CyclicFuncType_Constant;
	result.callback = nullptr;
	result.period    = Vec2_One;
	result.amplitude = Vec2_One;
	result.offset    = Vec2_Zero;
	result.constant  = constant;
	return result;
}
CyclicFunc2D_t NewCyclicFunc2DSine(v2 period, v2 amplitude, v2 offset, v2 constant)
{
	CyclicFunc2D_t result;
	result.type = CyclicFuncType_Sine;
	result.callback = nullptr;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc2D_t NewCyclicFunc2DCosine(v2 period, v2 amplitude, v2 offset, v2 constant)
{
	CyclicFunc2D_t result;
	result.type = CyclicFuncType_Cosine;
	result.callback = nullptr;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc2D_t NewCyclicFunc2DTangent(v2 period, v2 amplitude, v2 offset, v2 constant)
{
	CyclicFunc2D_t result;
	result.type = CyclicFuncType_Tangent;
	result.callback = nullptr;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc2D_t NewCyclicFunc2DSaw(v2 period, v2 amplitude, v2 offset, v2 constant)
{
	CyclicFunc2D_t result;
	result.type = CyclicFuncType_Saw;
	result.callback = nullptr;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}
CyclicFunc2D_t NewCyclicFunc2DSquare(v2 period, v2 amplitude, v2 offset, v2 constant)
{
	CyclicFunc2D_t result;
	result.type = CyclicFuncType_Square;
	result.callback = nullptr;
	result.period    = period;
	result.amplitude = amplitude;
	result.offset    = offset;
	result.constant  = constant;
	return result;
}

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
r32 CyclicFuncGetValue(CyclicFunc_t func, r32 t)
{
	switch (func.type)
	{
		case CyclicFuncType_Custom:
		{
			NotNull(func.callback);
			return func.callback(func, t);
		}
		case CyclicFuncType_Constant:
		{
			return func.constant;
		}
		case CyclicFuncType_Sine:
		{
			return (func.amplitude * SinR32(((t - func.offset) * TwoPi32) / func.period)) + func.constant;
		}
		case CyclicFuncType_Cosine:
		{
			return (func.amplitude * CosR32(((t - func.offset) * TwoPi32) / func.period)) + func.constant;
		}
		case CyclicFuncType_Tangent:
		{
			return (func.amplitude * TanR32(((t - func.offset) * Pi32) / func.period)) + func.constant;
		}
		case CyclicFuncType_Saw:
		{
			r32 fractionalPart = ModR32(AbsR32(t - func.offset), func.period) / AbsR32(func.period);
			return (func.amplitude * (4 * AbsR32(fractionalPart - 0.5f) - 1.0f)) + func.constant;
		}
		case CyclicFuncType_Square:
		{
			r32 fractionalPart = ModR32(AbsR32(t - func.offset), func.period) / AbsR32(func.period);
			return ((fractionalPart >= 0.5f ? 1.0f : -1.0f) * ((t - func.offset < 0.0f) ? -1.0f : 1.0f) * func.amplitude) + func.constant;
		}
		default: AssertMsg(false, "Unimplmented CyclicFuncType_t passed to CyclicFuncGetValue!"); return 0.0f;
	}
}

r32 CyclicFunc2DGetValue(CyclicFunc2D_t func, v2 t)
{
	if (func.type == CyclicFuncType_Custom)
	{
		NotNull(func.callback);
		return func.callback(func, t);
	}
	else
	{
		CyclicFunc_t xFunc = NewCyclicFunc(func.type, func.period.x, func.amplitude.x, func.offset.x, func.constant.x);
		CyclicFunc_t yFunc = NewCyclicFunc(func.type, func.period.y, func.amplitude.y, func.offset.y, func.constant.y);
		return CyclicFuncGetValue(xFunc, t.x) + CyclicFuncGetValue(yFunc, t.y);
	}
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_CYCLIC_FUNCTIONS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
CyclicFuncType_None
CyclicFuncType_Custom
CyclicFuncType_Constant
CyclicFuncType_Sine
CyclicFuncType_Cosine
CyclicFuncType_Tangent
CyclicFuncType_Saw
CyclicFuncType_Square
CyclicFuncType_NumTypes
CyclicFunc_Default
CyclicFunc_NormalSine
CyclicFunc_NormalCosine
CyclicFunc_NormalTangent
CyclicFunc_NormalSaw
CyclicFunc_NormalSquare
CyclicFunc2D_Default
CyclicFunc2D_NormalSine
CyclicFunc2D_NormalCosine
CyclicFunc2D_NormalTangent
CyclicFunc2D_NormalSaw
CyclicFunc2D_NormalSquare
@Types
CyclicFuncCallback_f
CyclicFunc2DCallback_f
CyclicFuncType_t
CyclicFunc_t
CyclicFunc2D_t
@Functions
r32 CYCLIC_FUNC_CALLBACK_DEF(CyclicFunc_t func, r32 t)
r32 CYCLIC_FUNC2D_CALLBACK_DEF(CyclicFunc2D_t func, v2 t)
const char* GetCyclicFuncTypeStr(CyclicFuncType_t funcType)
CyclicFunc_t NewCyclicFunc(CyclicFuncType_t type, r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
CyclicFunc_t NewCyclicFuncCustom(CyclicFuncCallback_f* callback)
CyclicFunc_t NewCyclicFuncConstant(r32 constant = 0.0f)
CyclicFunc_t NewCyclicFuncSine(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
CyclicFunc_t NewCyclicFuncCosine(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
CyclicFunc_t NewCyclicFuncTangent(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
CyclicFunc_t NewCyclicFuncSaw(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
CyclicFunc_t NewCyclicFuncSquare(r32 period = 1.0f, r32 amplitude = 1.0f, r32 offset = 0.0f, r32 constant = 0.0f)
CyclicFunc2D_t NewCyclicFunc2D(CyclicFuncType_t type, v2 period, v2 amplitude, v2 offset, v2 constant)
CyclicFunc2D_t NewCyclicFunc2DCustom(CyclicFunc2DCallback_f* callback)
CyclicFunc2D_t NewCyclicFunc2DConstant(v2 constant)
CyclicFunc2D_t NewCyclicFunc2DSine(v2 period, v2 amplitude, v2 offset, v2 constant)
CyclicFunc2D_t NewCyclicFunc2DCosine(v2 period, v2 amplitude, v2 offset, v2 constant)
CyclicFunc2D_t NewCyclicFunc2DTangent(v2 period, v2 amplitude, v2 offset, v2 constant)
CyclicFunc2D_t NewCyclicFunc2DSaw(v2 period, v2 amplitude, v2 offset, v2 constant)
CyclicFunc2D_t NewCyclicFunc2DSquare(v2 period, v2 amplitude, v2 offset, v2 constant)
r32 CyclicFuncGetValue(CyclicFunc_t func, r32 t)
r32 CyclicFunc2DGetValue(CyclicFunc2D_t func, v2 t)
*/
