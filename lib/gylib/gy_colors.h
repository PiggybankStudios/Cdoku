/*
File:   gy_colors.h
Author: Taylor Robbins
Date:   09\26\2021
Description:
	** Holds the Color_t structure and other similar structures and all the functions to manipulate them
*/

#ifndef _GY_COLORS_H
#define _GY_COLORS_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"

union Color_t
{
	u32 value;
	struct { u8 b, g, r, a; };
	struct { u8 blue, green, red, alpha; };
	struct { u8 b, g, r, a; } argb; //TODO: The "ARGB" ordering used by windows? Or PNGs? or BMPs? or GIFs?
	struct { u8 r, g, b, a; } rgba; //TODO: The RGBA ordering is used by a few other things
};
union Colorf_t
{
	v4 values;
	struct { r32 b, g, r, a; };
	struct { r32 blue, green, red, alpha; };
};
union ColorHSV_t
{
	v4 values;
	struct { r32 h, s, v, a; };
	struct { r32 hue, saturation, value, alpha; };
};
union ColorXYZ_t
{
	r64 values[4];
	struct { r64 x, y, z, a; };
	struct { r64 unused1, unused2, unused3, alpha; };
};
union ColorLCH_t
{
	r64 values[4];
	struct { r64 l, c, h, a; };
	struct { r64 luminence, chroma, hue, alpha; };
};
union ColorLAB_t
{
	r64 values[4];
	struct { r64 l, a, b, alpha; };
};

