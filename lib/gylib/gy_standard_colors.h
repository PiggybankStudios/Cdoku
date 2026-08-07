/*
File:   gy_standard_colors.h
Author: Taylor Robbins
Date:   09\26\2021
Description:
	** Holds a bunch of standard named colors
*/

#ifndef _GY_STANDARD_COLORS_H
#define _GY_STANDARD_COLORS_H

#include "gy_colors.h"

// +--------------------------------------------------------------+
// |                         Pure Colors                          |
// +--------------------------------------------------------------+
	#define NUM_PREDEF_PURE_COLORS       6
	#define NUM_PREDEF_GREYSCALE_COLORS  16
	
	#define NoColor_Value                 0x00000000UL
	#define TransparentBlack_Value        0x00000000UL
	#define Transparent_Value             0x00FFFFFFUL
	#define TransparentWhite_Value        0x00FFFFFFUL
	
	#define Black_Value                   0xFF000000UL
	#define Grey1_Value                   0xFF111111UL
	#define Grey2_Value                   0xFF222222UL
	#define Grey3_Value                   0xFF333333UL
	#define Grey4_Value                   0xFF444444UL
	#define Grey5_Value                   0xFF555555UL
	#define Grey6_Value                   0xFF666666UL
	#define Grey7_Value                   0xFF777777UL
	#define Grey8_Value                   0xFF888888UL
	#define Grey9_Value                   0xFF999999UL
	#define Grey10_Value                  0xFFAAAAAAUL
	#define Grey11_Value                  0xFFBBBBBBUL
	#define Grey12_Value                  0xFFCCCCCCUL
	#define Grey13_Value                  0xFFDDDDDDUL
	#define Grey14_Value                  0xFFEEEEEEUL
	#define White_Value                   0xFFFFFFFFUL
	
	#define PureRed_Value                 0xFFFF0000UL
	#define PureOrange_Value              0xFFFFA500UL
	#define PureYellow_Value              0xFFFFFF00UL
	#define PureGreen_Value               0xFF008000UL
	#define PureBlue_Value                0xFF0000FFUL
	#define PurePurple_Value              0xFF800080UL
	
	#define NoColor                 NewColor(NoColor_Value)
	#define TransparentBlack        NewColor(TransparentBlack_Value)
	#define Transparent             NewColor(Transparent_Value)
	#define TransparentWhite        NewColor(TransparentWhite_Value)
	#define Black                   NewColor(Black_Value)
	#define Grey1                   NewColor(Grey1_Value)
	#define Grey2                   NewColor(Grey2_Value)
	#define Grey3                   NewColor(Grey3_Value)
	#define Grey4                   NewColor(Grey4_Value)
	#define Grey5                   NewColor(Grey5_Value)
	#define Grey6                   NewColor(Grey6_Value)
	#define Grey7                   NewColor(Grey7_Value)
	#define Grey8                   NewColor(Grey8_Value)
	#define Grey9                   NewColor(Grey9_Value)
	#define Grey10                  NewColor(Grey10_Value)
	#define Grey11                  NewColor(Grey11_Value)
	#define Grey12                  NewColor(Grey12_Value)
	#define Grey13                  NewColor(Grey13_Value)
	#define Grey14                  NewColor(Grey14_Value)
	#define White                   NewColor(White_Value)
	#define PureRed                 NewColor(PureRed_Value)
	#define PureOrange              NewColor(PureOrange_Value)
	#define PureYellow              NewColor(PureYellow_Value)
	#define PureGreen               NewColor(PureGreen_Value)
	#define PureBlue                NewColor(PureBlue_Value)
	#define PurePurple              NewColor(PurePurple_Value)
	
	#ifdef GYLIB_HEADER_ONLY
		Color_t GetPureColorByIndex(u64 index);
		Color_t GetGreyscaleColorByIndex(u64 index);
		u64 GetPureColorIndex(u64 colorValue);
		u64 GetPureColorIndex(Color_t color);
		u64 GetGreyscaleColorIndex(u64 colorValue);
		u64 GetGreyscaleColorIndex(Color_t color);
	#else
	Color_t GetPureColorByIndex(u64 index)
	{
		switch (index % NUM_PREDEF_PURE_COLORS)
		{
			case 0:  return PureRed;
			case 1:  return PureOrange;
			case 2:  return PureYellow;
			case 3:  return PureGreen;
			case 4:  return PureBlue;
			case 5:  return PurePurple;
			default: return PureRed;
		}
	}
	Color_t GetGreyscaleColorByIndex(u64 index)
	{
		switch (index % NUM_PREDEF_GREYSCALE_COLORS)
		{
			case 0:  return Black;
			case 1:  return Grey1;
			case 2:  return Grey2;
			case 3:  return Grey3;
			case 4:  return Grey4;
			case 5:  return Grey5;
			case 6:  return Grey6;
			case 7:  return Grey7;
			case 8:  return Grey8;
			case 9:  return Grey9;
			case 10: return Grey10;
			case 11: return Grey11;
			case 12: return Grey12;
			case 13: return Grey13;
			case 14: return Grey14;
			case 15: return White;
			default: return Black;
		}
	}
	u64 GetPureColorIndex(u64 colorValue)
	{
		switch (colorValue)
		{
			case PureRed_Value:     return 0;
			case PureOrange_Value:  return 1;
			case PureYellow_Value:  return 2;
			case PureGreen_Value:   return 3;
			case PureBlue_Value:    return 4;
			case PurePurple_Value:  return 5;
			default: return NUM_PREDEF_PURE_COLORS;
		}
	}
	u64 GetPureColorIndex(Color_t color)
	{
		return GetPureColorIndex(color.value);
	}
	u64 GetGreyscaleColorIndex(u64 colorValue)
	{
		switch (colorValue)
		{
			case Black_Value:  return 0;
			case Grey1_Value:  return 1;
			case Grey2_Value:  return 2;
			case Grey3_Value:  return 3;
			case Grey4_Value:  return 4;
			case Grey5_Value:  return 5;
			case Grey6_Value:  return 6;
			case Grey7_Value:  return 7;
			case Grey8_Value:  return 8;
			case Grey9_Value:  return 9;
			case Grey10_Value: return 10;
			case Grey11_Value: return 11;
			case Grey12_Value: return 12;
			case Grey13_Value: return 13;
			case Grey14_Value: return 14;
			case White_Value:  return 15;
			default: return NUM_PREDEF_GREYSCALE_COLORS;
		}
	}
	u64 GetGreyscaleColorIndex(Color_t color)
	{
		return GetGreyscaleColorIndex(color.value);
	}
	#endif
