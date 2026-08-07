/*
File:   gy_rays.h
Author: Taylor Robbins
Date:   10\09\2021
Description:
	** Holds Ray2D_t, Ray3D_t, Frustum_t and a bunch of functions that help us test collision and intersection with these primitives and others
*/

#ifndef _GY_RAYS_H
#define _GY_RAYS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct Ray2_t
{
	union
	{
		v2 origin;
		struct { r32 x, y, z; };
	};
	union
	{
		v2 direction;
		struct { r32 dX, dY, dZ; };
	};
};
struct Ray3_t
{
	union
	{
		v3 origin;
		struct { r32 x, y, z; };
	};
	union
	{
		v3 direction;
		struct { r32 dX, dY, dZ; };
	};
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	Ray2_t NewRay2(v2 origin, v2 direction);
	Ray2_t NewRay2(v2 origin, r32 angle);
	Ray3_t NewRay3(v3 origin, v3 direction);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Ray2_t NewRay2(v2 origin, v2 direction)
{
	Ray2_t result;
	result.origin = origin;
	result.direction = direction;
	return result;
}
Ray2_t NewRay2(v2 origin, r32 angle)
{
	Ray2_t result;
	result.origin = origin;
	result.direction = NewVec2(CosR32(angle), SinR32(angle));
	return result;
}
Ray3_t NewRay3(v3 origin, v3 direction)
{
	Ray3_t result;
	result.origin = origin;
	result.direction = direction;
	return result;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_RAYS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
Ray2_t
Ray3_t
@Functions
Ray2_t NewRay2(v2 origin, v2 direction)
Ray3_t NewRay3(v3 origin, v3 direction)
*/
