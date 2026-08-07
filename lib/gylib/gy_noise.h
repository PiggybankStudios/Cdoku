/*
File:   gy_noise.h
Author: Taylor Robbins
Date:   09\08\2022
Description:
	** Contains functions that help us generate various kinds of noise
	** (Currently just "Improved" Perlin Noise or "Simplex Noise")
*/

#ifndef _GY_NOISE_H
#define _GY_NOISE_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"

#ifdef GYLIB_HEADER_ONLY
extern const uint8_t SimplexPerm[256];
#else
//TODO: Regenerate random numbers for this array
const uint8_t SimplexPerm[256] = {
    151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
    140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
    247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
     57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
     74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
     60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
     65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
    200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
     52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
    207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
    119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
    129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
    218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
     81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
    184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
    222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180
};
#endif

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	u8 SimplexHash(i32 input);
	r32 SimplexGradient1D(i32 hash, r32 x);
	r32 SimplexGradient2D(i32 hash, r32 x, r32 y);
	r32 SimplexGradient3D(i32 hash, r32 x, r32 y, r32 z);
	r32 SimplexNoise1D(r32 x);
	r32 SimplexNoise2D(r32 x, r32 y);
	r32 SimplexNoise2DIter(r32 x, r32 y, u64 numIterations, r32 scale = 1.0f);
#else

u8 SimplexHash(i32 input)
{
	return SimplexPerm[(u8)input];
}

r32 SimplexGradient1D(i32 hash, r32 x)
{
	const i32 h = (hash & 0x0F);
	r32 gradient = 1.0f + (h & 0x07);
	if (IsFlagSet(h, 0x08)) { gradient = -gradient; }
	return (gradient * x);
}

r32 SimplexGradient2D(i32 hash, r32 x, r32 y)
{
	const i32 h = (hash & 0x3F);
	const r32 u = (h < 4) ? x : y;
	const r32 v = (h < 4) ? y : x;
	return (IsFlagSet(h, 0x01) ? -u : u) + (IsFlagSet(h, 0x02) ? (-2.0f * v) : (2.0f * v));
}

r32 SimplexGradient3D(i32 hash, r32 x, r32 y, r32 z)
{
	int h = (hash & 15);
	r32 u = (h < 8) ? x : y;
	r32 v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);
	return (IsFlagSet(h, 0x01) ? -u : u) + (IsFlagSet(h, 0x02) ? -v : v);
}

//NOTE: These functions return values in [-1, 1] range not [0, 1]

r32 SimplexNoise1D(r32 x)
{
	i32 i0 = FloorR32i(x);
	i32 i1 = i0 + 1;
	r32 x0 = x - i0;
	r32 x1 = x0 - 1.0f;
	
	r32 t0 = 1.0f - Square(x0);
	t0 *= t0;
	r32 noiseContrib0 = Square(t0) * SimplexGradient1D(SimplexHash(i0), x0);
	
	r32 t1 = 1.0f - Square(x1);
	t1 *= t1;
	r32 noiseContrib1 = Square(t1) * SimplexGradient1D(SimplexHash(i1), x1);
	
	return 0.395f * (noiseContrib0 + noiseContrib1);
}

r32 SimplexNoise2D(r32 x, r32 y)
{
	static const r32 F2 = 0.366025403f; // = (sqrt(3) - 1) / 2
	static const r32 G2 = 0.211324865f; // = (3 - sqrt(3)) / 6
	
	const r32 s = (x + y) * F2;
	const r32 xs = x + s;
	const r32 ys = y + s;
	const i32 i = FloorR32i(xs);
	const i32 j = FloorR32i(ys);
	
	const r32 t = (r32)(i + j) * G2;
	const r32 X0 = i - t;
	const r32 Y0 = j - t;
	const r32 x0 = x - X0;
	const r32 y0 = y - Y0;
	
	i32 i1, j1;
	if (x0 > y0) { i1 = 1; j1 = 0; }
	else         { i1 = 0; j1 = 1; }
	
	const r32 x1 = x0 - i1 + G2;
	const r32 y1 = y0 - j1 + G2;
	const r32 x2 = x0 - 1 + (2 * G2);
	const r32 y2 = y0 - 1 + (2 * G2);
	
	const int gi0 = SimplexHash(i + SimplexHash(j));
	const int gi1 = SimplexHash(i + i1 + SimplexHash(j + j1));
	const int gi2 = SimplexHash(i + 1 + SimplexHash(j + 1));
	
    r32 noiseContrib0;
	r32 t0 = 0.5f - Square(x0) - Square(y0);
	if (t0 < 0.0f) { noiseContrib0 = 0.0f; }
	else { t0 *= t0; noiseContrib0 = Square(t0) * SimplexGradient2D(gi0, x0, y0); }
	
    r32 noiseContrib1;
	r32 t1 = 0.5f - Square(x1) - Square(y1);
	if (t1 < 0.0f) { noiseContrib1 = 0.0f; }
	else { t1 *= t1; noiseContrib1 = Square(t1) * SimplexGradient2D(gi1, x1, y1); }
	
    r32 noiseContrib2;
	r32 t2 = 0.5f - Square(x2) - Square(y2);
	if (t2 < 0.0f) { noiseContrib2 = 0.0f; }
	else { t2 *= t2; noiseContrib2 = Square(t2) * SimplexGradient2D(gi2, x2, y2); }
	
	return 45.23065f * (noiseContrib0 + noiseContrib1 + noiseContrib2);
}

r32 SimplexNoise2DIter(r32 x, r32 y, u64 numIterations, r32 scale = 1.0f)
{
	r32 extraOnFirst = 1.0f / (r32)(1ULL << (numIterations+1));
	r32 result = 0.0f;
	for (u64 iIndex = 0; iIndex < numIterations; iIndex++)
	{
		r32 amplitude = 1.0f / (r32)(1ULL << (iIndex+1)) + ((iIndex == 0) ? extraOnFirst : 0.0f);
		r32 frequency = (r32)(iIndex+1);
		r32 noiseValue = SimplexNoise2D(x * frequency * scale, y * frequency * scale);
		result += noiseValue * amplitude;
	}
	return result;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_NOISE_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
u8 SimplexHash(i32 input)
r32 SimplexGradient1D(i32 hash, r32 x)
r32 SimplexGradient2D(i32 hash, r32 x, r32 y)
r32 SimplexGradient3D(i32 hash, r32 x, r32 y, r32 z)
r32 SimplexNoise1D(r32 x)
r32 SimplexNoise2D(r32 x, r32 y)
r32 SimplexNoise2DIter(r32 x, r32 y, u64 numIterations, r32 scale = 1.0f)
*/