// +--------------------------------------------------------------+
// |                Illuminant/Observer Constants                 |
// +--------------------------------------------------------------+
//NOTE: These values are copied from https://www.easyrgb.com/en/math.php
enum ColorObserver_t
{
	ColorObserver_TwoDegree = 0x00, //CIE 1931
	ColorObserver_TenDegree,        //CIE 1964
	ColorObserver_NumOptions,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetColorObserverStr(ColorObserver_t enumValue);
#else
const char* GetColorObserverStr(ColorObserver_t enumValue)
{
	switch (enumValue)
	{
		case ColorObserver_TwoDegree: return "TwoDegree";
		case ColorObserver_TenDegree: return "TenDegree";
		default: return "Unknown";
	}
}
#endif

enum ColorIlluminant_t
{
	ColorIlluminant_A = 0x00, //Incandescent/tungsten
	ColorIlluminant_B,        //Old direct sunlight at noon
	ColorIlluminant_C,        //Old daylight
	ColorIlluminant_D50,      //ICC profile PCS
	ColorIlluminant_D55,      //Mid-morning daylight
	ColorIlluminant_D65,      //Daylight, sRGB, Adobe-RGB
	ColorIlluminant_D75,      //North sky daylight
	ColorIlluminant_E,        //Equal energy
	ColorIlluminant_F1,       //Daylight Fluorescent
	ColorIlluminant_F2,       //Cool fluorescent
	ColorIlluminant_F3,       //White Fluorescent
	ColorIlluminant_F4,       //Warm White Fluorescent
	ColorIlluminant_F5,       //Daylight Fluorescent
	ColorIlluminant_F6,       //Lite White Fluorescent
	ColorIlluminant_F7,       //Daylight fluorescent, D65 simulator
	ColorIlluminant_F8,       //Sylvania F40, D50 simulator
	ColorIlluminant_F9,       //Cool White Fluorescent
	ColorIlluminant_F10,      //Ultralume 50, Philips TL85
	ColorIlluminant_F11,      //Ultralume 40, Philips TL84
	ColorIlluminant_F12,      //Ultralume 30, Philips TL83
	ColorIlluminant_NumOptions,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetColorIlluminantStr(ColorIlluminant_t enumValue);
#else
const char* GetColorIlluminantStr(ColorIlluminant_t enumValue)
{
	switch (enumValue)
	{
		case ColorIlluminant_A:   return "A";
		case ColorIlluminant_B:   return "B";
		case ColorIlluminant_C:   return "C";
		case ColorIlluminant_D50: return "D50";
		case ColorIlluminant_D55: return "D55";
		case ColorIlluminant_D65: return "D65";
		case ColorIlluminant_D75: return "D75";
		case ColorIlluminant_E:   return "E";
		case ColorIlluminant_F1:  return "F1";
		case ColorIlluminant_F2:  return "F2";
		case ColorIlluminant_F3:  return "F3";
		case ColorIlluminant_F4:  return "F4";
		case ColorIlluminant_F5:  return "F5";
		case ColorIlluminant_F6:  return "F6";
		case ColorIlluminant_F7:  return "F7";
		case ColorIlluminant_F8:  return "F8";
		case ColorIlluminant_F9:  return "F9";
		case ColorIlluminant_F10: return "F10";
		case ColorIlluminant_F11: return "F11";
		case ColorIlluminant_F12: return "F12";
		default: return "Unknown";
	}
}
#endif

#ifdef GYLIB_HEADER_ONLY
extern const r64 ColorIllumValues[ColorIlluminant_NumOptions][ColorObserver_NumOptions][3];
#else
const r64 ColorIllumValues[ColorIlluminant_NumOptions][ColorObserver_NumOptions][3] = {
	{ { 109.850, 100.000, 35.585  }, { 111.144, 100.000, 35.200  } },
	{ { 99.0927, 100.000, 85.313  }, { 99.178,  100.000, 84.3493 } },
	{ { 98.074,  100.000, 118.232 }, { 97.285,  100.000, 116.145 } },
	{ { 96.422,  100.000, 82.521  }, { 96.720,  100.000, 81.427  } },
	{ { 95.682,  100.000, 92.149  }, { 95.799,  100.000, 90.926  } },
	{ { 95.047,  100.000, 108.883 }, { 94.811,  100.000, 107.304 } },
	{ { 94.972,  100.000, 122.638 }, { 94.416,  100.000, 120.641 } },
	{ { 100.000, 100.000, 100.000 }, { 100.000, 100.000, 100.000 } },
	{ { 92.834,  100.000, 103.665 }, { 94.791,  100.000, 103.191 } },
	{ { 99.187,  100.000, 67.395  }, { 103.280, 100.000, 69.026  } },
	{ { 103.754, 100.000, 49.861  }, { 108.968, 100.000, 51.965  } },
	{ { 109.147, 100.000, 38.813  }, { 114.961, 100.000, 40.963  } },
	{ { 90.872,  100.000, 98.723  }, { 93.369,  100.000, 98.636  } },
	{ { 97.309,  100.000, 60.191  }, { 102.148, 100.000, 62.074  } },
	{ { 95.044,  100.000, 108.755 }, { 95.792,  100.000, 107.687 } },
	{ { 96.413,  100.000, 82.333  }, { 97.115,  100.000, 81.135  } },
	{ { 100.365, 100.000, 67.868  }, { 102.116, 100.000, 67.826  } },
	{ { 96.174,  100.000, 81.712  }, { 99.001,  100.000, 83.134  } },
	{ { 100.966, 100.000, 64.370  }, { 103.866, 100.000, 65.627  } },
	{ { 108.046, 100.000, 39.228  }, { 111.428, 100.000, 40.353  } }
};
#endif

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	Color_t NewColor(u32 value, bool rgbaOrder = false);
	Color_t NewColor(u8 r, u8 g, u8 b, u8 a, bool rgbaOrder = false);
	Color_t NewColor(u8 r, u8 g, u8 b);
	Colorf_t NewColorf(r32 r, r32 g, r32 b);
	Colorf_t NewColorf(r32 r, r32 g, r32 b, r32 a);
	ColorHSV_t NewColorHsv(r32 hue, r32 saturation, r32 value);
	ColorHSV_t NewColorHsv(r32 hue, r32 saturation, r32 value, r32 alpha);
	u8 ColorChannelToU8(r32 valueR32);
	r32 ColorChannelToR32(u8 valueU8);
	u8 MultiplyColorChannelU8(u8 left, u8 right);
	u8 MultiplyColorChannelR32(u8 left, r32 rightR32);
	Color_t ToColor(Colorf_t colorf);
	Color_t ToColor(v3 vector3);
	Color_t ToColor(v4 vector4);
	Color_t ColorTransparent(r32 alpha);
	Color_t ColorTransparent(Color_t color, r32 alpha);
	Colorf_t ToColorf(Color_t color);
	Colorf_t ToColorf(v3 vector3);
	Colorf_t ToColorf(v4 vector4);
	ColorHSV_t ToColorHsv(v3 vector3);
	ColorHSV_t ToColorHsv(v4 vector4);
	v3 ToVec3(Color_t color);
	v3 ToVec3(Colorf_t color);
	v4 ToVec4(Color_t color);
	v4 ToVec4(Colorf_t color);
	Color_t ColorLerp(Color_t start, Color_t end, r32 amount);
	Color_t ColorDarken(Color_t color, u8 amount);
	Color_t ColorLighten(Color_t color, u8 amount);
	Color_t ColorMultiply(Color_t color1, Color_t color2);
	Color_t ColorMultiplyAlpha(Color_t color, u8 alphaValue);
	Color_t ColorMultiplyAlphaR32(Color_t color, r32 amount);
	Color_t ColorOpposite(Color_t color);
	bool BasicallyEqualColorf(Colorf_t left, Colorf_t right, r32 tolerance = 0.001f);
	Colorf_t ColorfLerp(Colorf_t start, Colorf_t end, r32 amount);
	Colorf_t ColorfDarken(Colorf_t color, r32 amount);
	Colorf_t ColorfLighten(Colorf_t color, r32 amount);
	Colorf_t ColorfMultiply(Colorf_t color1, Colorf_t color2);
	Colorf_t ColorfMultiplyAlpha(Colorf_t color, r32 alphaValue);
	Colorf_t ColorfOpposite(Colorf_t color);
	Color_t ColorRGBFromHSV(ColorHSV_t colorHsv);
	ColorHSV_t ColorHSVFromRGB(Color_t color);
	r64 ExpandNonLinearSrgb(r64 nonlinearValue);
	r64 CompressLinearSrgb(r64 linearValue);
	ColorXYZ_t ColorXyzFromSrgb(Color_t color);
	Color_t ColorSrgbFromXyz(ColorXYZ_t colorXyz, bool* isValidColorOut = nullptr);
	r64 DoSpecialThing(r64 xyzValue);
	r64 UndoSpecialThing(r64 specialValue);
	ColorLAB_t ColorLabFromXyz(ColorXYZ_t colorXyz, ColorObserver_t observer = ColorObserver_TwoDegree, ColorIlluminant_t illuminant = ColorIlluminant_D65);
	ColorXYZ_t ColorXyzFromLab(ColorLAB_t colorLab, ColorObserver_t observer = ColorObserver_TwoDegree, ColorIlluminant_t illuminant = ColorIlluminant_D65);
	ColorLCH_t ColorLchFromLab(ColorLAB_t colorLab);
	ColorLAB_t ColorLabFromLch(ColorLCH_t colorLch);
	Color_t ColorComplementary(Color_t color);
	Color_t ColorComplementaryOld(Color_t color);
	Color_t ColorDesaturate(Color_t color, r32 saturation);
	Colorf_t ColorfComplementary(Colorf_t color);
	Colorf_t ColorfDesaturate(Colorf_t color, r32 saturation);
	bool operator == (Color_t left, Color_t right);
	bool operator != (Color_t left, Color_t right);
	bool operator == (Colorf_t left, Colorf_t right);
	bool operator != (Colorf_t left, Colorf_t right);
	bool operator == (ColorHSV_t left, ColorHSV_t right);
	bool operator != (ColorHSV_t left, ColorHSV_t right);
	bool operator == (ColorXYZ_t left, ColorXYZ_t right);
	bool operator != (ColorXYZ_t left, ColorXYZ_t right);
	bool operator == (ColorLCH_t left, ColorLCH_t right);
	bool operator != (ColorLCH_t left, ColorLCH_t right);
	bool operator == (ColorLAB_t left, ColorLAB_t right);
	bool operator != (ColorLAB_t left, ColorLAB_t right);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Color_t NewColor(u32 value, bool rgbaOrder = false) //default is argb order
{
	Color_t result;
	result.value = rgbaOrder ? (((value&0x00FF0000) >> 16) | ((value&0x0000FF00) >> 0) | ((value&0x000000FF) << 16) | ((value&0xFF000000) >> 0)) : value;
	return result;
}
Color_t NewColor(u8 r, u8 g, u8 b, u8 a, bool rgbaOrder = false) //default is argb order
{
	Color_t result;
	if (rgbaOrder) { result.rgba.r = r; result.rgba.g = g; result.rgba.b = b; result.rgba.a = a; }
	else           { result.argb.r = r; result.argb.g = g; result.argb.b = b; result.argb.a = a; }
	return result;
}
Color_t NewColor(u8 r, u8 g, u8 b)
{
	Color_t result;
	result.r = r;
	result.g = g;
	result.b = b;
	result.a = 255;
	return result;
}

Colorf_t NewColorf(r32 r, r32 g, r32 b)
{
	Colorf_t result = {};
	result.r = r;
	result.g = g;
	result.b = b;
	result.a = 1.0f;
	return result;
}
Colorf_t NewColorf(r32 r, r32 g, r32 b, r32 a)
{
	Colorf_t result = {};
	result.r = r;
	result.g = g;
	result.b = b;
	result.a = a;
	return result;
}

ColorHSV_t NewColorHsv(r32 hue, r32 saturation, r32 value)
{
	ColorHSV_t result = {};
	result.hue = hue;
	result.saturation = saturation;
	result.value = value;
	result.alpha = 1.0f;
	return result;
}
ColorHSV_t NewColorHsv(r32 hue, r32 saturation, r32 value, r32 alpha)
{
	ColorHSV_t result = {};
	result.hue = hue;
	result.saturation = saturation;
	result.value = value;
	result.alpha = alpha;
	return result;
}

// +--------------------------------------------------------------+
// |                    Color Channel Helpers                     |
// +--------------------------------------------------------------+
u8 ColorChannelToU8(r32 valueR32)
{
	return ClampI32toU8(RoundR32i(valueR32 * 255.0f));
}
r32 ColorChannelToR32(u8 valueU8)
{
	return (r32)valueU8 / 255.0f;
}
u8 MultiplyColorChannelU8(u8 left, u8 right)
{
	return ColorChannelToU8(ColorChannelToR32(left) * ColorChannelToR32(right));
}
u8 MultiplyColorChannelR32(u8 left, r32 rightR32)
{
	return ColorChannelToU8(ColorChannelToR32(left) * rightR32);
}

// +--------------------------------------------------------------+
// |               Casting and Conversion Functions               |
// +--------------------------------------------------------------+
Color_t ToColor(Colorf_t colorf)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(colorf.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(colorf.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(colorf.b * 255.0f));
	result.a = ClampI32toU8(RoundR32i(colorf.a * 255.0f));
	return result;
}
Color_t ToColor(v3 vector3)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(vector3.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(vector3.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(vector3.b * 255.0f));
	result.a = 255;
	return result;
}
Color_t ToColor(v4 vector4)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(vector4.r * 255.0f));
	result.g = ClampI32toU8(RoundR32i(vector4.g * 255.0f));
	result.b = ClampI32toU8(RoundR32i(vector4.b * 255.0f));
	result.a = ClampI32toU8(RoundR32i(vector4.a * 255.0f));
	return result;
}
Color_t ColorTransparent(r32 alpha)
{
	return NewColor(255, 255, 255, ColorChannelToU8(alpha));
}
Color_t ColorTransparent(Color_t color, r32 alpha)
{
	return NewColor(color.r, color.g, color.b, ColorChannelToU8(alpha));
}

