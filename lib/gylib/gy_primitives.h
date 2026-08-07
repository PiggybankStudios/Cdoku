/*
File:   gy_primitives.h
Author: Taylor Robbins
Date:   10\09\2021
Description:
	** Contains a bunch of 3D, 2D (and even 1D) primitive structures and related functions
*/

#ifndef _GY_PRIMITIVES_H
#define _GY_PRIMITIVES_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"
#include "gy_rectangles.h"
#include "gy_rays.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
//Face Indices: top=0 right=1 front=2 left=3 back=4 bottom=5 (top face upward is forward, bottom face upward is backward, clockwise faces)
#define BOX_PRIMITIVE_TOP_FACE_INDEX      0
#define BOX_PRIMITIVE_RIGHT_FACE_INDEX    1
#define BOX_PRIMITIVE_FRONT_FACE_INDEX    2
#define BOX_PRIMITIVE_LEFT_FACE_INDEX     3
#define BOX_PRIMITIVE_BACK_FACE_INDEX     4
#define BOX_PRIMITIVE_BOTTOM_FACE_INDEX   5
#define BOX_PRIMITIVE_INDICES_PER_FACE    6

union PrimitiveVert3D_t
{
	v3 position;
	struct { r32 x, y, z; };
};
struct PrimitiveIndex3D_t
{
	u64 index;
	u64 faceIndex;
	union
	{
		v3 normal;
		struct { r32 nX, nY, nZ; };
	};
	union
	{
		v2 texCoord;
		struct { r32 tX, tY, tZ; };
	};
};
struct PrimitiveIndexedVerts_t
{
	MemArena_t* allocArena;
	u64 numVertices;
	PrimitiveVert3D_t* vertices;
	u64 numIndices;
	PrimitiveIndex3D_t* indices;
	u64 numFaces;
};

struct Frustum_t
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
	union
	{
		v3 upVector;
		struct { r32 upX, upY, upZ; };
	};
	union
	{
		v2 fov;
		struct { r32 fovX, fovY; };
	};
	r32 zNear;
	r32 zFar;
};

struct Plane_t
{
	union
	{
		v3 normal;
		struct { r32 nX, nY, nZ; };
	};
	r32 distance;
};

union Triangle2D_t
{
	v2 vertices[3];
	r32 valuesR32[3*2];
	struct { v2 vert0, vert1, vert2; };
};

union Triangle3D_t
{
	v3 vertices[3];
	r32 valuesR32[3*3];
	struct { v3 vert0, vert1, vert2; };
};

union Simplex_t
{
	v3 vertices[4];
	r32 valuesR32[4*3];
	struct { v3 vert0, vert1, vert2, vert3; };
};

struct Circle_t
{
	union
	{
		v2 center;
		struct { r32 x, y; };
	};
	r32 radius;
};

struct Sphere_t
{
	union
	{
		v3 center;
		struct { r32 x, y, z; };
	};
	r32 radius;
};

//NOTE: All of the following primitives support translation information only. Rotation information should be encoded alongside these if needed
struct Cylinder_t
{
	union
	{
		v3 base;
		struct { r32 x, y, z; };
	};
	r32 radius;
	r32 height;
};

struct Cone_t
{
	union
	{
		v3 base;
		struct { r32 x, y, z; };
	};
	r32 height;
	r32 radius;
};

struct Pyramid_t
{
	union
	{
		v3 base;
		struct { r32 x, y, z; };
	};
	r32 height;
	union
	{
		v2 baseSize;
		struct { r32 width, depth; };
	};
};

union Wedge_t
{
	box bounds;
	struct
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
	};
};

//      0,'. Tip       |
//     ,' | `.         | Height
//   ,'   |Rad`.       |
//4,'     |ius  `.1    |
// \             / +x  |
//  \           /      |
//   \         /       |
//   3\_______/2       |
//        +y (Base)
#define PENTAGON_NUM_EDGES        5
#define PENTAGON_NUM_VERTICES     5
#define PENTAGON_ANGLE_STEP32     ToRadians32(72.0f)
#define PENTAGON_ANGLE_STEP64     ToRadians64(72.0)
#define PENTAGON_INTERNAL_ANGLE32 ToRadians32(108.0f)
#define PENTAGON_INTERNAL_ANGLE64 ToRadians64(108.0)
struct Pentagon_t
{
	v2 center;
	r32 sideLength;
	r32 rotation;
};

//          +y             Top is face 0
//      _,--"^"--._        Faces adjacent to top are faces 1-5 (starting at front, going cw)
//    ,'\   Top   /`.      Every other face but bottom are faces 6-10 (starting down right from front, going cw)
//  ,'   \_______/   `.    Bottom is face 11
// | [2] /       \ [5] |   Vertices go cw with top face starting at tip (5)
// |    /         \    |   then all middle vertices cw starting with one adjacent to top tip (10)
// |  _/   Front   \_  |   then bottom face going cw (ccw when looking at bottom) starting with it's tip (5)
// \'' `-.  [1]  ,-' ``/ +x
//  \     `-._,-'     /    Dodecahedron has 12 faces, 20 vertices, pentagons for faces, 3 faces per vertex
//   \ [7]   |   [6] /
//    `-.._  |  _,,-'
//         ``"''
#define DODECAHEDRON_NUM_FACES    12
#define DODECAHEDRON_NUM_VERTICES 20
#define DODECAHEDRON_NUM_EDGES    30
// AcosR32(-SqrtR32(5) / 5.0f) or 116.56505117707798935157219372045329467120421429964522102798601631... degrees or (2.0344439357957027354455779231009658441271217539736731742984053848... radians)
#define DODECAHEDRON_DIHEDRAL_ANGLE32 2.03444385528564453125f         //radians
#define DODECAHEDRON_DIHEDRAL_ANGLE64 2.03444393579570270702561174403 //radians
struct Dodecahedron_t
{
	v3 center;
	r32 sideLength;
	quat rotation;
};
typedef Dodecahedron_t Dodec_t;

//Hexagon radius and sideLength are equal
//    4 +-----+ 5
//     /       \
//    /         \
// 3 +     --R---+ 0
//    \         /
//     \   R   /
//    2 +-----+ 1
#define HEXAGON_NUM_VERTICES        6
#define HEXAGON_NUM_EDGES           6
#define HEXAGON_INNER_ANGLE_DEGREES 60 //degress
#define HEXAGON_INNER_ANGLE32       ToRadians32(HEXAGON_INNER_ANGLE_DEGREES)
#define HEXAGON_INNER_ANGLE64       ToRadians64(HEXAGON_INNER_ANGLE_DEGREES)
struct Hexagon_t
{
	v2 center;
	union
	{
		r32 radius;
		r32 sideLength;
	};
	r32 rotation;
};

