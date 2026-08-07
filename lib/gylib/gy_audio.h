/*
File:   gy_audio.h
Author: Taylor Robbins
Date:   01\14\2022
Description:
	** Holds a bunch of helper functions and defines for working audio and sample generation
*/

#ifndef _GY_AUDIO_H
#define _GY_AUDIO_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"

// +--------------------------------------------------------------+
// |                      Frequency Defines                       |
// +--------------------------------------------------------------+
	#define FREQUENCY_C0   16.35 //Hz
	#define FREQUENCY_Db0  17.32 //Hz
	#define FREQUENCY_D0   18.35 //Hz
	#define FREQUENCY_Eb0  19.45 //Hz
	#define FREQUENCY_E0   20.60 //Hz
	#define FREQUENCY_F0   21.83 //Hz
	#define FREQUENCY_Gb0  23.12 //Hz
	#define FREQUENCY_G0   24.50 //Hz
	#define FREQUENCY_Ab0  25.96 //Hz
	#define FREQUENCY_A0   27.50 //Hz
	#define FREQUENCY_Bb0  29.14 //Hz
	#define FREQUENCY_B0   30.87 //Hz
	#define FREQUENCY_C1   32.70 //Hz
	#define FREQUENCY_Db1  34.65 //Hz
	#define FREQUENCY_D1   36.71 //Hz
	#define FREQUENCY_Eb1  38.89 //Hz
	#define FREQUENCY_E1   41.20 //Hz
	#define FREQUENCY_F1   43.65 //Hz
	#define FREQUENCY_Gb1  46.25 //Hz
	#define FREQUENCY_G1   49.00 //Hz
	#define FREQUENCY_Ab1  51.91 //Hz
	#define FREQUENCY_A1   55.00 //Hz
	#define FREQUENCY_Bb1  58.27 //Hz
	#define FREQUENCY_B1   61.74 //Hz
	#define FREQUENCY_C2   65.41 //Hz
	#define FREQUENCY_Db2  69.30 //Hz
	#define FREQUENCY_D2   73.42 //Hz
	#define FREQUENCY_Eb2  77.78 //Hz
	#define FREQUENCY_E2   82.41 //Hz
	#define FREQUENCY_F2   87.31 //Hz
	#define FREQUENCY_Gb2  92.50 //Hz
	#define FREQUENCY_G2   98.00 //Hz
	#define FREQUENCY_Ab2  103.83 //Hz
	#define FREQUENCY_A2   110.00 //Hz
	#define FREQUENCY_Bb2  116.54 //Hz
	#define FREQUENCY_B2   123.47 //Hz
	#define FREQUENCY_C3   130.81 //Hz
	#define FREQUENCY_Db3  138.59 //Hz
	#define FREQUENCY_D3   146.83 //Hz
	#define FREQUENCY_Eb3  155.56 //Hz
	#define FREQUENCY_E3   164.81 //Hz
	#define FREQUENCY_F3   174.61 //Hz
	#define FREQUENCY_Gb3  185.00 //Hz
	#define FREQUENCY_G3   196.00 //Hz
	#define FREQUENCY_Ab3  207.65 //Hz
	#define FREQUENCY_A3   220.00 //Hz
	#define FREQUENCY_Bb3  233.08 //Hz
	#define FREQUENCY_B3   246.94 //Hz
	#define FREQUENCY_C4   261.63 //Hz
	#define FREQUENCY_Db4  277.18 //Hz
	#define FREQUENCY_D4   293.66 //Hz
	#define FREQUENCY_Eb4  311.13 //Hz
	#define FREQUENCY_E4   329.63 //Hz
	#define FREQUENCY_F4   349.23 //Hz
	#define FREQUENCY_Gb4  369.99 //Hz
	#define FREQUENCY_G4   392.00 //Hz
	#define FREQUENCY_Ab4  415.30 //Hz
	#define FREQUENCY_A4   440.00 //Hz
	#define FREQUENCY_Bb4  466.16 //Hz
	#define FREQUENCY_B4   493.88 //Hz
	#define FREQUENCY_C5   523.25 //Hz
	#define FREQUENCY_Db5  554.37 //Hz
	#define FREQUENCY_D5   587.33 //Hz
	#define FREQUENCY_Eb5  622.25 //Hz
	#define FREQUENCY_E5   659.25 //Hz
	#define FREQUENCY_F5   698.46 //Hz
	#define FREQUENCY_Gb5  739.99 //Hz
	#define FREQUENCY_G5   783.99 //Hz
	#define FREQUENCY_Ab5  830.61 //Hz
	#define FREQUENCY_A5   880.00 //Hz
	#define FREQUENCY_Bb5  932.33 //Hz
	#define FREQUENCY_B5   987.77 //Hz
	#define FREQUENCY_C6   1046.50 //Hz
	#define FREQUENCY_Db6  1108.73 //Hz
	#define FREQUENCY_D6   1174.66 //Hz
	#define FREQUENCY_Eb6  1244.51 //Hz
	#define FREQUENCY_E6   1318.51 //Hz
	#define FREQUENCY_F6   1396.91 //Hz
	#define FREQUENCY_Gb6  1479.98 //Hz
	#define FREQUENCY_G6   1567.98 //Hz
	#define FREQUENCY_Ab6  1661.22 //Hz
	#define FREQUENCY_A6   1760.00 //Hz
	#define FREQUENCY_Bb6  1864.66 //Hz
	#define FREQUENCY_B6   1975.53 //Hz
	#define FREQUENCY_C7   2093.00 //Hz
	#define FREQUENCY_Db7  2217.46 //Hz
	#define FREQUENCY_D7   2349.32 //Hz
	#define FREQUENCY_Eb7  2489.02 //Hz
	#define FREQUENCY_E7   2637.02 //Hz
	#define FREQUENCY_F7   2793.83 //Hz
	#define FREQUENCY_Gb7  2959.96 //Hz
	#define FREQUENCY_G7   3135.96 //Hz
	#define FREQUENCY_Ab7  3322.44 //Hz
	#define FREQUENCY_A7   3520.00 //Hz
	#define FREQUENCY_Bb7  3729.31 //Hz
	#define FREQUENCY_B7   3951.07 //Hz
	#define FREQUENCY_C8   4186.01 //Hz
	#define FREQUENCY_Db8  4434.92 //Hz
	#define FREQUENCY_D8   4698.63 //Hz
	#define FREQUENCY_Eb8  4978.03 //Hz
	#define FREQUENCY_E8   5274.04 //Hz
	#define FREQUENCY_F8   5587.65 //Hz
	#define FREQUENCY_Gb8  5919.91 //Hz
	#define FREQUENCY_G8   6271.93 //Hz
	#define FREQUENCY_Ab8  6644.88 //Hz
	#define FREQUENCY_A8   7040.00 //Hz
	#define FREQUENCY_Bb8  7458.62 //Hz
	#define FREQUENCY_B8   7902.13 //Hz

	#define FREQUENCY_MIDDLE_C FREQUENCY_C4 //Hz