Colorf_t ToColorf(Color_t color)
{
	Colorf_t result = {};
	result.r = ((r32)color.r / 255.0f);
	result.g = ((r32)color.g / 255.0f);
	result.b = ((r32)color.b / 255.0f);
	result.a = ((r32)color.a / 255.0f);
	return result;
}
Colorf_t ToColorf(v3 vector3)
{
	Colorf_t result = {};
	result.r = vector3.r;
	result.g = vector3.g;
	result.b = vector3.b;
	result.a = 1.0f;
	return result;
}
Colorf_t ToColorf(v4 vector4)
{
	Colorf_t result = {};
	result.r = vector4.r;
	result.g = vector4.g;
	result.b = vector4.b;
	result.a = vector4.a;
	return result;
}

ColorHSV_t ToColorHsv(v3 vector3)
{
	ColorHSV_t result = {};
	result.hue = vector3.x;
	result.saturation = vector3.y;
	result.value = vector3.z;
	result.alpha = 1.0f;
	return result;
}
ColorHSV_t ToColorHsv(v4 vector4)
{
	ColorHSV_t result = {};
	result.hue = vector4.x;
	result.saturation = vector4.y;
	result.value = vector4.z;
	result.alpha = vector4.w;
	return result;
}

v3 ToVec3(Color_t color)
{
	v3 result;
	result.r = (r32)color.r / 255.0f;
	result.g = (r32)color.g / 255.0f;
	result.b = (r32)color.b / 255.0f;
	return result;
}
v3 ToVec3(Colorf_t color)
{
	v3 result;
	result.r = color.r;
	result.g = color.g;
	result.b = color.b;
	return result;
}