// +--------------------------------------------------------------+

// +--------------------------------------------------------------+
// |                        Palette Colors                        |
// +--------------------------------------------------------------+
	#define NUM_PREDEF_PAL_COLORS       70
	
	#define PalRedDarker_Value            0xFFB91668
	#define PalRedDark_Value              0xFFE1126B
	#define PalRed_Value                  0xFFF92672
	#define PalRedLight_Value             0xFFFB6C7F
	#define PalRedLighter_Value           0xFFFDB5A8
	#define PalPinkDarker_Value           0xFFBF2A83
	#define PalPinkDark_Value             0xFFD9368B
	#define PalPink_Value                 0xFFED4E95
	#define PalPinkLight_Value            0xFFF088B8
	#define PalPinkLighter_Value          0xFFF6ACCD
	#define PalOrangeDarker_Value         0xFFBD7628
	#define PalOrangeDark_Value           0xFFE4861C
	#define PalOrange_Value               0xFFFD971F
	#define PalOrangeLight_Value          0xFFFEB64B
	#define PalOrangeLighter_Value        0xFFFED88F
	#define PalHoneyDarker_Value          0xFFD5A43E
	#define PalHoneyDark_Value            0xFFE9B34A
	#define PalHoney_Value                0xFFFDC459
	#define PalHoneyLight_Value           0xFFFED47A
	#define PalHoneyLighter_Value         0xFFFEE8AC
	#define PalYellowDarker_Value         0xFFB3B947
	#define PalYellowDark_Value           0xFFD5CE62
	#define PalYellow_Value               0xFFE6DB74
	#define PalYellowLight_Value          0xFFEFE48C
	#define PalYellowLighter_Value        0xFFF3ECAC
	#define PalBananaDarker_Value         0xFFB7BD2F
	#define PalBananaDark_Value           0xFFDAD942
	#define PalBanana_Value               0xFFEEE64F
	#define PalBananaLight_Value          0xFFF4EA6F
	#define PalBananaLighter_Value        0xFFF9F1A4
	#define PalGreenDarker_Value          0xFF76B434
	#define PalGreenDark_Value            0xFF8DCE2E
	#define PalGreen_Value                0xFFA6E22E
	#define PalGreenLight_Value           0xFFC3E753
	#define PalGreenLighter_Value         0xFFE1EE92
	#define PalGrassDarker_Value          0xFF608B4D
	#define PalGrassDark_Value            0xFF7DA641
	#define PalGrass_Value                0xFF92BB41
	#define PalGrassLight_Value           0xFFB3D05B
	#define PalGrassLighter_Value         0xFFD7E295
	#define PalBlueDarker_Value           0xFF2471EC
	#define PalBlueDark_Value             0xFF4585EE
	#define PalBlue_Value                 0xFF669BEF
	#define PalBlueLight_Value            0xFF99B7DE
	#define PalBlueLighter_Value          0xFFCDD9DB
	#define PalSkyDarker_Value            0xFF24B1EC
	#define PalSkyDark_Value              0xFF45C9EE
	#define PalSky_Value                  0xFF66D9EF
	#define PalSkyLight_Value             0xFF99E1DE
	#define PalSkyLighter_Value           0xFFCDEDDB
	#define PalPurpleDarker_Value         0xFF7445FF
	#define PalPurpleDark_Value           0xFF9161FF
	#define PalPurple_Value               0xFFAE81FF
	#define PalPurpleLight_Value          0xFFCBAAEA
	#define PalPurpleLighter_Value        0xFFD6BBEE
	#define PalVioletDarker_Value         0xFFC038FF
	#define PalVioletDark_Value           0xFFC95BFF
	#define PalViolet_Value               0xFFDD7BFF
	#define PalVioletLight_Value          0xFFE595FF
	#define PalVioletLighter_Value        0xFFEBAAFF
	#define PalNeutralDarker_Value        0xFF5E5E4A
	#define PalNeutralDark_Value          0xFF75715E
	#define PalNeutral_Value              0xFF908861
	#define PalNeutralLight_Value         0xFFA79C65
	#define PalNeutralLighter_Value       0xFFB9B084
	
	#define PalBackgroundDarker_Value     0xFF131410
	#define PalBackgroundDark_Value       0xFF1F221A
	#define PalBackground_Value           0xFF272822
	#define PalBackgroundLight_Value      0xFF424431
	#define PalBackgroundLighter_Value    0xFF545934
	#define PalGreyDarker_Value           0xFF64635B
	#define PalGreyDark_Value             0xFF707067
	#define PalGrey_Value                 0xFF7D7C72
	#define PalGreyLight_Value            0xFF98978E
	#define PalGreyLighter_Value          0xFFB1B1AA
	#define PalBlackDarker_Value          0xFF121211
	#define PalBlackDark_Value            0xFF1E1E1D
	#define PalBlack_Value                0xFF262624
	#define PalBlackLight_Value           0xFF3C3C39
	#define PalBlackLighter_Value         0xFF53534E
	
	#define PalRedDarker            NewColor(PalRedDarker_Value)
	#define PalRedDark              NewColor(PalRedDark_Value)
	#define PalRed                  NewColor(PalRed_Value)
	#define PalRedLight             NewColor(PalRedLight_Value)
	#define PalRedLighter           NewColor(PalRedLighter_Value)
	#define PalPinkDarker           NewColor(PalPinkDarker_Value)
	#define PalPinkDark             NewColor(PalPinkDark_Value)
	#define PalPink                 NewColor(PalPink_Value)
	#define PalPinkLight            NewColor(PalPinkLight_Value)
	#define PalPinkLighter          NewColor(PalPinkLighter_Value)
	#define PalOrangeDarker         NewColor(PalOrangeDarker_Value)
	#define PalOrangeDark           NewColor(PalOrangeDark_Value)
	#define PalOrange               NewColor(PalOrange_Value)
	#define PalOrangeLight          NewColor(PalOrangeLight_Value)
	#define PalOrangeLighter        NewColor(PalOrangeLighter_Value)
	#define PalHoneyDarker          NewColor(PalHoneyDarker_Value)
	#define PalHoneyDark            NewColor(PalHoneyDark_Value)
	#define PalHoney                NewColor(PalHoney_Value)
	#define PalHoneyLight           NewColor(PalHoneyLight_Value)
	#define PalHoneyLighter         NewColor(PalHoneyLighter_Value)
	#define PalYellowDarker         NewColor(PalYellowDarker_Value)
	#define PalYellowDark           NewColor(PalYellowDark_Value)
	#define PalYellow               NewColor(PalYellow_Value)
	#define PalYellowLight          NewColor(PalYellowLight_Value)
	#define PalYellowLighter        NewColor(PalYellowLighter_Value)
	#define PalBananaDarker         NewColor(PalBananaDarker_Value)
	#define PalBananaDark           NewColor(PalBananaDark_Value)
	#define PalBanana               NewColor(PalBanana_Value)
	#define PalBananaLight          NewColor(PalBananaLight_Value)
	#define PalBananaLighter        NewColor(PalBananaLighter_Value)
	#define PalGreenDarker          NewColor(PalGreenDarker_Value)
	#define PalGreenDark            NewColor(PalGreenDark_Value)
	#define PalGreen                NewColor(PalGreen_Value)
	#define PalGreenLight           NewColor(PalGreenLight_Value)
	#define PalGreenLighter         NewColor(PalGreenLighter_Value)
	#define PalGrassDarker          NewColor(PalGrassDarker_Value)
	#define PalGrassDark            NewColor(PalGrassDark_Value)
	#define PalGrass                NewColor(PalGrass_Value)
	#define PalGrassLight           NewColor(PalGrassLight_Value)
	#define PalGrassLighter         NewColor(PalGrassLighter_Value)
	#define PalBlueDarker           NewColor(PalBlueDarker_Value)
	#define PalBlueDark             NewColor(PalBlueDark_Value)
	#define PalBlue                 NewColor(PalBlue_Value)
	#define PalBlueLight            NewColor(PalBlueLight_Value)
	#define PalBlueLighter          NewColor(PalBlueLighter_Value)
	#define PalSkyDarker            NewColor(PalSkyDarker_Value)
	#define PalSkyDark              NewColor(PalSkyDark_Value)
	#define PalSky                  NewColor(PalSky_Value)
	#define PalSkyLight             NewColor(PalSkyLight_Value)
	#define PalSkyLighter           NewColor(PalSkyLighter_Value)
	#define PalPurpleDarker         NewColor(PalPurpleDarker_Value)
	#define PalPurpleDark           NewColor(PalPurpleDark_Value)
	#define PalPurple               NewColor(PalPurple_Value)
	#define PalPurpleLight          NewColor(PalPurpleLight_Value)
	#define PalPurpleLighter        NewColor(PalPurpleLighter_Value)
	#define PalVioletDarker         NewColor(PalVioletDarker_Value)
	#define PalVioletDark           NewColor(PalVioletDark_Value)
	#define PalViolet               NewColor(PalViolet_Value)
	#define PalVioletLight          NewColor(PalVioletLight_Value)
	#define PalVioletLighter        NewColor(PalVioletLighter_Value)
	#define PalNeutralDarker        NewColor(PalNeutralDarker_Value)
	#define PalNeutralDark          NewColor(PalNeutralDark_Value)
	#define PalNeutral              NewColor(PalNeutral_Value)
	#define PalNeutralLight         NewColor(PalNeutralLight_Value)
	#define PalNeutralLighter       NewColor(PalNeutralLighter_Value)
	
	#define PalBackgroundDarker     NewColor(PalBackgroundDarker_Value)
	#define PalBackgroundDark       NewColor(PalBackgroundDark_Value)
	#define PalBackground           NewColor(PalBackground_Value)
	#define PalBackgroundLight      NewColor(PalBackgroundLight_Value)
	#define PalBackgroundLighter    NewColor(PalBackgroundLighter_Value)
	#define PalGreyDarker           NewColor(PalGreyDarker_Value)
	#define PalGreyDark             NewColor(PalGreyDark_Value)
	#define PalGrey                 NewColor(PalGrey_Value)
	#define PalGreyLight            NewColor(PalGreyLight_Value)
	#define PalGreyLighter          NewColor(PalGreyLighter_Value)
	#define PalBlackDarker          NewColor(PalBlackDarker_Value)
	#define PalBlackDark            NewColor(PalBlackDark_Value)
	#define PalBlack                NewColor(PalBlack_Value)
	#define PalBlackLight           NewColor(PalBlackLight_Value)
	#define PalBlackLighter         NewColor(PalBlackLighter_Value)

	#ifdef GYLIB_HEADER_ONLY
		Color_t GetPredefPalColorByIndex(u64 index);
		u64 GetPredefPalColorIndex(u64 colorValue);
		u64 GetPredefPalColorIndex(Color_t color);
	#else
	Color_t GetPredefPalColorByIndex(u64 index)
	{
		switch (index % NUM_PREDEF_PAL_COLORS)
		{
			case 0:  return PalRed;
			case 1:  return PalOrange;
			case 2:  return PalYellow;
			case 3:  return PalGreen;
			case 4:  return PalBlue;
			case 5:  return PalPurple;
			case 6:  return PalNeutral;
			
			case 7:  return PalRedLight;
			case 8:  return PalOrangeLight;
			case 9:  return PalYellowLight;
			case 10: return PalGreenLight;
			case 11: return PalBlueLight;
			case 12: return PalPurpleLight;
			case 13: return PalNeutralLight;
			
			case 14: return PalRedDark;
			case 15: return PalOrangeDark;
			case 16: return PalYellowDark;
			case 17: return PalGreenDark;
			case 18: return PalBlueDark;
			case 19: return PalPurpleDark;
			case 20: return PalNeutralDark;
			
			case 21: return PalPink;
			case 22: return PalHoney;
			case 23: return PalBanana;
			case 24: return PalGrass;
			case 25: return PalSky;
			case 26: return PalViolet;
			case 27: return PalGrey;
			
			case 28: return PalPinkLight;
			case 29: return PalHoneyLight;
			case 30: return PalBananaLight;
			case 31: return PalGrassLight;
			case 32: return PalSkyLight;
			case 33: return PalVioletLight;
			case 34: return PalGreyLight;
			
			case 35: return PalPinkDark;
			case 36: return PalHoneyDark;
			case 37: return PalBananaDark;
			case 38: return PalGrassDark;
			case 39: return PalSkyDark;
			case 40: return PalVioletDark;
			case 41: return PalGreyDark;
			
			case 42: return PalRedLighter;
			case 43: return PalOrangeLighter;
			case 44: return PalYellowLighter;
			case 45: return PalGreenLighter;
			case 46: return PalBlueLighter;
			case 47: return PalPurpleLighter;
			case 48: return PalNeutralLighter;
			
			case 49: return PalPinkLighter;
			case 50: return PalHoneyLighter;
			case 51: return PalBananaLighter;
			case 52: return PalGrassLighter;
			case 53: return PalSkyLighter;
			case 54: return PalVioletLighter;
			case 55: return PalGreyLighter;
			
			case 56: return PalRedDarker;
			case 57: return PalOrangeDarker;
			case 58: return PalYellowDarker;
			case 59: return PalGreenDarker;
			case 60: return PalBlueDarker;
			case 61: return PalPurpleDarker;
			case 62: return PalNeutralDarker;
			
			case 63: return PalPinkDarker;
			case 64: return PalHoneyDarker;
			case 65: return PalBananaDarker;
			case 66: return PalGrassDarker;
			case 67: return PalSkyDarker;
			case 68: return PalVioletDarker;
			case 69: return PalGreyDarker;
			
			default: return Black;
		}
	}
	u64 GetPredefPalColorIndex(u64 colorValue)
	{
		switch (colorValue)
		{
			case PalRed_Value:            return 0;
			case PalOrange_Value:         return 1;
			case PalYellow_Value:         return 2;
			case PalGreen_Value:          return 3;
			case PalBlue_Value:           return 4;
			case PalPurple_Value:         return 5;
			case PalNeutral_Value:        return 6;
			case PalRedLight_Value:       return 7;
			case PalOrangeLight_Value:    return 8;
			case PalYellowLight_Value:    return 9;
			case PalGreenLight_Value:     return 10;
			case PalBlueLight_Value:      return 11;
			case PalPurpleLight_Value:    return 12;
			case PalNeutralLight_Value:   return 13;
			case PalRedDark_Value:        return 14;
			case PalOrangeDark_Value:     return 15;
			case PalYellowDark_Value:     return 16;
			case PalGreenDark_Value:      return 17;
			case PalBlueDark_Value:       return 18;
			case PalPurpleDark_Value:     return 19;
			case PalNeutralDark_Value:    return 20;
			case PalPink_Value:           return 21;
			case PalHoney_Value:          return 22;
			case PalBanana_Value:         return 23;
			case PalGrass_Value:          return 24;
			case PalSky_Value:            return 25;
			case PalViolet_Value:         return 26;
			case PalGrey_Value:           return 27;
			case PalPinkLight_Value:      return 28;
			case PalHoneyLight_Value:     return 29;
			case PalBananaLight_Value:    return 30;
			case PalGrassLight_Value:     return 31;
			case PalSkyLight_Value:       return 32;
			case PalVioletLight_Value:    return 33;
			case PalGreyLight_Value:      return 34;
			case PalPinkDark_Value:       return 35;
			case PalHoneyDark_Value:      return 36;
			case PalBananaDark_Value:     return 37;
			case PalGrassDark_Value:      return 38;
			case PalSkyDark_Value:        return 39;
			case PalVioletDark_Value:     return 40;
			case PalGreyDark_Value:       return 41;
			case PalRedLighter_Value:     return 42;
			case PalOrangeLighter_Value:  return 43;
			case PalYellowLighter_Value:  return 44;
			case PalGreenLighter_Value:   return 45;
			case PalBlueLighter_Value:    return 46;
			case PalPurpleLighter_Value:  return 47;
			case PalNeutralLighter_Value: return 48;
			case PalPinkLighter_Value:    return 49;
			case PalHoneyLighter_Value:   return 50;
			case PalBananaLighter_Value:  return 51;
			case PalGrassLighter_Value:   return 52;
			case PalSkyLighter_Value:     return 53;
			case PalVioletLighter_Value:  return 54;
			case PalGreyLighter_Value:    return 55;
			case PalRedDarker_Value:      return 56;
			case PalOrangeDarker_Value:   return 57;
			case PalYellowDarker_Value:   return 58;
			case PalGreenDarker_Value:    return 59;
			case PalBlueDarker_Value:     return 60;
			case PalPurpleDarker_Value:   return 61;
			case PalNeutralDarker_Value:  return 62;
			case PalPinkDarker_Value:     return 63;
			case PalHoneyDarker_Value:    return 64;
			case PalBananaDarker_Value:   return 65;
			case PalGrassDarker_Value:    return 66;
			case PalSkyDarker_Value:      return 67;
			case PalVioletDarker_Value:   return 68;
			case PalGreyDarker_Value:     return 69;
			default: return NUM_PREDEF_PAL_COLORS;
		}
	}
	u64 GetPredefPalColorIndex(Color_t color)
	{
		return GetPredefPalColorIndex(color.value);
	}
	#endif