struct Range_t
{
	r32 min;
	r32 max;
};
struct Rangei_t
{
	i32 min;
	i32 max;
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define Range_Empty NewRange(0, 0)
#define Range_One   NewRange(0, 1)

#define Rangei_Empty NewRangei(0, 0)
#define Rangei_One   NewRangei(0, 1)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	PrimitiveIndex3D_t NewPrimitiveIndex3D(u64 index, u64 faceIndex, v3 normal, v2 texCoord);
	Frustum_t NewFrustum(v3 origin, v3 direction, v3 upVector, v2 fov, r32 zNear, r32 zFar);
	Plane_t NewPlane(v3 normal, r32 distance);
	Plane_t NewPlane(r32 nX, r32 nY, r32 nZ, r32 distance);
	Triangle2D_t NewTriangle2D(v2 vert0, v2 vert1, v2 vert2);
	Triangle3D_t NewTriangle3D(v3 vert0, v3 vert1, v3 vert2);
	Simplex_t NewSimplex(v3 vert0, v3 vert1, v3 vert2, v3 vert3);
	Circle_t NewCircle(v2 center, r32 radius);
	Circle_t NewCircle(r32 x, r32 y, r32 radius);
	Sphere_t NewSphere(v3 center, r32 radius);
	Sphere_t NewSphere(r32 x, r32 y, r32 z, r32 radius);
	Cylinder_t NewCylinder(v3 base, r32 height, r32 radius);
	Cylinder_t NewCylinder(r32 x, r32 y, r32 z, r32 height, r32 radius);
	Cone_t NewCone(v3 base, r32 height, r32 radius);
	Cone_t NewCone(r32 x, r32 y, r32 z, r32 height, r32 radius);
	Pyramid_t NewPyramid(v3 base, r32 height, v2 baseSize);
	Pyramid_t NewPyramid(r32 x, r32 y, r32 z, r32 height, v2 baseSize);
	Pyramid_t NewPyramid(r32 x, r32 y, r32 z, r32 height, r32 baseWidth, r32 baseDepth);
	Pyramid_t NewPyramid(v3 base, r32 height, r32 baseWidth, r32 baseDepth);
	Wedge_t NewWedge(box bounds);
	Wedge_t NewWedge(v3 bottomLeft, v3 size);
	Wedge_t NewWedge(v3 bottomLeft, r32 width, r32 height, r32 depth);
	Wedge_t NewWedge(r32 x, r32 y, r32 z, v3 size);
	Wedge_t NewWedge(r32 x, r32 y, r32 z, r32 width, r32 height, r32 depth);
	Pentagon_t NewPentagon(v2 center, r32 sideLength, r32 rotation = 0.0f);
	Dodec_t NewDodec(v3 center, r32 sideLength, quat rotation);
	Hexagon_t NewHexagon(v2 center, r32 radiusOrSideLength, r32 rotation = 0.0f);
	Range_t NewRange(r32 min, r32 max);
	Range_t NewEmptyRange(r32 value);
	Rangei_t NewRangei(i32 min, i32 max);
	Rangei_t NewEmptyRangei(i32 value);
	r32 PentagonGetHeight(r32 sideLength);
	r32 PentagonGetHeight(Pentagon_t pentagon);
	r32 PentagonGetRadius(r32 sideLength);
	r32 PentagonGetRadius(Pentagon_t pentagon);
	v2 PentagonGetVertex(Pentagon_t pentagon, u64 index);
	v2 PentagonGetEdgeCenter(Pentagon_t pentagon, u64 index);
	v2 PentagonGetDrawCenter(Pentagon_t pentagon);
	r32 DodecGetFaceRadius(Dodec_t dodec);
	r32 DodecGetVertexRadius(Dodec_t dodec);
	v3 DodecGetVertex(Dodec_t dodec, u64 index);
	u8 GetDiceValueForDodecFace(u64 faceIndex);
	v2 GetHexagonVertex(Hexagon_t hexagon, u64 index);
	void RangeFix(Range_t* range);
	Range_t RangeShift(Range_t range, r32 shiftAmount);
	Range_t RangeScale(Range_t range, r32 scalar);
	Range_t RangeBoth(Range_t range1, Range_t range2);
	bool DoesRangeContain(Range_t range, r32 value, bool inclusiveMin = true, bool inclusiveMax = true);
	bool DoRangesOverlap(Range_t range1, Range_t range2, bool inclusive = true);
	Range_t RangeOverlap(Range_t range1, Range_t range2);
	r32 InverseLerpRange(Range_t range, r32 value, bool clamp = false);
	void RangeiFix(Rangei_t* range);
	Rangei_t RangeiShift(Rangei_t range, i32 shiftAmount);
	Rangei_t RangeiScale(Rangei_t range, i32 scalar);
	Rangei_t RangeiBoth(Rangei_t range1, Rangei_t range2);
	bool DoesRangeiContain(Rangei_t range, i32 value, bool inclusiveMin = true, bool inclusiveMax = true);
	bool DoRangeisOverlap(Rangei_t range1, Rangei_t range2, bool inclusive = true);
	Rangei_t RangeiOverlap(Rangei_t range1, Rangei_t range2);
	void FreePrimitiveIndexedVerts(PrimitiveIndexedVerts_t* indexedVerts);
	void InvertPrimitiveVerts(PrimitiveIndexedVerts_t* indexedVerts);
	PrimitiveIndexedVerts_t GenerateVertsForBox(Box_t boundingBox, MemArena_t* memArena);
	PrimitiveIndexedVerts_t GenerateVertsForSphere(Sphere_t sphere, u64 numRings, u64 numSegments, bool smoothShading, MemArena_t* memArena);
	PrimitiveIndexedVerts_t GenerateVertsForDodecohedron(Dodec_t dodec, MemArena_t* memArena);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
PrimitiveIndex3D_t NewPrimitiveIndex3D(u64 index, u64 faceIndex, v3 normal, v2 texCoord)
{
	PrimitiveIndex3D_t result;
	result.index = index;
	result.faceIndex = faceIndex;
	result.normal = normal;
	result.texCoord = texCoord;
	return result;
}
Frustum_t NewFrustum(v3 origin, v3 direction, v3 upVector, v2 fov, r32 zNear, r32 zFar)
{
	Frustum_t result;
	result.origin = origin;
	result.direction = Vec3Normalize(direction);
	result.upVector = Vec3Normalize(upVector);
	result.fov = fov;
	result.zNear = zNear;
	result.zFar = zFar;
	return result;
}
Plane_t NewPlane(v3 normal, r32 distance)
{
	Plane_t result;
	result.normal = normal;
	result.distance = distance;
	return result;
}
Plane_t NewPlane(r32 nX, r32 nY, r32 nZ, r32 distance)
{
	Plane_t result;
	result.nX = nX;
	result.nY = nY;
	result.nZ = nZ;
	result.distance = distance;
	return result;
}
Triangle2D_t NewTriangle2D(v2 vert0, v2 vert1, v2 vert2)
{
	Triangle2D_t result;
	result.vert0 = vert0;
	result.vert1 = vert1;
	result.vert2 = vert2;
	return result;
}
Triangle3D_t NewTriangle3D(v3 vert0, v3 vert1, v3 vert2)
{
	Triangle3D_t result;
	result.vert0 = vert0;
	result.vert1 = vert1;
	result.vert2 = vert2;
	return result;
}
Simplex_t NewSimplex(v3 vert0, v3 vert1, v3 vert2, v3 vert3)
{
	Simplex_t result;
	result.vert0 = vert0;
	result.vert1 = vert1;
	result.vert2 = vert2;
	result.vert3 = vert3;
	return result;
}
Circle_t NewCircle(v2 center, r32 radius)
{
	Circle_t result;
	result.center = center;
	result.radius = radius;
	return result;
}
Circle_t NewCircle(r32 x, r32 y, r32 radius)
{
	Circle_t result;
	result.x = x;
	result.y = y;
	result.radius = radius;
	return result;
}
Sphere_t NewSphere(v3 center, r32 radius)
{
	Sphere_t result;
	result.center = center;
	result.radius = radius;
	return result;
}
Sphere_t NewSphere(r32 x, r32 y, r32 z, r32 radius)
{
	Sphere_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.radius = radius;
	return result;
}
Cylinder_t NewCylinder(v3 base, r32 height, r32 radius)
{
	Cylinder_t result;
	result.base = base;
	result.height = height;
	result.radius = radius;
	return result;
}
Cylinder_t NewCylinder(r32 x, r32 y, r32 z, r32 height, r32 radius)
{
	Cylinder_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.height = height;
	result.radius = radius;
	return result;
}
Cone_t NewCone(v3 base, r32 height, r32 radius)
{
	Cone_t result;
	result.base = base;
	result.height = height;
	result.radius = radius;
	return result;
}
Cone_t NewCone(r32 x, r32 y, r32 z, r32 height, r32 radius)
{
	Cone_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.height = height;
	result.radius = radius;
	return result;
}
Pyramid_t NewPyramid(v3 base, r32 height, v2 baseSize)
{
	Pyramid_t result;
	result.base = base;
	result.height = height;
	result.baseSize = baseSize;
	return result;
}
Pyramid_t NewPyramid(r32 x, r32 y, r32 z, r32 height, v2 baseSize)
{
	Pyramid_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.height = height;
	result.baseSize = baseSize;
	return result;
}
Pyramid_t NewPyramid(r32 x, r32 y, r32 z, r32 height, r32 baseWidth, r32 baseDepth)
{
	Pyramid_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.height = height;
	result.width = baseWidth;
	result.depth = baseDepth;
	return result;
}
Pyramid_t NewPyramid(v3 base, r32 height, r32 baseWidth, r32 baseDepth)
{
	Pyramid_t result;
	result.base = base;
	result.height = height;
	result.width = baseWidth;
	result.depth = baseDepth;
	return result;
}
Wedge_t NewWedge(box bounds)
{
	Wedge_t result;
	result.bounds = bounds;
	return result;
}
Wedge_t NewWedge(v3 bottomLeft, v3 size)
{
	Wedge_t result;
	result.bottomLeft = bottomLeft;
	result.size = size;
	return result;
}
Wedge_t NewWedge(v3 bottomLeft, r32 width, r32 height, r32 depth)
{
	Wedge_t result;
	result.bottomLeft = bottomLeft;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
Wedge_t NewWedge(r32 x, r32 y, r32 z, v3 size)
{
	Wedge_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.size = size;
	return result;
}
Wedge_t NewWedge(r32 x, r32 y, r32 z, r32 width, r32 height, r32 depth)
{
	Wedge_t result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.width = width;
	result.height = height;
	result.depth = depth;
	return result;
}
Pentagon_t NewPentagon(v2 center, r32 sideLength, r32 rotation = 0.0f)
{
	Pentagon_t result;
	result.center = center;
	result.sideLength = sideLength;
	result.rotation = rotation;
	return result;
}
Dodec_t NewDodec(v3 center, r32 sideLength, quat rotation)
{
	Dodecahedron_t result;
	result.center = center;
	result.sideLength = sideLength;
	result.rotation = rotation;
	return result;
}
Hexagon_t NewHexagon(v2 center, r32 radiusOrSideLength, r32 rotation = 0.0f)
{
	Hexagon_t result;
	result.center = center;
	result.radius = radiusOrSideLength;
	result.rotation = rotation;
	return result;
}
Range_t NewRange(r32 min, r32 max)
{
	Range_t result;
	result.min = MinR32(min, max);
	result.max = MaxR32(min, max);
	return result;
}
Range_t NewEmptyRange(r32 value)
{
	Range_t result;
	result.min = value;
	result.max = value;
	return result;
}
Rangei_t NewRangei(i32 min, i32 max)
{
	Rangei_t result;
	result.min = MinI32(min, max);
	result.max = MaxI32(min, max);
	return result;
}
Rangei_t NewEmptyRangei(i32 value)
{
	Rangei_t result;
	result.min = value;
	result.max = value;
	return result;
}

// +--------------------------------------------------------------+
// |              Pentagon and Dodecahedron Helpers               |
// +--------------------------------------------------------------+
// https://en.wikipedia.org/wiki/Pentagon
r32 PentagonGetHeight(r32 sideLength)
{
	return sideLength * 0.5f * SqrtR32(5 + (2*SqrtR32(5))); // sideLength * 1.539
}
r32 PentagonGetHeight(Pentagon_t pentagon)
{
	return PentagonGetHeight(pentagon.sideLength);
}
r32 PentagonGetRadius(r32 sideLength)
{
	return sideLength * SqrtR32((5 + SqrtR32(5)) * 0.1f); //sideLength * 0.8507
}
r32 PentagonGetRadius(Pentagon_t pentagon)
{
	return PentagonGetRadius(pentagon.sideLength);
}
v2 PentagonGetVertex(Pentagon_t pentagon, u64 index)
{
	v2 result = Vec2_Zero;
	r32 pentRadius = PentagonGetRadius(pentagon);
	r32 vertexDirection = ToRadians32(-90 + 72*(r32)index);
	result = Vec2FromAngle(vertexDirection, pentRadius);
	result = Vec2Rotate(result, pentagon.rotation);
	result += pentagon.center;
	return result;
}
v2 PentagonGetEdgeCenter(Pentagon_t pentagon, u64 index)
{
	v2 result = Vec2_Zero;
	r32 pentRadius = PentagonGetRadius(pentagon);
	r32 vertexDirection1 = ToRadians32(-90 + 72*(r32)index);
	r32 vertexDirection2 = ToRadians32(-90 + 72*(r32)(index+1));
	v2 vert1 = Vec2FromAngle(vertexDirection1, pentRadius);
	v2 vert2 = Vec2FromAngle(vertexDirection2, pentRadius);
	result = ((vert1 + vert2) / 2.0f);
	result = Vec2Rotate(result, pentagon.rotation);
	result += pentagon.center;
	return result;
}
v2 PentagonGetDrawCenter(Pentagon_t pentagon)
{
	return pentagon.center + NewVec2(0, -PentagonGetRadius(pentagon) + PentagonGetHeight(pentagon)/2);
}

// https://en.wikipedia.org/wiki/Regular_dodecahedron
r32 DodecGetFaceRadius(Dodec_t dodec)
{
	return dodec.sideLength * 0.5f * SqrtR32(2.5f + (1.1f * SqrtR32(5))); //sideLength * 1.113516364
}
r32 DodecGetVertexRadius(Dodec_t dodec)
{
	return dodec.sideLength * (SqrtR32(3) / 4.0f) * (1 + SqrtR32(5)); //sideLength * 1.401258538
}
v3 DodecGetVertex(Dodec_t dodec, u64 index)
{
	v3 result = Vec3_Zero;
	r32 faceRadius = DodecGetFaceRadius(dodec);
	Pentagon_t pentagon = NewPentagon(Vec2_Zero, dodec.sideLength);
	
	index = (index % DODECAHEDRON_NUM_VERTICES);
	switch (index)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		{
			v2 pentVert = PentagonGetVertex(pentagon, index);
			result = NewVec3(pentVert.x, faceRadius, -pentVert.y);
		} break;
		case 5:
		case 7:
		case 9:
		case 11:
		case 13:
		{
			v2 pentEdgeCenter = Vec2Normalize(PentagonGetEdgeCenter(pentagon, (index-5) / 2));
			v3 verticalVec = Vec3_Up;
			v3 horizontalVec = NewVec3(pentEdgeCenter.x, 0, -pentEdgeCenter.y);
			v3 faceNormal = (CosR32(DODECAHEDRON_DIHEDRAL_ANGLE32 - HalfPi32) * horizontalVec) + (SinR32(DODECAHEDRON_DIHEDRAL_ANGLE32 - HalfPi32) * verticalVec);
			v3 faceTangent = Vec3Cross(faceNormal, verticalVec);
			v3 faceUp = Vec3Cross(faceNormal, faceTangent);
			v2 pentVert = PentagonGetVertex(pentagon, 1);
			result = (faceRadius * faceNormal) + (pentVert.x * faceTangent) + (-pentVert.y * faceUp);
		} break;
		case 6:
		case 8:
		case 10:
		case 12:
		case 14:
		{
			v2 pentEdgeCenter = Vec2Normalize(PentagonGetEdgeCenter(pentagon, (index-6) / 2));
			v3 verticalVec = Vec3_Up;
			v3 horizontalVec = NewVec3(pentEdgeCenter.x, 0, -pentEdgeCenter.y);
			v3 faceNormal = (CosR32(DODECAHEDRON_DIHEDRAL_ANGLE32 - HalfPi32) * horizontalVec) + (SinR32(DODECAHEDRON_DIHEDRAL_ANGLE32 - HalfPi32) * verticalVec);
			v3 faceTangent = Vec3Cross(faceNormal, verticalVec);
			v3 faceUp = Vec3Cross(faceNormal, faceTangent);
			v2 pentVert = PentagonGetVertex(pentagon, 0);
			result = (faceRadius * faceNormal) + (pentVert.x * faceTangent) + (-pentVert.y * faceUp);
		} break;
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		{
			v2 pentVert = PentagonGetVertex(pentagon, 20-index);
			result = NewVec3(pentVert.x, -faceRadius, pentVert.y);
		} break;
		default: Assert(false); break;
	}
	
	result = Mat4MultiplyVec3(Mat4Quaternion(dodec.rotation), result, false);
	result += dodec.center;
	return result;
}
u8 GetDiceValueForDodecFace(u64 faceIndex)
{
	switch (faceIndex % DODECAHEDRON_NUM_FACES)
	{
		case 0:  return 1;
		case 1:  return 6;
		case 2:  return 5;
		case 3:  return 3;
		case 4:  return 2;
		case 5:  return 4;
		case 6:  return 10;
		case 7:  return 11;
		case 8:  return 9;
		case 9:  return 7;
		case 10: return 8;
		case 11: return 12;
		default: Assert(false); return 0;
	}
}

// +--------------------------------------------------------------+
// |                       Hexagon Helpers                        |
// +--------------------------------------------------------------+
v2 GetHexagonVertex(Hexagon_t hexagon, u64 index)
{
	v2 result = Vec2_Zero;
	r32 vertexDirection = hexagon.rotation + ToRadians32(HEXAGON_INNER_ANGLE_DEGREES * (r32)(index % HEXAGON_NUM_VERTICES));
	result = Vec2FromAngle(vertexDirection, hexagon.radius);
	result += hexagon.center;
	return result;
}

// +--------------------------------------------------------------+
// |                        Range Helpers                         |
// +--------------------------------------------------------------+
void RangeFix(Range_t* range)
{
	if (range->min > range->max)
	{
		SWAP_VARIABLES(r32, range->min, range->max);
	}
}

Range_t RangeShift(Range_t range, r32 shiftAmount)
{
	return NewRange(range.min + shiftAmount, range.max + shiftAmount);
}
Range_t RangeScale(Range_t range, r32 scalar)
{
	return NewRange(range.min * scalar, range.max * scalar);
}

Range_t RangeBoth(Range_t range1, Range_t range2)
{
	r32 minValue = MinR32(range1.min, range1.max, range2.min, range2.max);
	r32 maxValue = MaxR32(range1.min, range1.max, range2.min, range2.max);
	return NewRange(minValue, maxValue);
}
bool DoesRangeContain(Range_t range, r32 value, bool inclusiveMin = true, bool inclusiveMax = true)
{
	return (
		(value > range.min || (inclusiveMin && value == range.min)) &&
		(value < range.max || (inclusiveMax && value == range.max))
	);
}
bool DoRangesOverlap(Range_t range1, Range_t range2, bool inclusive = true)
{
	if (range1.max < range2.min) { return false; }
	if (range2.max < range1.min) { return false; }
	if (range1.max == range2.min && !inclusive) { return false; }
	if (range2.max == range1.min && !inclusive) { return false; }
	return true;
}

Range_t RangeOverlap(Range_t range1, Range_t range2)
{
	if (range1.max < range2.min) { return NewEmptyRange((range1.max + range2.min) / 2); }
	if (range2.max < range1.min) { return NewEmptyRange((range2.max + range1.min) / 2); }
	return NewRange(
		MaxR32(range1.min, range2.min),
		MinR32(range1.max, range2.max)
	);
}

r32 InverseLerpRange(Range_t range, r32 value, bool clamp = false)
{
	return InverseLerpR32(range.min, range.max, value, clamp);
}

// +--------------------------------------------------------------+
// |                        Rangei Helpers                        |
// +--------------------------------------------------------------+
void RangeiFix(Rangei_t* range)
{
	if (range->min > range->max)
	{
		SWAP_VARIABLES(i32, range->min, range->max);
	}
}

Rangei_t RangeiShift(Rangei_t range, i32 shiftAmount)
{
	return NewRangei(range.min + shiftAmount, range.max + shiftAmount);
}
Rangei_t RangeiScale(Rangei_t range, i32 scalar)
{
	return NewRangei(range.min * scalar, range.max * scalar);
}

Rangei_t RangeiBoth(Rangei_t range1, Rangei_t range2)
{
	i32 minValue = MinI32(range1.min, range1.max, range2.min, range2.max);
	i32 maxValue = MaxI32(range1.min, range1.max, range2.min, range2.max);
	return NewRangei(minValue, maxValue);
}
bool DoesRangeiContain(Rangei_t range, i32 value, bool inclusiveMin = true, bool inclusiveMax = true)
{
	return (
		(value > range.min || (inclusiveMin && value == range.min)) &&
		(value < range.max || (inclusiveMax && value == range.max))
	);
}
bool DoRangeisOverlap(Rangei_t range1, Rangei_t range2, bool inclusive = true)
{
	if (range1.max < range2.min) { return false; }
	if (range2.max < range1.min) { return false; }
	if (range1.max == range2.min && !inclusive) { return false; }
	if (range2.max == range1.min && !inclusive) { return false; }
	return true;
}

Rangei_t RangeiOverlap(Rangei_t range1, Rangei_t range2)
{
	if (range1.max < range2.min) { return NewEmptyRangei((range1.max + range2.min) / 2); }
	if (range2.max < range1.min) { return NewEmptyRangei((range2.max + range1.min) / 2); }
	return NewRangei(
		MaxI32(range1.min, range2.min),
		MinI32(range1.max, range2.max)
	);
}

// +--------------------------------------------------------------+
// |                 Vertex Generating Functions                  |
// +--------------------------------------------------------------+
void FreePrimitiveIndexedVerts(PrimitiveIndexedVerts_t* indexedVerts)
{
	NotNull(indexedVerts);
	if (indexedVerts->vertices != nullptr)
	{
		NotNull(indexedVerts->allocArena);
		Assert(indexedVerts->numVertices > 0);
		FreeMem(indexedVerts->allocArena, indexedVerts->vertices, sizeof(PrimitiveVert3D_t) * indexedVerts->numVertices);
	}
	if (indexedVerts->indices != nullptr)
	{
		NotNull(indexedVerts->allocArena);
		Assert(indexedVerts->numIndices > 0);
		FreeMem(indexedVerts->allocArena, indexedVerts->indices, sizeof(PrimitiveIndex3D_t) * indexedVerts->numIndices);
	}
	ClearPointer(indexedVerts);
}

void InvertPrimitiveVerts(PrimitiveIndexedVerts_t* indexedVerts)
{
	NotNull(indexedVerts);
	if (indexedVerts->numVertices == 0) { return; }
	if (indexedVerts->numIndices == 0) { return; }
	NotNull2(indexedVerts->vertices, indexedVerts->indices);
	for (u64 iIndex = 0; iIndex+3 <= indexedVerts->numIndices; iIndex += 3)
	{
		PrimitiveIndex3D_t* index0 = &indexedVerts->indices[iIndex + 0];
		PrimitiveIndex3D_t* index1 = &indexedVerts->indices[iIndex + 1];
		PrimitiveIndex3D_t* index2 = &indexedVerts->indices[iIndex + 2];
		index0->normal = -index0->normal;
		index1->normal = -index1->normal;
		index2->normal = -index2->normal;
		PrimitiveIndex3D_t temp = {};
		MyMemCopy(&temp, index1, sizeof(PrimitiveIndex3D_t));
		MyMemCopy(index1, index2, sizeof(PrimitiveIndex3D_t));
		MyMemCopy(index2, &temp, sizeof(PrimitiveIndex3D_t));
	}
}

//NOTE: Passing nullptr for memArena in these functions will fill out the result struct with numVertices, numIndices, and numFaces but nothing else

// See BOX_PRIMITIVE_TOP_FACE_INDEX and similar defines above
PrimitiveIndexedVerts_t GenerateVertsForBox(Box_t boundingBox, MemArena_t* memArena)
{
	PrimitiveIndexedVerts_t result = {};
	result.numVertices = 8;
	result.numIndices = (6*2*3); //6 faces, 2 triangles/face, 3 indices/triangle = 36 indices
	result.numFaces = 6;
	if (memArena == nullptr) { return result; }
	
	result.allocArena = memArena;
	result.vertices = AllocArray(memArena, PrimitiveVert3D_t, result.numVertices);
	NotNull(result.vertices);
	result.indices = AllocArray(memArena, PrimitiveIndex3D_t, result.numIndices);
	NotNull(result.indices);
	
	const u64 blbIndex = 0; //bottomLeftBack
	const u64 brbIndex = 1; //bottomRightBack
	const u64 blfIndex = 2; //bottomLeftFront
	const u64 brfIndex = 3; //bottomRightFront
	const u64 tlbIndex = 4; //topLeftBack
	const u64 trbIndex = 5; //topRightBack
	const u64 tlfIndex = 6; //topLeftFront
	const u64 trfIndex = 7; //topRightFront
	result.vertices[blbIndex].position = NewVec3(boundingBox.x,                     boundingBox.y,                      boundingBox.z);
	result.vertices[brbIndex].position = NewVec3(boundingBox.x + boundingBox.width, boundingBox.y,                      boundingBox.z);
	result.vertices[blfIndex].position = NewVec3(boundingBox.x,                     boundingBox.y,                      boundingBox.z + boundingBox.depth);
	result.vertices[brfIndex].position = NewVec3(boundingBox.x + boundingBox.width, boundingBox.y,                      boundingBox.z + boundingBox.depth);
	result.vertices[tlbIndex].position = NewVec3(boundingBox.x,                     boundingBox.y + boundingBox.height, boundingBox.z);
	result.vertices[trbIndex].position = NewVec3(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height, boundingBox.z);
	result.vertices[tlfIndex].position = NewVec3(boundingBox.x,                     boundingBox.y + boundingBox.height, boundingBox.z + boundingBox.depth);
	result.vertices[trfIndex].position = NewVec3(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height, boundingBox.z + boundingBox.depth);
	
	u64 iIndex = 0;
	//top face (+y) forward is up
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 0, Vec3_Up,      NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 0, Vec3_Up,      NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 0, Vec3_Up,      NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 0, Vec3_Up,      NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 0, Vec3_Up,      NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 0, Vec3_Up,      NewVec2(1, 0));
	