v4 ToVec4(Color_t color)
{
	v4 result;
	result.r = (r32)color.r / 255.0f;
	result.g = (r32)color.g / 255.0f;
	result.b = (r32)color.b / 255.0f;
	result.a = (r32)color.a / 255.0f;
	return result;
}
v4 ToVec4(Colorf_t color)
{
	v4 result;
	result.r = color.r;
	result.g = color.g;
	result.b = color.b;
	result.a = color.a;
	return result;
}

// +--------------------------------------------------------------+
// |                     Basic Manipulations                      |
// +--------------------------------------------------------------+
// +==============================+
// |            Color             |
// +==============================+
Color_t ColorLerp(Color_t start, Color_t end, r32 amount)
{
	Color_t result = {};
	result.r = ClampI32toU8(RoundR32i(LerpR32((r32)start.r, (r32)end.r, amount)));
	result.g = ClampI32toU8(RoundR32i(LerpR32((r32)start.g, (r32)end.g, amount)));
	result.b = ClampI32toU8(RoundR32i(LerpR32((r32)start.b, (r32)end.b, amount)));
	result.a = ClampI32toU8(RoundR32i(LerpR32((r32)start.a, (r32)end.a, amount)));
	return result;
}

Color_t ColorDarken(Color_t color, u8 amount)
{
	Color_t result = color;
	if (result.r > amount) { result.r -= amount; }
	else { result.r = 0; }
	if (result.g > amount) { result.g -= amount; }
	else { result.g = 0; }
	if (result.b > amount) { result.b -= amount; }
	else { result.b = 0; }
	return result;
}
Color_t ColorLighten(Color_t color, u8 amount)
{
	Color_t result = color;
	if (result.r < 255 - amount) { result.r += amount; }
	else { result.r = 255; }
	if (result.g < 255 - amount) { result.g += amount; }
	else { result.g = 255; }
	if (result.b < 255 - amount) { result.b += amount; }
	else { result.b = 255; }
	return result;
}

//TODO: Does this need to convert to/from linear color space before/after
Color_t ColorMultiply(Color_t color1, Color_t color2)
{
	Color_t result;
	result.r = ClampI32toU8(RoundR32i(((r32)color1.r / 255.0f) * ((r32)color2.r / 255.0f) * 255));
	result.g = ClampI32toU8(RoundR32i(((r32)color1.g / 255.0f) * ((r32)color2.g / 255.0f) * 255));
	result.b = ClampI32toU8(RoundR32i(((r32)color1.b / 255.0f) * ((r32)color2.b / 255.0f) * 255));
	result.a = ClampI32toU8(RoundR32i(((r32)color1.a / 255.0f) * ((r32)color2.a / 255.0f) * 255));
	return result;
}
Color_t ColorMultiplyAlpha(Color_t color, u8 alphaValue)
{
	Color_t result = color;
	result.a = ClampI32toU8(RoundR32i(((r32)color.a / 255.0f) * ((r32)alphaValue / 255.0f) * 255));
	return result;
}
Color_t ColorMultiplyAlphaR32(Color_t color, r32 amount)
{
	Color_t result = color;
	result.a = ClampI32toU8(RoundR32i(((r32)color.a / 255.0f) * amount * 255));
	return result;
}