// +--------------------------------------------------------------+

// +--------------------------------------------------------------+
// |                        Monokai Colors                        |
// +--------------------------------------------------------------+
	#define NUM_PREDEF_MONOKAI_COLORS       20
	
	#define MonokaiBack_Value               0xFF3B3A32
	#define MonokaiYellow_Value             0xFFE6DB74
	#define MonokaiLightYellow_Value        0xFFFFE792
	#define MonokaiFadedYellow_Value        0xFFFFEFB7
	#define MonokaiPurple_Value             0xFFAE81FF
	#define MonokaiLightPurple_Value        0xFFE777FF
	#define MonokaiGreen_Value              0xFFA6E22E
	#define MonokaiDarkGreen_Value          0xFF829520
	#define MonokaiOrange_Value             0xFFFD971F
	#define MonokaiBrown_Value              0xFF9D550F
	#define MonokaiMagenta_Value            0xFFF92672
	#define MonokaiRed_Value                0xFFF83333
	#define MonokaiLightRed_Value           0xFFFF5959
	#define MonokaiBlue_Value               0xFF66D9EF
	#define MonokaiLightBlue_Value          0xFFA9FFFF
	#define MonokaiWhite_Value              0xFFF8F8F2
	#define MonokaiLightGray_Value          0xFFBBBBBB
	#define MonokaiGray1_Value              0xFFAFAFA2
	#define MonokaiGray2_Value              0xFF75715E
	#define MonokaiDarkGray_Value           0xFF212121

	#define MonokaiBack                     NewColor(MonokaiBack_Value)
	#define MonokaiYellow                   NewColor(MonokaiYellow_Value)
	#define MonokaiLightYellow              NewColor(MonokaiLightYellow_Value)
	#define MonokaiFadedYellow              NewColor(MonokaiFadedYellow_Value)
	#define MonokaiPurple                   NewColor(MonokaiPurple_Value)
	#define MonokaiLightPurple              NewColor(MonokaiLightPurple_Value)
	#define MonokaiGreen                    NewColor(MonokaiGreen_Value)
	#define MonokaiDarkGreen                NewColor(MonokaiDarkGreen_Value)
	#define MonokaiOrange                   NewColor(MonokaiOrange_Value)
	#define MonokaiBrown                    NewColor(MonokaiBrown_Value)
	#define MonokaiMagenta                  NewColor(MonokaiMagenta_Value)
	#define MonokaiRed                      NewColor(MonokaiRed_Value)
	#define MonokaiLightRed                 NewColor(MonokaiLightRed_Value)
	#define MonokaiBlue                     NewColor(MonokaiBlue_Value)
	#define MonokaiLightBlue                NewColor(MonokaiLightBlue_Value)
	#define MonokaiWhite                    NewColor(MonokaiWhite_Value)
	#define MonokaiLightGray                NewColor(MonokaiLightGray_Value)
	#define MonokaiGray1                    NewColor(MonokaiGray1_Value)
	#define MonokaiGray2                    NewColor(MonokaiGray2_Value)
	#define MonokaiDarkGray                 NewColor(MonokaiDarkGray_Value)