// +--------------------------------------------------------------+

#endif //  _GY_AUDIO_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
FREQUENCY_C0
FREQUENCY_Db0
FREQUENCY_D0
FREQUENCY_Eb0
FREQUENCY_E0
FREQUENCY_F0
FREQUENCY_Gb0
FREQUENCY_G0
FREQUENCY_Ab0
FREQUENCY_A0
FREQUENCY_Bb0
FREQUENCY_B0
FREQUENCY_C1
FREQUENCY_Db1
FREQUENCY_D1
FREQUENCY_Eb1
FREQUENCY_E1
FREQUENCY_F1
FREQUENCY_Gb1
FREQUENCY_G1
FREQUENCY_Ab1
FREQUENCY_A1
FREQUENCY_Bb1
FREQUENCY_B1
FREQUENCY_C2
FREQUENCY_Db2
FREQUENCY_D2
FREQUENCY_Eb2
FREQUENCY_E2
FREQUENCY_F2
FREQUENCY_Gb2
FREQUENCY_G2
FREQUENCY_Ab2
FREQUENCY_A2
FREQUENCY_Bb2
FREQUENCY_B2
FREQUENCY_C3
FREQUENCY_Db3
FREQUENCY_D3
FREQUENCY_Eb3
FREQUENCY_E3
FREQUENCY_F3
FREQUENCY_Gb3
FREQUENCY_G3
FREQUENCY_Ab3
FREQUENCY_A3
FREQUENCY_Bb3
FREQUENCY_B3
FREQUENCY_C4
FREQUENCY_Db4
FREQUENCY_D4
FREQUENCY_Eb4
FREQUENCY_E4
FREQUENCY_F4
FREQUENCY_Gb4
FREQUENCY_G4
FREQUENCY_Ab4
FREQUENCY_A4
FREQUENCY_Bb4
FREQUENCY_B4
FREQUENCY_C5
FREQUENCY_Db5
FREQUENCY_D5
FREQUENCY_Eb5
FREQUENCY_E5
FREQUENCY_F5
FREQUENCY_Gb5
FREQUENCY_G5
FREQUENCY_Ab5
FREQUENCY_A5
FREQUENCY_Bb5
FREQUENCY_B5
FREQUENCY_C6
FREQUENCY_Db6
FREQUENCY_D6
FREQUENCY_Eb6
FREQUENCY_E6
FREQUENCY_F6
FREQUENCY_Gb6
FREQUENCY_G6
FREQUENCY_Ab6
FREQUENCY_A6
FREQUENCY_Bb6
FREQUENCY_B6
FREQUENCY_C7
FREQUENCY_Db7
FREQUENCY_D7
FREQUENCY_Eb7
FREQUENCY_E7
FREQUENCY_F7
FREQUENCY_Gb7
FREQUENCY_G7
FREQUENCY_Ab7
FREQUENCY_A7
FREQUENCY_Bb7
FREQUENCY_B7
FREQUENCY_C8
FREQUENCY_Db8
FREQUENCY_D8
FREQUENCY_Eb8
FREQUENCY_E8
FREQUENCY_F8
FREQUENCY_Gb8
FREQUENCY_G8
FREQUENCY_Ab8
FREQUENCY_A8
FREQUENCY_Bb8
FREQUENCY_B8
FREQUENCY_MIDDLE_C
@Types
@Functions
*/