Color_t ColorOpposite(Color_t color)
{
	Color_t result;
	result.r = 255 - color.r;
	result.g = 255 - color.g;
	result.b = 255 - color.b;
	result.a = 255 - color.a;
	return result;
}

// +==============================+
// |            Colorf            |
// +==============================+
bool BasicallyEqualColorf(Colorf_t left, Colorf_t right, r32 tolerance = 0.001f)
{
	if (!BasicallyEqualR32(left.r, right.r, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.g, right.g, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.b, right.b, tolerance)) { return false; }
	if (!BasicallyEqualR32(left.a, right.a, tolerance)) { return false; }
	return true;
}

Colorf_t ColorfLerp(Colorf_t start, Colorf_t end, r32 amount)
{
	Colorf_t result = {};
	result.r = LerpR32(start.r, end.r, amount);
	result.g = LerpR32(start.g, end.g, amount);
	result.b = LerpR32(start.b, end.b, amount);
	result.a = LerpR32(start.a, end.a, amount);
	return result;
}

Colorf_t ColorfDarken(Colorf_t color, r32 amount)
{
	Colorf_t result = color;
	result.r = ClampR32(result.r - amount, 0, 1.0f);
	result.g = ClampR32(result.g - amount, 0, 1.0f);
	result.b = ClampR32(result.b - amount, 0, 1.0f);
	return result;
}
Colorf_t ColorfLighten(Colorf_t color, r32 amount)
{
	Colorf_t result = color;
	result.r = ClampR32(result.r + amount, 0, 1.0f);
	result.g = ClampR32(result.g + amount, 0, 1.0f);
	result.b = ClampR32(result.b + amount, 0, 1.0f);
	return result;
}

//TODO: Does this need to convert to/from linear color space before/after?
Colorf_t ColorfMultiply(Colorf_t color1, Colorf_t color2)
{
	Colorf_t result;
	result.r = color1.r * color2.r;
	result.g = color1.g * color2.g;
	result.b = color1.b * color2.b;
	result.a = color1.a * color2.a;
	return result;
}
Colorf_t ColorfMultiplyAlpha(Colorf_t color, r32 alphaValue)
{
	Colorf_t result = color;
	result.a *= alphaValue;
	return result;
}

Colorf_t ColorfOpposite(Colorf_t color)
{
	Colorf_t result;
	result.r = 1.0f - color.r;
	result.g = 1.0f - color.g;
	result.b = 1.0f - color.b;
	result.a = 1.0f - color.a;
	return result;
}

// +--------------------------------------------------------------+
// |                   Color Space Conversions                    |
// +--------------------------------------------------------------+
//TODO: These HSV <-> RGB functions seem to have problems!
Color_t ColorRGBFromHSV(ColorHSV_t colorHsv)
{
	r32 cValue = colorHsv.v * colorHsv.s;
	r32 xValue = cValue * (1 - AbsR32((r32)((RoundR32i(colorHsv.h) / 60)%2) - 1));
	r32 nextX = cValue * (1 - AbsR32((r32)((RoundR32i(colorHsv.h) / 60 + 1)%2) - 1));
	r32 mValue = colorHsv.v - cValue;
	
	u8 hueSwitch = ClampI32toU8(RoundR32i(colorHsv.h) / 60);
	v3 color1 = {};
	v3 color2 = {};
	r32 lerpAmount = (RoundR32i(colorHsv.h)%60) / 60.0f;
	
	switch (hueSwitch)
	{
		case 0: //0-60 degrees
		{
			color1 = NewVec3(cValue, xValue,      0);
			color2 = NewVec3( nextX, cValue,      0);
		} break;
		case 1: //60-120 degrees
		{
			color1 = NewVec3(xValue, cValue,      0);
			color2 = NewVec3(     0, cValue,  nextX);
		} break;
		case 2: //120-180 degrees
		{
			color1 = NewVec3(     0, cValue, xValue);
			color2 = NewVec3(     0,  nextX, cValue);
		} break;
		case 3: //180-240 degrees
		{
			color1 = NewVec3(     0, xValue, cValue);
			color2 = NewVec3( nextX,      0, cValue);
		} break;
		case 4: //240-300 degrees
		{
			color1 = NewVec3(xValue,      0, cValue);
			color2 = NewVec3(cValue,      0,  nextX);
		} break;
		case 5: //300-360 degrees
		case 6:
		{
			color1 = NewVec3(cValue,      0, xValue);
			color2 = NewVec3(cValue,  nextX,      0);
		} break;
		
		default: return NewColor(0, 0, 0, 0);
	};
	
	Color_t result = NewColor(
		(u8)((LerpR32(color1.x, color2.x, lerpAmount) + mValue)*255),
		(u8)((LerpR32(color1.y, color2.y, lerpAmount) + mValue)*255),
		(u8)((LerpR32(color1.z, color2.z, lerpAmount) + mValue)*255), 255);
	
	return result;
}
ColorHSV_t ColorHSVFromRGB(Color_t color)
{
	ColorHSV_t result = {};
	
	r32 red   = ((r32)color.r / 255.0f);
	r32 green = ((r32)color.g / 255.0f);
	r32 blue  = ((r32)color.b / 255.0f);
	r32 minRgb = MinR32(red, green, blue);
	r32 maxRgb = MaxR32(red, green, blue);
	r32 minMaxDelta = maxRgb - minRgb;
	
	result.a = ((r32)color.a / 255.0f);
	result.v = maxRgb;
	if (minMaxDelta == 0)
	{
		result.h = 0;
		result.s = 0.0f;
	}
	else
	{
		result.s = minMaxDelta / maxRgb;
		
		r32 rDelta = (((maxRgb - red)   / 6) + (minMaxDelta / 2)) / minMaxDelta;
		r32 gDelta = (((maxRgb - green) / 6) + (minMaxDelta / 2)) / minMaxDelta;
		r32 bDelta = (((maxRgb - blue)  / 6) + (minMaxDelta / 2)) / minMaxDelta;
		
		r32 hueR32 = 0.0f;
		if (red == maxRgb) { hueR32 = bDelta - gDelta; }
		else if (green == maxRgb) { hueR32 = (1.0f/3.0f) + rDelta - bDelta; }
		else if (blue == maxRgb) { hueR32 = (2.0f/3.0f) + gDelta - rDelta; }
		
		if (hueR32 < 0) { hueR32 += 1; }
		if (hueR32 > 1) { hueR32 -= 1; }
		
		result.hue = (r32)(RoundR32i(hueR32*360) % 360);
	}
	
	return result;
}