	//right face (+x) up is up
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 1, Vec3_Right,   NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 1, Vec3_Right,   NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 1, Vec3_Right,   NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 1, Vec3_Right,   NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 1, Vec3_Right,   NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 1, Vec3_Right,   NewVec2(1, 0));
	
	//front face (+z) up is up
	result.indices[iIndex++] = NewPrimitiveIndex3D(trfIndex, 2, Vec3_Forward, NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 2, Vec3_Forward, NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 2, Vec3_Forward, NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 2, Vec3_Forward, NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 2, Vec3_Forward, NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 2, Vec3_Forward, NewVec2(1, 0));
	
	//left face (-x) up is up
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlfIndex, 3, Vec3_Left,    NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 3, Vec3_Left,    NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 3, Vec3_Left,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 3, Vec3_Left,    NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 3, Vec3_Left,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 3, Vec3_Left,    NewVec2(1, 0));
	
	//back face (-z) up is up
	result.indices[iIndex++] = NewPrimitiveIndex3D(tlbIndex, 4, Vec3_Back,    NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 4, Vec3_Back,    NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 4, Vec3_Back,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 4, Vec3_Back,    NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 4, Vec3_Back,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(trbIndex, 4, Vec3_Back,    NewVec2(1, 0));
	
	//bottom face (-y) backward is up
	result.indices[iIndex++] = NewPrimitiveIndex3D(blbIndex, 5, Vec3_Down,    NewVec2(0, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 5, Vec3_Down,    NewVec2(1, 0));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 5, Vec3_Down,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brfIndex, 5, Vec3_Down,    NewVec2(1, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(blfIndex, 5, Vec3_Down,    NewVec2(0, 1));
	result.indices[iIndex++] = NewPrimitiveIndex3D(brbIndex, 5, Vec3_Down,    NewVec2(1, 0));
	
	Assert(iIndex == result.numIndices);
	return result;
}

PrimitiveIndexedVerts_t GenerateVertsForSphere(Sphere_t sphere, u64 numRings, u64 numSegments, bool smoothShading, MemArena_t* memArena)
{
	Assert(numRings >= 1);
	Assert(numSegments >= 3);
	
	PrimitiveIndexedVerts_t result = {};
	result.numVertices = 2; //top and bottom verts
	result.numVertices += numSegments * numRings;
	result.numIndices = numSegments * 2 * 3; //top and bottom triangles
	result.numIndices += numSegments*2 * (numRings-1) * 3;
	if (memArena == nullptr) { return result; }
	
	result.allocArena = memArena;
	result.vertices = AllocArray(memArena, PrimitiveVert3D_t, result.numVertices);
	NotNull(result.vertices);
	result.indices = AllocArray(memArena, PrimitiveIndex3D_t, result.numIndices);
	NotNull(result.indices);
	MyMemSet(result.indices, 0x00, sizeof(PrimitiveIndex3D_t) * result.numIndices); //TODO: Remove me!
	
	const u64 bottomCenterIndex = 0;
	const u64 topCenterIndex = result.numVertices-1;
	
	result.vertices[topCenterIndex].position    = NewVec3(sphere.x, sphere.y + sphere.radius, sphere.z);
	result.vertices[bottomCenterIndex].position = NewVec3(sphere.x, sphere.y - sphere.radius, sphere.z);
	
	r32 ringStep = Pi32 / (r32)(numRings+1);
	r32 segmentStep = TwoPi32 / (r32)(numSegments);
	for (u64 rIndex = 0; rIndex < numRings; rIndex++)
	{
		r32 ringAngle = -HalfPi32 + (ringStep * (rIndex+1));
		r32 ringY = sphere.y + (SinR32(ringAngle) * sphere.radius);
		r32 ringRadius = CosR32(ringAngle) * sphere.radius;
		for (u64 sIndex = 0; sIndex < numSegments; sIndex++)
		{
			r32 segmentAngle = (sIndex * segmentStep);
			u64 ringVertIndex = 1 + (rIndex * numSegments) + sIndex;
			Assert(ringVertIndex < result.numVertices);
			result.vertices[ringVertIndex].position = NewVec3(sphere.x + CosR32(segmentAngle) * ringRadius, ringY, sphere.z + SinR32(segmentAngle) * ringRadius);
		}
	}
	
	r32 texCoordStepX = 1.0f / (r32)numSegments;
	r32 texCoordStepY = 1.0f / (r32)(numRings-1);
	for (u64 rIndex = 0; rIndex <= numRings; rIndex++)
	{
		for (u64 sIndex = 0; sIndex < numSegments; sIndex++)
		{
			u64 upperVertIndex = 0;
			if (rIndex < numRings) { upperVertIndex = 1 + (rIndex * numSegments) + sIndex; }
			else { upperVertIndex = topCenterIndex; }
			Assert(upperVertIndex < result.numVertices);
			
			u64 lowerVertIndex = 0;
			if (rIndex > 0) { lowerVertIndex = 1 + ((rIndex-1) * numSegments) + sIndex; }
			else { lowerVertIndex = bottomCenterIndex; }
			Assert(lowerVertIndex < result.numVertices);
			
			u64 upperNextVertIndex = 0;
			if (rIndex < numRings)
			{
				if (sIndex+1 < numSegments) { upperNextVertIndex = 1 + (rIndex * numSegments) + (sIndex+1); }
				else { upperNextVertIndex = 1 + (rIndex * numSegments) + (0); }
			}
			else { upperNextVertIndex = topCenterIndex; }
			Assert(upperNextVertIndex < result.numVertices);
			
			u64 lowerNextVertIndex = 0;
			if (rIndex > 0)
			{
				if (sIndex+1 < numSegments) { lowerNextVertIndex = 1 + ((rIndex-1) * numSegments) + (sIndex+1); }
				else { lowerNextVertIndex = 1 + ((rIndex-1) * numSegments) + (0); }
			}
			else { lowerNextVertIndex = bottomCenterIndex; }
			Assert(lowerNextVertIndex < result.numVertices);
			
			v2 upperTexCoord = NewVec2(1.0f - (r32)sIndex     * texCoordStepX, 1.0f - (r32)rIndex     * texCoordStepY);
			v2 lowerTexCoord = NewVec2(1.0f - (r32)(sIndex+1) * texCoordStepX, 1.0f - (r32)(rIndex-1) * texCoordStepY);
			
			u64 ringBaseIndex = 0;
			if (rIndex > 0) { ringBaseIndex = (numSegments * 3) + (numSegments * (rIndex-1) * 3 * 2); }
			if (rIndex > 0 && rIndex < numRings)
			{
				u64 triIndex1 = ringBaseIndex + (sIndex * 3 * 2) + 0;
				u64 triIndex2 = ringBaseIndex + (sIndex * 3 * 2) + 3;
				Assert(triIndex1+3 <= result.numIndices);
				Assert(triIndex2+3 <= result.numIndices);
				
				result.indices[triIndex1 + 0].index = upperVertIndex;     result.indices[triIndex1 + 0].faceIndex = sIndex; result.indices[triIndex1 + 0].texCoord = NewVec2(upperTexCoord.x, upperTexCoord.y);
				result.indices[triIndex1 + 1].index = lowerVertIndex;     result.indices[triIndex1 + 2].faceIndex = sIndex; result.indices[triIndex1 + 1].texCoord = NewVec2(upperTexCoord.x, lowerTexCoord.y);
				result.indices[triIndex1 + 2].index = upperNextVertIndex; result.indices[triIndex1 + 1].faceIndex = sIndex; result.indices[triIndex1 + 2].texCoord = NewVec2(lowerTexCoord.x, upperTexCoord.y);
				
				result.indices[triIndex2 + 0].index = lowerNextVertIndex; result.indices[triIndex2 + 0].faceIndex = sIndex; result.indices[triIndex2 + 0].texCoord = NewVec2(lowerTexCoord.x, lowerTexCoord.y);
				result.indices[triIndex2 + 1].index = upperNextVertIndex; result.indices[triIndex2 + 2].faceIndex = sIndex; result.indices[triIndex2 + 1].texCoord = NewVec2(lowerTexCoord.x, upperTexCoord.y);
				result.indices[triIndex2 + 2].index = lowerVertIndex;     result.indices[triIndex2 + 1].faceIndex = sIndex; result.indices[triIndex2 + 2].texCoord = NewVec2(upperTexCoord.x, lowerTexCoord.y);
			}
			else
			{
				u64 triIndex = ringBaseIndex + (sIndex * 3);
				Assert(triIndex < result.numIndices);
				if (rIndex == 0)
				{
					result.indices[triIndex + 0].index = lowerVertIndex;     result.indices[triIndex + 0].faceIndex = sIndex; result.indices[triIndex + 0].texCoord = NewVec2(upperTexCoord.x, lowerTexCoord.y);
					result.indices[triIndex + 1].index = upperNextVertIndex; result.indices[triIndex + 1].faceIndex = sIndex; result.indices[triIndex + 1].texCoord = NewVec2(lowerTexCoord.x, upperTexCoord.y);
					result.indices[triIndex + 2].index = upperVertIndex;     result.indices[triIndex + 2].faceIndex = sIndex; result.indices[triIndex + 2].texCoord = NewVec2(upperTexCoord.x, upperTexCoord.y);
				}
				else //rIndex == numRings
				{
					result.indices[triIndex + 0].index = upperVertIndex;     result.indices[triIndex + 0].faceIndex = sIndex; result.indices[triIndex + 0].texCoord = NewVec2(upperTexCoord.x, upperTexCoord.y);
					result.indices[triIndex + 1].index = lowerVertIndex;     result.indices[triIndex + 1].faceIndex = sIndex; result.indices[triIndex + 1].texCoord = NewVec2(upperTexCoord.x, lowerTexCoord.y);
					result.indices[triIndex + 2].index = lowerNextVertIndex; result.indices[triIndex + 2].faceIndex = sIndex; result.indices[triIndex + 2].texCoord = NewVec2(lowerTexCoord.x, lowerTexCoord.y);
				}
			}
		}
	}
	
	if (smoothShading)
	{
		for (u64 iIndex = 0; iIndex < result.numIndices; iIndex++)
		{
			PrimitiveIndex3D_t* index = &result.indices[iIndex];
			index->normal = Vec3Normalize(result.vertices[index->index].position - sphere.center);
		}
	}
	else
	{
		for (u64 iIndex = 0; iIndex+3 <= result.numIndices; iIndex += 3)
		{
			PrimitiveIndex3D_t* index0 = &result.indices[iIndex + 0];
			PrimitiveIndex3D_t* index1 = &result.indices[iIndex + 1];
			PrimitiveIndex3D_t* index2 = &result.indices[iIndex + 2];
			v3 normal = Vec3Normalize(Vec3Cross(result.vertices[index2->index].position - result.vertices[index0->index].position, result.vertices[index1->index].position - result.vertices[index0->index].position));
			index0->normal = normal;
			index1->normal = normal;
			index2->normal = normal;
		}
	}
	
	return result;
}

//      1,|.5
//     ,'/ \`.
//   ,' / 7 \ `.  Triangulation of pentagon faces
//0,'   |   |   `.
// \    |   |    /3
//  \   /   \   /
//   \ |     | /
//   2\|6___8|/4
PrimitiveIndexedVerts_t GenerateVertsForDodecohedron(Dodec_t dodec, MemArena_t* memArena)
{
	PrimitiveIndexedVerts_t result = {};
	result.numVertices = DODECAHEDRON_NUM_VERTICES;
	result.numIndices = DODECAHEDRON_NUM_FACES * (3*3); //12 faces, 3 triangles per face, 3 vertices per triangle = 108
	result.numFaces = 12;
	if (memArena == nullptr) { return result; }
	
	result.allocArena = memArena;
	result.vertices = AllocArray(memArena, PrimitiveVert3D_t, result.numVertices);
	NotNull(result.vertices);
	result.indices = AllocArray(memArena, PrimitiveIndex3D_t, result.numIndices);
	NotNull(result.indices);
	
	for (u64 vIndex = 0; vIndex < DODECAHEDRON_NUM_VERTICES; vIndex++)
	{
		result.vertices[vIndex].position = DodecGetVertex(dodec, vIndex);
	}
	
	//TODO: Actually calculate these numbers. Right now they are estimates from a raster image pentagon
	r32 v0 = 0.39f;//0.41263940520446096654275092936803f;
	r32 u0 = 0.21f;//0.19921875f;
	r32 u1 = 0.79f;//0.80078125f;
	v2 texCoords[5];
	texCoords[0] = NewVec2(0.5f, 0.01f); //top
	texCoords[1] = NewVec2(1.0f, v0); //right
	texCoords[2] = NewVec2(u1, 1.0f); //bottom right
	texCoords[3] = NewVec2(u0, 1.0f); //bottom left
	texCoords[4] = NewVec2(0.0f, v0); //left
	
	for (u64 fIndex = 0; fIndex < DODECAHEDRON_NUM_FACES; fIndex++)
	{
		u64 faceIndices[5];
		switch (fIndex)
		{
			case 0:  faceIndices[0] = 0;  faceIndices[1] = 1;  faceIndices[2] = 2;  faceIndices[3] = 3;  faceIndices[4] = 4;  break; //Top Face
			
			case 1:  faceIndices[0] = 3;  faceIndices[1] = 2;  faceIndices[2] = 9;  faceIndices[3] = 10; faceIndices[4] = 11; break; //Front Face
			case 2:  faceIndices[0] = 4;  faceIndices[1] = 3;  faceIndices[2] = 11; faceIndices[3] = 12; faceIndices[4] = 13; break; //Left Face
			case 3:  faceIndices[0] = 0;  faceIndices[1] = 4;  faceIndices[2] = 13; faceIndices[3] = 14; faceIndices[4] = 5;  break;
			case 4:  faceIndices[0] = 1;  faceIndices[1] = 0;  faceIndices[2] = 5;  faceIndices[3] = 6;  faceIndices[4] = 7;  break;
			case 5:  faceIndices[0] = 2;  faceIndices[1] = 1;  faceIndices[2] = 7;  faceIndices[3] = 8;  faceIndices[4] = 9;  break; //Right Face
			
			case 6:  faceIndices[0] = 19; faceIndices[1] = 15; faceIndices[2] = 10; faceIndices[3] = 9;  faceIndices[4] = 8;  break;
			case 7:  faceIndices[0] = 15; faceIndices[1] = 16; faceIndices[2] = 12; faceIndices[3] = 11; faceIndices[4] = 10; break;
			case 8:  faceIndices[0] = 16; faceIndices[1] = 17; faceIndices[2] = 14; faceIndices[3] = 13; faceIndices[4] = 12; break;
			case 9:  faceIndices[0] = 17; faceIndices[1] = 18; faceIndices[2] = 6;  faceIndices[3] = 5;  faceIndices[4] = 14; break; //Back Face
			case 10: faceIndices[0] = 18; faceIndices[1] = 19; faceIndices[2] = 8;  faceIndices[3] = 7;  faceIndices[4] = 6;  break;
			
			case 11: faceIndices[0] = 15; faceIndices[1] = 19; faceIndices[2] = 18; faceIndices[3] = 17; faceIndices[4] = 16; break; //Bottom Face
			default: Assert(false); return result; break;
		}
		
		v3 faceNormal = Vec3Normalize(Vec3Cross(
			result.vertices[faceIndices[1]].position - result.vertices[faceIndices[0]].position,
			result.vertices[faceIndices[4]].position - result.vertices[faceIndices[0]].position
		));
		
		//Left Triangle
		result.indices[(fIndex*9) + 0] = NewPrimitiveIndex3D(faceIndices[4], fIndex, faceNormal, texCoords[4]); //left tri
		result.indices[(fIndex*9) + 1] = NewPrimitiveIndex3D(faceIndices[0], fIndex, faceNormal, texCoords[0]);
		result.indices[(fIndex*9) + 2] = NewPrimitiveIndex3D(faceIndices[3], fIndex, faceNormal, texCoords[3]);
		
		//Right Triangle
		result.indices[(fIndex*9) + 3] = NewPrimitiveIndex3D(faceIndices[1], fIndex, faceNormal, texCoords[1]); //left tri
		result.indices[(fIndex*9) + 4] = NewPrimitiveIndex3D(faceIndices[2], fIndex, faceNormal, texCoords[2]);
		result.indices[(fIndex*9) + 5] = NewPrimitiveIndex3D(faceIndices[0], fIndex, faceNormal, texCoords[0]);
		
		//Middle Triangle
		result.indices[(fIndex*9) + 6] = NewPrimitiveIndex3D(faceIndices[2], fIndex, faceNormal, texCoords[2]); //left tri
		result.indices[(fIndex*9) + 7] = NewPrimitiveIndex3D(faceIndices[3], fIndex, faceNormal, texCoords[3]);
		result.indices[(fIndex*9) + 8] = NewPrimitiveIndex3D(faceIndices[0], fIndex, faceNormal, texCoords[0]);
	}
	
	return result;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_PRIMITIVES_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
BOX_PRIMITIVE_TOP_FACE_INDEX
BOX_PRIMITIVE_RIGHT_FACE_INDEX
BOX_PRIMITIVE_FRONT_FACE_INDEX
BOX_PRIMITIVE_LEFT_FACE_INDEX
BOX_PRIMITIVE_BACK_FACE_INDEX
BOX_PRIMITIVE_BOTTOM_FACE_INDEX
BOX_PRIMITIVE_INDICES_PER_FACE
PENTAGON_NUM_EDGES
PENTAGON_NUM_VERTICES
PENTAGON_ANGLE_STEP32
PENTAGON_ANGLE_STEP64
PENTAGON_INTERNAL_ANGLE32
PENTAGON_INTERNAL_ANGLE64
DODECAHEDRON_NUM_FACES
DODECAHEDRON_NUM_VERTICES
DODECAHEDRON_NUM_EDGES
DODECAHEDRON_DIHEDRAL_ANGLE32
DODECAHEDRON_DIHEDRAL_ANGLE64
HEXAGON_NUM_VERTICES
HEXAGON_NUM_EDGES
HEXAGON_INNER_ANGLE_DEGREES
HEXAGON_INNER_ANGLE32
HEXAGON_INNER_ANGLE64
Range_Empty
Range_One
Rangei_Empty
Rangei_One
@Types
PrimitiveVert3D_t
PrimitiveIndex3D_t
PrimitiveIndexedVerts_t
Frustum_t
Plane_t
Triangle2D_t
Triangle3D_t
Simplex_t
Circle_t
Sphere_t
Cylinder_t
Cone_t
Pyramid_t
Wedge_t
Pentagon_t
Dodecahedron_t
Hexagon_t
Range_t
@Functions
PrimitiveIndex3D_t NewPrimitiveIndex3D(u64 index, u64 faceIndex, v3 normal, v2 texCoord)
Frustum_t NewFrustum(v3 origin, v3 direction, v3 upVector, v2 fov, r32 zNear, r32 zFar)
Plane_t NewPlane(v3 normal, r32 distance)
Triangle2D_t NewTriangle2D(v2 vert0, v2 vert1, v2 vert2)
Triangle3D_t NewTriangle3D(v3 vert0, v3 vert1, v3 vert2)
Simplex_t NewSimplex(v3 vert0, v3 vert1, v3 vert2, v3 vert3)
Circle_t NewCircle(v2 center, r32 radius)
Sphere_t NewSphere(v3 center, r32 radius)
Cylinder_t NewCylinder(v3 base, r32 height, r32 radius)
Cone_t NewCone(v3 base, r32 height, r32 radius)
Pyramid_t NewPyramid(v3 base, r32 height, v2 baseSize)
Wedge_t NewWedge(v3 bottomLeft, v3 size)
Pentagon_t NewPentagon(v2 center, r32 sideLength, r32 rotation = 0.0f)
Dodec_t NewDodec(v3 center, r32 sideLength, quat rotation)
Hexagon_t NewHexagon(v2 center, r32 radiusOrSideLength, r32 rotation = 0.0f)
Range_t NewRange(r32 min, r32 max)
r32 PentagonGetHeight(r32 sideLength)
r32 PentagonGetHeight(Pentagon_t pentagon)
r32 PentagonGetRadius(r32 sideLength)
r32 PentagonGetRadius(Pentagon_t pentagon)
v2 PentagonGetVertex(Pentagon_t pentagon, u64 index)
v2 PentagonGetEdgeCenter(Pentagon_t pentagon, u64 index)
v2 PentagonGetDrawCenter(Pentagon_t pentagon)
r32 DodecGetFaceRadius(Dodec_t dodec)
r32 DodecGetVertexRadius(Dodec_t dodec)
v3 DodecGetVertex(Dodec_t dodec, u64 index)
u8 GetDiceValueForDodecFace(u64 faceIndex)
v2 GetHexagonVertex(Hexagon_t hexagon, u64 index)
void RangeFix(Range_t* range)
Range_t RangeShift(Range_t range, r32 shiftAmount)
Range_t RangeScale(Range_t range, r32 scalar)
Range_t RangeBoth(Range_t range1, Range_t range2)
bool DoesRangeContain(Range_t range, r32 value, bool inclusiveMin = true, bool inclusiveMax = true)
bool DoRangesOverlap(Range_t range1, Range_t range2, bool inclusive = true)
Range_t RangeOverlap(Range_t range1, Range_t range2)
r32 InverseLerpRange(Range_t range, r32 value, bool clamp = false)
void RangeiFix(Rangei_t* range)
Rangei_t RangeiShift(Rangei_t range, i32 shiftAmount)
Rangei_t RangeiScale(Rangei_t range, i32 scalar)
Rangei_t RangeiBoth(Rangei_t range1, Rangei_t range2)
bool DoesRangeiContain(Rangei_t range, i32 value, bool inclusiveMin = true, bool inclusiveMax = true)
bool DoRangeisOverlap(Rangei_t range1, Rangei_t range2, bool inclusive = true)
Rangei_t RangeiOverlap(Rangei_t range1, Rangei_t range2)
void FreePrimitiveIndexedVerts(PrimitiveIndexedVerts_t* indexedVerts)
void InvertPrimitiveVerts(PrimitiveIndexedVerts_t* indexedVerts)
PrimitiveIndexedVerts_t GenerateVertsForBox(Box_t box, MemArena_t* memArena)
PrimitiveIndexedVerts_t GenerateVertsForSphere(Sphere_t sphere, u64 numRings, u64 numSegments, bool smoothShading, MemArena_t* memArena)
*/