// +--------------------------------------------------------------+

// +--------------------------------------------------------------+
// |                       Other Functions                        |
// +--------------------------------------------------------------+
#ifdef _GY_DEBUG_H
#ifdef GYLIB_HEADER_ONLY
Color_t GetDbgLevelTextColor(DbgLevel_t dbgLevel);
#else
Color_t GetDbgLevelTextColor(DbgLevel_t dbgLevel)
{
	switch (dbgLevel)
	{
		case DbgLevel_Debug:   return MonokaiGray1;     break;
		case DbgLevel_Regular: return MonokaiWhite;     break;
		case DbgLevel_Info:    return MonokaiGreen;     break;
		case DbgLevel_Notify:  return MonokaiPurple;    break;
		case DbgLevel_Other:   return MonokaiLightBlue; break;
		case DbgLevel_Warning: return MonokaiOrange;    break;
		case DbgLevel_Error:   return MonokaiMagenta;   break;
		default: return MonokaiWhite; break;
	}
}
#endif
#endif //_GY_DEBUG_H

#endif //  _GY_STANDARD_COLORS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
NUM_PREDEF_PURE_COLORS
NUM_PREDEF_GREYSCALE_COLORS
NoColor_Value
TransparentBlack_Value
Transparent_Value
TransparentWhite_Value
Black_Value
Grey1_Value
Grey2_Value
Grey3_Value
Grey4_Value
Grey5_Value
Grey6_Value
Grey7_Value
Grey8_Value
Grey9_Value
Grey10_Value
Grey11_Value
Grey12_Value
Grey13_Value
Grey14_Value
White_Value
PureRed_Value
PureOrange_Value
PureYellow_Value
PureGreen_Value
PureBlue_Value
PurePurple_Value
NoColor
TransparentBlack
Transparent
TransparentWhite
Black
Grey1
Grey2
Grey3
Grey4
Grey5
Grey6
Grey7
Grey8
Grey9
Grey10
Grey11
Grey12
Grey13
Grey14
White
PureRed
PureOrange
PureYellow
PureGreen
PureBlue
PurePurple
NUM_PREDEF_PAL_COLORS
PalRedDarker_Value
PalRedDark_Value
PalRed_Value
PalRedLight_Value
PalRedLighter_Value
PalPinkDarker_Value
PalPinkDark_Value
PalPink_Value
PalPinkLight_Value
PalPinkLighter_Value
PalOrangeDarker_Value
PalOrangeDark_Value
PalOrange_Value
PalOrangeLight_Value
PalOrangeLighter_Value
PalHoneyDarker_Value
PalHoneyDark_Value
PalHoney_Value
PalHoneyLight_Value
PalHoneyLighter_Value
PalYellowDarker_Value
PalYellowDark_Value
PalYellow_Value
PalYellowLight_Value
PalYellowLighter_Value
PalBananaDarker_Value
PalBananaDark_Value
PalBanana_Value
PalBananaLight_Value
PalBananaLighter_Value
PalGreenDarker_Value
PalGreenDark_Value
PalGreen_Value
PalGreenLight_Value
PalGreenLighter_Value
PalGrassDarker_Value
PalGrassDark_Value
PalGrass_Value
PalGrassLight_Value
PalGrassLighter_Value
PalBlueDarker_Value
PalBlueDark_Value
PalBlue_Value
PalBlueLight_Value
PalBlueLighter_Value
PalSkyDarker_Value
PalSkyDark_Value
PalSky_Value
PalSkyLight_Value
PalSkyLighter_Value
PalPurpleDarker_Value
PalPurpleDark_Value
PalPurple_Value
PalPurpleLight_Value
PalPurpleLighter_Value
PalVioletDarker_Value
PalVioletDark_Value
PalViolet_Value
PalVioletLight_Value
PalVioletLighter_Value
PalNeutralDarker_Value
PalNeutralDark_Value
PalNeutral_Value
PalNeutralLight_Value
PalNeutralLighter_Value
PalBackgroundDarker_Value
PalBackgroundDark_Value
PalBackground_Value
PalBackgroundLight_Value
PalBackgroundLighter_Value
PalGreyDarker_Value
PalGreyDark_Value
PalGrey_Value
PalGreyLight_Value
PalGreyLighter_Value
PalBlackDarker_Value
PalBlackDark_Value
PalBlack_Value
PalBlackLight_Value
PalBlackLighter_Value
PalRedDarker
PalRedDark
PalRed
PalRedLight
PalRedLighter
PalPinkDarker
PalPinkDark
PalPink
PalPinkLight
PalPinkLighter
PalOrangeDarker
PalOrangeDark
PalOrange
PalOrangeLight
PalOrangeLighter
PalHoneyDarker
PalHoneyDark
PalHoney
PalHoneyLight
PalHoneyLighter
PalYellowDarker
PalYellowDark
PalYellow
PalYellowLight
PalYellowLighter
PalBananaDarker
PalBananaDark
PalBanana
PalBananaLight
PalBananaLighter
PalGreenDarker
PalGreenDark
PalGreen
PalGreenLight
PalGreenLighter
PalGrassDarker
PalGrassDark
PalGrass
PalGrassLight
PalGrassLighter
PalBlueDarker
PalBlueDark
PalBlue
PalBlueLight
PalBlueLighter
PalSkyDarker
PalSkyDark
PalSky
PalSkyLight
PalSkyLighter
PalPurpleDarker
PalPurpleDark
PalPurple
PalPurpleLight
PalPurpleLighter
PalVioletDarker
PalVioletDark
PalViolet
PalVioletLight
PalVioletLighter
PalNeutralDarker
PalNeutralDark
PalNeutral
PalNeutralLight
PalNeutralLighter
PalBackgroundDarker
PalBackgroundDark
PalBackground
PalBackgroundLight
PalBackgroundLighter
PalGreyDarker
PalGreyDark
PalGrey
PalGreyLight
PalGreyLighter
PalBlackDarker
PalBlackDark
PalBlack
PalBlackLight
PalBlackLighter
NUM_PREDEF_MONOKAI_COLORS
MonokaiBack_Value
MonokaiYellow_Value
MonokaiLightYellow_Value
MonokaiFadedYellow_Value
MonokaiPurple_Value
MonokaiLightPurple_Value
MonokaiGreen_Value
MonokaiDarkGreen_Value
MonokaiOrange_Value
MonokaiBrown_Value
MonokaiMagenta_Value
MonokaiRed_Value
MonokaiLightRed_Value
MonokaiBlue_Value
MonokaiLightBlue_Value
MonokaiWhite_Value
MonokaiLightGray_Value
MonokaiGray1_Value
MonokaiGray2_Value
MonokaiDarkGray_Value
MonokaiBack
MonokaiYellow
MonokaiLightYellow
MonokaiFadedYellow
MonokaiPurple
MonokaiLightPurple
MonokaiGreen
MonokaiDarkGreen
MonokaiOrange
MonokaiBrown
MonokaiMagenta
MonokaiRed
MonokaiLightRed
MonokaiBlue
MonokaiLightBlue
MonokaiWhite
MonokaiLightGray
MonokaiGray1
MonokaiGray2
MonokaiDarkGray
@Types
@Functions
Color_t GetPureColorByIndex(u64 index)
Color_t GetGreyscaleColorByIndex(u64 index)
u64 GetPureColorIndex(Color_t color)
u64 GetGreyscaleColorIndex(Color_t color)
Color_t GetPredefPalColorByIndex(u64 index)
u64 GetPredefPalColorIndex(Color_t color)
Color_t GetDbgLevelTextColor(DbgLevel_t dbgLevel)
*/