//NOTE: I don't remember how these work. What the heck is the <= 0.04045 check?
//Convert sRGB to linear RGB
r64 ExpandNonLinearSrgb(r64 nonlinearValue)
{
	return (nonlinearValue <= 0.04045) ? (nonlinearValue / 12.92) : (PowR64((nonlinearValue+0.055)/1.055, 2.4));
}
//Convert linear RGB to sRGB
r64 CompressLinearSrgb(r64 linearValue)
{
	return (linearValue <= 0.0031308) ? (linearValue * 12.92) : (1.055 * PowR64(linearValue, 1.0/2.4) - 0.055);
}

ColorXYZ_t ColorXyzFromSrgb(Color_t color)
{
	r64 linearRed   = ExpandNonLinearSrgb((r64)color.r / 255.0);
	r64 linearGreen = ExpandNonLinearSrgb((r64)color.g / 255.0);
	r64 linearBlue  = ExpandNonLinearSrgb((r64)color.b / 255.0);
	
	//NOTE: X, Y and Z output refer to a D65/2° standard illuminant
	ColorXYZ_t result = {};
	result.x = (linearRed * 0.4124 + linearGreen * 0.3576 + linearBlue * 0.1805) * 100;
	result.y = (linearRed * 0.2126 + linearGreen * 0.7152 + linearBlue * 0.0722) * 100;
	result.z = (linearRed * 0.0193 + linearGreen * 0.1192 + linearBlue * 0.9505) * 100;
	result.a = ((r64)color.a / 255.0);
	return result;
}
Color_t ColorSrgbFromXyz(ColorXYZ_t colorXyz, bool* isValidColorOut = nullptr)
{
	r64 nonlinearRed   = CompressLinearSrgb((colorXyz.x *  3.2406 + colorXyz.y * -1.5372 + colorXyz.z * -0.4986) / 100.0);
	r64 nonlinearGreen = CompressLinearSrgb((colorXyz.x * -0.9689 + colorXyz.y *  1.8758 + colorXyz.z *  0.0415) / 100.0);
	r64 nonlinearBlue  = CompressLinearSrgb((colorXyz.x *  0.0557 + colorXyz.y * -0.2040 + colorXyz.z *  1.0570) / 100.0);
	
	Color_t result = {};
	result.r = ClampI32toU8((i32)RoundR64i(nonlinearRed * 255));
	result.g = ClampI32toU8((i32)RoundR64i(nonlinearGreen * 255));
	result.b = ClampI32toU8((i32)RoundR64i(nonlinearBlue * 255));
	result.a = ClampI32toU8((i32)RoundR64i(colorXyz.a * 255));
	if (isValidColorOut != nullptr)
	{
		if      (nonlinearRed   < 0.0 || nonlinearRed   > 1.0) { *isValidColorOut = false; }
		else if (nonlinearGreen < 0.0 || nonlinearGreen > 1.0) { *isValidColorOut = false; }
		else if (nonlinearBlue  < 0.0 || nonlinearBlue  > 1.0) { *isValidColorOut = false; }
		else { *isValidColorOut = true; }
		
	}
	return result;
}

r64 DoSpecialThing(r64 xyzValue) //TODO: Rename me!
{
	return (xyzValue > 0.008856) ? (CbrtR64(xyzValue)) : ((7.787 * xyzValue) + (16.0 / 116.0));
}
r64 UndoSpecialThing(r64 specialValue) //TODO: Rename me!
{
	return ((specialValue*specialValue*specialValue) > 0.008856) ? (Cube(specialValue)) : ((specialValue - (16.0 / 116.0)) / 7.787);
}

ColorLAB_t ColorLabFromXyz(ColorXYZ_t colorXyz, ColorObserver_t observer = ColorObserver_TwoDegree, ColorIlluminant_t illuminant = ColorIlluminant_D65)
{
	Assert(observer < ColorObserver_NumOptions);
	Assert(illuminant < ColorIlluminant_NumOptions);
	
	// MyLibPrintLine_D("colorXyz = (%f, %f, %f)", colorXyz.x, colorXyz.y, colorXyz.z);
	// MyLibPrintLine_D("IllumValues[%u][%u] = {%f, %f, %f}", illuminant, observer, ColorIllumValues[illuminant][observer][0], ColorIllumValues[illuminant][observer][1], ColorIllumValues[illuminant][observer][2]);
	r64 xValue = colorXyz.x / ColorIllumValues[illuminant][observer][0];
	r64 yValue = colorXyz.y / ColorIllumValues[illuminant][observer][1];
	r64 zValue = colorXyz.z / ColorIllumValues[illuminant][observer][2];
	// MyLibPrintLine_D("zyzValues = (%f, %f, %f)", xValue, yValue, zValue);
	
	xValue = DoSpecialThing(xValue);
	yValue = DoSpecialThing(yValue);
	zValue = DoSpecialThing(zValue);
	// MyLibPrintLine_D("specialValues = (%f, %f, %f)", xValue, yValue, zValue);
	
	ColorLAB_t result = {};
	result.l = (116 * yValue) - 16;
	result.a = 500 * (xValue - yValue);
	result.b = 200 * (yValue - zValue);
	result.alpha = colorXyz.alpha;
	return result;
}
ColorXYZ_t ColorXyzFromLab(ColorLAB_t colorLab, ColorObserver_t observer = ColorObserver_TwoDegree, ColorIlluminant_t illuminant = ColorIlluminant_D65)
{
	Assert(observer < ColorObserver_NumOptions);
	Assert(illuminant < ColorIlluminant_NumOptions);
	
	r64 yValue = (colorLab.l + 16) / 116;
	r64 xValue = (colorLab.a / 500) + yValue;
	r64 zValue = yValue - (colorLab.b / 200);
	
	xValue = UndoSpecialThing(xValue);
	yValue = UndoSpecialThing(yValue);
	zValue = UndoSpecialThing(zValue);
	
	ColorXYZ_t result = {};
	result.x = xValue * ColorIllumValues[illuminant][observer][0];
	result.y = yValue * ColorIllumValues[illuminant][observer][1];
	result.z = zValue * ColorIllumValues[illuminant][observer][2];
	result.alpha = colorLab.alpha;
	return result;
}

ColorLCH_t ColorLchFromLab(ColorLAB_t colorLab)
{
	r64 hValue = AtanR64(colorLab.b, colorLab.a);
	
	if (hValue > 0) { hValue = (hValue/Pi64) * 180; }
	else { hValue = 360 - ((AbsR64(hValue) / Pi64) * 180); }
	
	ColorLCH_t result = {};
	result.l = colorLab.l;
	result.c = SqrtR64(colorLab.a*colorLab.a + colorLab.b*colorLab.b);
	result.h = hValue;
	result.alpha = colorLab.alpha;
	return result;
}
ColorLAB_t ColorLabFromLch(ColorLCH_t colorLch)
{
	ColorLAB_t result = {};
	result.l = colorLch.l;
	result.a = CosR64(ToRadians64(colorLch.h)) * colorLch.c;
	result.b = SinR64(ToRadians64(colorLch.h)) * colorLch.c;
	result.alpha = colorLch.alpha;
	return result;
}

// +--------------------------------------------------------------+
// |                  Complicated Manipulations                   |
// +--------------------------------------------------------------+
// +==============================+
// |           Color_t            |
// +==============================+
Color_t ColorComplementary(Color_t color)
{
	ColorHSV_t colorHueFlipped = ColorHSVFromRGB(color);
	colorHueFlipped.hue = ModR32(colorHueFlipped.hue + 180, 360);
	ColorHSV_t colorValueFlipped = colorHueFlipped;
	colorValueFlipped.v = DecimalPartR32(colorHueFlipped.v + 0.5f);
	Color_t result = ColorRGBFromHSV(colorHueFlipped);
	Color_t valueFlippedResult = ColorRGBFromHSV(colorValueFlipped);
	result = ColorLerp(result, valueFlippedResult, 1-colorHueFlipped.s);
	return result;
}
Color_t ColorComplementaryOld(Color_t color)
{
	Color_t result;
	result.r = (u8)(((u32)color.r + 128) % 256);
	result.g = (u8)(((u32)color.g + 128) % 256);
	result.b = (u8)(((u32)color.b + 128) % 256);
	result.a = color.a;
	return result;
}

Color_t ColorDesaturate(Color_t color, r32 saturation)
{
	v3 colorVec = ToVec3(color);
	u8 intensity = ClampI32toU8(RoundR32i(Vec3Dot(colorVec, NewVec3(0.2125f, 0.7154f, 0.0721f)) * 255));
	return ColorLerp(NewColor(intensity, intensity, intensity, color.a), color, saturation);
}

// +==============================+
// |           Colorf_t           |
// +==============================+
Colorf_t ColorfComplementary(Colorf_t color)
{
	Colorf_t result;
	result.r = DecimalPartR32(color.r + 0.5f);
	result.g = DecimalPartR32(color.g + 0.5f);
	result.b = DecimalPartR32(color.b + 0.5f);
	result.a = color.a;
	return result;
}

Colorf_t ColorfDesaturate(Colorf_t color, r32 saturation)
{
	v3 colorVec = ToVec3(color);
	r32 intensity = Vec3Dot(colorVec, NewVec3(0.2125f, 0.7154f, 0.0721f));
	return ColorfLerp(NewColorf(intensity, intensity, intensity, color.a), color, saturation);
}

// +--------------------------------------------------------------+
// |                      Operator Overloads                      |
// +--------------------------------------------------------------+
bool operator == (Color_t left, Color_t right) { return  (left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a); }
bool operator != (Color_t left, Color_t right) { return !(left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a); }

bool operator == (Colorf_t left, Colorf_t right) { return  (left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a); }
bool operator != (Colorf_t left, Colorf_t right) { return !(left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a); }

bool operator == (ColorHSV_t left, ColorHSV_t right) { return  (left.h == right.h && left.s == right.s && left.v == right.v && left.a == right.a); }
bool operator != (ColorHSV_t left, ColorHSV_t right) { return !(left.h == right.h && left.s == right.s && left.v == right.v && left.a == right.a); }

bool operator == (ColorXYZ_t left, ColorXYZ_t right) { return  (left.x == right.x && left.y == right.y && left.z == right.z && left.a == right.a); }
bool operator != (ColorXYZ_t left, ColorXYZ_t right) { return !(left.x == right.x && left.y == right.y && left.z == right.z && left.a == right.a); }

bool operator == (ColorLCH_t left, ColorLCH_t right) { return  (left.l == right.l && left.c == right.c && left.h == right.h && left.a == right.a); }
bool operator != (ColorLCH_t left, ColorLCH_t right) { return !(left.l == right.l && left.c == right.c && left.h == right.h && left.a == right.a); }

bool operator == (ColorLAB_t left, ColorLAB_t right) { return  (left.l == right.l && left.a == right.a && left.b == right.b && left.b == right.b); }
bool operator != (ColorLAB_t left, ColorLAB_t right) { return !(left.l == right.l && left.a == right.a && left.b == right.b && left.b == right.b); }

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_COLORS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
Color_t
Colorf_t
ColorHSV_t
ColorXYZ_t
ColorLCH_t
ColorLAB_t
ColorObserver_t
ColorIlluminant_t
@Globals
ColorIllumValues
@Functions
const char* GetColorObserverStr(ColorObserver_t enumValue);
const char* GetColorIlluminantStr(ColorIlluminant_t enumValue);
Color_t NewColor(u8 r, u8 g, u8 b)
Colorf_t NewColorf(r32 r, r32 g, r32 b)
ColorHSV_t NewColorHsv(r32 hue, r32 saturation, r32 value)
Color_t ToColor(Colorf_t colorf)
Color_t ColorTransparent(Color_t color, r32 alpha)
Colorf_t ToColorf(Color_t color)
ColorHSV_t ToColorHsv(v3 vector3)
u8 ColorChannelToU8(r32 valueR32)
r32 ColorChannelToR32(u8 valueU8)
u8 MultiplyColorChannelU8(u8 left, u8 right)
u8 MultiplyColorChannelR32(u8 left, r32 rightR32)
Color_t ColorLerp(Color_t start, Color_t end, r32 amount)
Color_t ColorDarken(Color_t color, u8 amount)
Color_t ColorLighten(Color_t color, u8 amount)
Color_t ColorMultiply(Color_t color1, Color_t color2)
Color_t ColorMultiplyAlpha(Color_t color, u8 alphaValue)
Color_t ColorMultiplyAlphaR32(Color_t color, r32 amount)
Color_t ColorOpposite(Color_t color)
bool BasicallyEqualColorf(Colorf_t left, Colorf_t right, r32 tolerance = 0.001f)
Colorf_t ColorfLerp(Colorf_t start, Colorf_t end, r32 amount)
Colorf_t ColorfDarken(Colorf_t color, r32 amount)
Colorf_t ColorfLighten(Colorf_t color, r32 amount)
Colorf_t ColorfMultiply(Colorf_t color1, Colorf_t color2)
Colorf_t ColorfMultiplyAlpha(Colorf_t color, r32 alphaValue)
Colorf_t ColorfOpposite(Colorf_t color)
Color_t ColorRGBFromHSV(ColorHSV_t colorHsv)
ColorHSV_t ColorHSVFromRGB(Color_t color)
Color_t ColorComplementary(Color_t color)
Color_t ColorComplementaryOld(Color_t color)
Color_t ColorDesaturate(Color_t color, r32 saturation)
Colorf_t ColorfComplementary(Colorf_t color)
Colorf_t ColorfDesaturate(Colorf_t color, r32 saturation)
*/