/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.
    Copyright (C) 1995 Ronny Wester
    Copyright (C) 2003 Jeremy Chin 
    Copyright (C) 2003-2007 Lucas Martin-King 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "pics.h"


color_t origPalette[256];


const int cBodyPic[BODY_COUNT][DIRECTION_COUNT][STATE_COUNT] =
{
	{
	 {25, 25, 25, 21, 22, 23, 24},
	 {25, 25, 25, 21, 22, 23, 24},
	 {20, 20, 20, 19, 18, 17, 16},
	 {4, 4, 4, 7, 8, 9, 10},
	 {4, 4, 4, 7, 8, 9, 10},
	 {4, 4, 4, 7, 8, 9, 10},
	 {15, 15, 15, 11, 12, 13, 14},
	 {15, 15, 15, 11, 12, 13, 14}
	 },
	{
	 {73, 73, 73, 74, 75, 76, 77},
	 {73, 73, 73, 74, 75, 76, 77},
	 {68, 68, 68, 72, 71, 70, 69},
	 {63, 63, 63, 64, 65, 66, 67},
	 {63, 63, 63, 64, 65, 66, 67},
	 {63, 63, 63, 64, 65, 66, 67},
	 {15, 15, 15, 11, 12, 13, 14},
	 {15, 15, 15, 11, 12, 13, 14}
	 }
};

const int cHeadPic[FACE_COUNT][DIRECTION_COUNT][STATE_COUNT+2] =
{
	{
	 {26, 26, 26, 26, 26, 26, 26, 26, 26},
	 {27, 27, 27, 27, 27, 27, 27, 27, 27},
	 {28, 28, 28, 28, 28, 28, 28, 137, 137},
	 {29, 29, 29, 29, 29, 29, 29, 138, 138},
	 {30, 30, 30, 30, 30, 30, 30, 139, 139},
	 {31, 31, 31, 31, 31, 31, 31, 140, 140},
	 {32, 32, 32, 32, 32, 32, 32, 141, 141},
	 {33, 33, 33, 33, 33, 33, 33, 33, 33}
	 },
	{
	 {129, 129, 129, 129, 129, 129, 129, 129, 129},
	 {130, 130, 130, 130, 130, 130, 130, 130, 130},
	 {131, 131, 131, 131, 131, 131, 131, 146, 146},
	 {132, 132, 132, 132, 132, 132, 132, 147, 147},
	 {133, 133, 133, 133, 133, 133, 133, 148, 148},
	 {134, 134, 134, 134, 134, 134, 134, 149, 149},
	 {135, 135, 135, 135, 135, 135, 135, 150, 150},
	 {136, 136, 136, 136, 136, 136, 136, 136, 136}
	 },
	{
	 {121, 121, 121, 121, 121, 121, 121, 121, 121},
	 {122, 122, 122, 122, 122, 122, 122, 122, 122},
	 {123, 123, 123, 123, 123, 123, 123, 123, 123},
	 {124, 124, 124, 124, 124, 124, 124, 124, 124},
	 {125, 125, 125, 125, 125, 125, 125, 125, 125},
	 {126, 126, 126, 126, 126, 126, 126, 126, 126},
	 {127, 127, 127, 127, 127, 127, 127, 127, 127},
	 {128, 128, 128, 128, 128, 128, 128, 128, 128}
	 },
	{
	 {228, 228, 228, 228, 228, 228, 228, 228, 228},
	 {227, 227, 227, 227, 227, 227, 227, 227, 227},
	 {226, 226, 226, 226, 226, 226, 226, 265, 265},
	 {225, 225, 225, 225, 225, 225, 225, 264, 264},
	 {224, 224, 224, 224, 224, 224, 224, 263, 263},
	 {223, 223, 223, 223, 223, 223, 223, 262, 262},
	 {222, 222, 222, 222, 222, 222, 222, 261, 261},
	 {229, 229, 229, 229, 229, 229, 229, 229, 229}
	 },
	{
	 {236, 236, 236, 236, 236, 236, 236, 236, 236},
	 {235, 235, 235, 235, 235, 235, 235, 235, 235},
	 {234, 234, 234, 234, 234, 234, 234, 242, 242},
	 {233, 233, 233, 233, 233, 233, 233, 241, 241},
	 {232, 232, 232, 232, 232, 232, 232, 240, 240},
	 {231, 231, 231, 231, 231, 231, 231, 239, 239},
	 {230, 230, 230, 230, 230, 230, 230, 238, 238},
	 {237, 237, 237, 237, 237, 237, 237, 237, 237}
	 },
	{
	 {249, 249, 249, 249, 249, 249, 249, 249, 249},
	 {248, 248, 248, 248, 248, 248, 248, 248, 248},
	 {247, 247, 247, 247, 247, 247, 247, 247, 247},
	 {246, 246, 246, 246, 246, 246, 246, 246, 246},
	 {245, 245, 245, 245, 245, 245, 245, 245, 245},
	 {244, 244, 244, 244, 244, 244, 244, 244, 244},
	 {243, 243, 243, 243, 243, 243, 243, 243, 243},
	 {250, 250, 250, 250, 250, 250, 250, 250, 250}
	 },
	{
	 {272, 272, 272, 272, 272, 272, 272, 272, 272},
	 {271, 271, 271, 271, 271, 271, 271, 271, 271},
	 {270, 270, 270, 270, 270, 270, 270, 278, 278},
	 {269, 269, 269, 269, 269, 269, 269, 277, 277},
	 {268, 268, 268, 268, 268, 268, 268, 276, 276},
	 {267, 267, 267, 267, 267, 267, 267, 275, 275},
	 {266, 266, 266, 266, 266, 266, 266, 274, 274},
	 {273, 273, 273, 273, 273, 273, 273, 273, 273}
	 },
	{
	 {308, 308, 308, 308, 308, 308, 308, 308, 308},
	 {307, 307, 307, 307, 307, 307, 307, 307, 307},
	 {306, 306, 306, 306, 306, 306, 306, 306, 306},
	 {305, 305, 305, 305, 305, 305, 305, 305, 305},
	 {304, 304, 304, 304, 304, 304, 304, 304, 304},
	 {303, 303, 303, 303, 303, 303, 303, 303, 303},
	 {302, 302, 302, 302, 302, 302, 302, 302, 302},
	 {309, 309, 309, 309, 309, 309, 309, 309, 309}
	 },
	{
	 {372, 372, 372, 372, 372, 372, 372, 372, 372},
	 {371, 371, 371, 371, 371, 371, 371, 371, 371},
	 {370, 370, 370, 370, 370, 370, 370, 370, 370},
	 {369, 369, 369, 369, 369, 369, 369, 369, 369},
	 {368, 368, 368, 368, 368, 368, 368, 368, 368},
	 {367, 367, 367, 367, 367, 367, 367, 367, 367},
	 {366, 366, 366, 366, 366, 366, 366, 366, 366},
	 {373, 373, 373, 373, 373, 373, 373, 373, 373}
	 },
	{
	 {396, 396, 396, 396, 396, 396, 396, 396, 396},
	 {395, 395, 395, 395, 395, 395, 395, 395, 395},
	 {394, 394, 394, 394, 394, 394, 394, 394, 394},
	 {393, 393, 393, 393, 393, 393, 393, 393, 393},
	 {392, 392, 392, 392, 392, 392, 392, 392, 392},
	 {391, 391, 391, 391, 391, 391, 391, 391, 391},
	 {390, 390, 390, 390, 390, 390, 390, 390, 390},
	 {397, 397, 397, 397, 397, 397, 397, 397, 397}
	 },
	{
	 {P2 + 6, P2 + 6, P2 + 6, P2 + 6, P2 + 6, P2 + 6, P2 + 6, P2 + 6,
	  P2 + 6},
	 {P2 + 5, P2 + 5, P2 + 5, P2 + 5, P2 + 5, P2 + 5, P2 + 5, P2 + 5,
	  P2 + 5},
	 {P2 + 4, P2 + 4, P2 + 4, P2 + 4, P2 + 4, P2 + 4, P2 + 4, P2 + 4,
	  P2 + 4},
	 {P2 + 3, P2 + 3, P2 + 3, P2 + 3, P2 + 3, P2 + 3, P2 + 3, P2 + 3,
	  P2 + 3},
	 {P2 + 2, P2 + 2, P2 + 2, P2 + 2, P2 + 2, P2 + 2, P2 + 2, P2 + 2,
	  P2 + 2},
	 {P2 + 1, P2 + 1, P2 + 1, P2 + 1, P2 + 1, P2 + 1, P2 + 1, P2 + 1,
	  P2 + 1},
	 {P2 + 0, P2 + 0, P2 + 0, P2 + 0, P2 + 0, P2 + 0, P2 + 0, P2 + 0,
	  P2 + 0},
	 {P2 + 7, P2 + 7, P2 + 7, P2 + 7, P2 + 7, P2 + 7, P2 + 7, P2 + 7,
	  P2 + 7}
	 },
	{
	 {P2 + 14, P2 + 14, P2 + 14, P2 + 14, P2 + 14, P2 + 14, P2 + 14,
	  P2 + 14, P2 + 14},
	 {P2 + 13, P2 + 13, P2 + 13, P2 + 13, P2 + 13, P2 + 13, P2 + 13,
	  P2 + 13, P2 + 13},
	 {P2 + 12, P2 + 12, P2 + 12, P2 + 12, P2 + 12, P2 + 12, P2 + 12,
	  P2 + 12, P2 + 12},
	 {P2 + 11, P2 + 11, P2 + 11, P2 + 11, P2 + 11, P2 + 11, P2 + 11,
	  P2 + 11, P2 + 11},
	 {P2 + 10, P2 + 10, P2 + 10, P2 + 10, P2 + 10, P2 + 10, P2 + 10,
	  P2 + 10, P2 + 10},
	 {P2 + 9, P2 + 9, P2 + 9, P2 + 9, P2 + 9, P2 + 9, P2 + 9, P2 + 9,
	  P2 + 9},
	 {P2 + 8, P2 + 8, P2 + 8, P2 + 8, P2 + 8, P2 + 8, P2 + 8, P2 + 8,
	  P2 + 8},
	 {P2 + 15, P2 + 15, P2 + 15, P2 + 15, P2 + 15, P2 + 15, P2 + 15,
	  P2 + 15, P2 + 15}
	 },
	{
	 {P2 + 22, P2 + 22, P2 + 22, P2 + 22, P2 + 22, P2 + 22, P2 + 22,
	  P2 + 22, P2 + 22},
	 {P2 + 21, P2 + 21, P2 + 21, P2 + 21, P2 + 21, P2 + 21, P2 + 21,
	  P2 + 21, P2 + 21},
	 {P2 + 20, P2 + 20, P2 + 20, P2 + 20, P2 + 20, P2 + 20, P2 + 20,
	  P2 + 20, P2 + 20},
	 {P2 + 19, P2 + 19, P2 + 19, P2 + 19, P2 + 19, P2 + 19, P2 + 19,
	  P2 + 19, P2 + 19},
	 {P2 + 18, P2 + 18, P2 + 18, P2 + 18, P2 + 18, P2 + 18, P2 + 18,
	  P2 + 18, P2 + 18},
	 {P2 + 17, P2 + 17, P2 + 17, P2 + 17, P2 + 17, P2 + 17, P2 + 17,
	  P2 + 17, P2 + 17},
	 {P2 + 16, P2 + 16, P2 + 16, P2 + 16, P2 + 16, P2 + 16, P2 + 16,
	  P2 + 16, P2 + 16},
	 {P2 + 23, P2 + 23, P2 + 23, P2 + 23, P2 + 23, P2 + 23, P2 + 23,
	  P2 + 23, P2 + 23}
	 },
	{
	 {P2 + 30, P2 + 30, P2 + 30, P2 + 30, P2 + 30, P2 + 30, P2 + 30,
	  P2 + 30, P2 + 30},
	 {P2 + 29, P2 + 29, P2 + 29, P2 + 29, P2 + 29, P2 + 29, P2 + 29,
	  P2 + 29, P2 + 29},
	 {P2 + 28, P2 + 28, P2 + 28, P2 + 28, P2 + 28, P2 + 28, P2 + 28,
	  P2 + 28, P2 + 28},
	 {P2 + 27, P2 + 27, P2 + 27, P2 + 27, P2 + 27, P2 + 27, P2 + 27,
	  P2 + 27, P2 + 27},
	 {P2 + 26, P2 + 26, P2 + 26, P2 + 26, P2 + 26, P2 + 26, P2 + 26,
	  P2 + 26, P2 + 26},
	 {P2 + 25, P2 + 25, P2 + 25, P2 + 25, P2 + 25, P2 + 25, P2 + 25,
	  P2 + 25, P2 + 25},
	 {P2 + 24, P2 + 24, P2 + 24, P2 + 24, P2 + 24, P2 + 24, P2 + 24,
	  P2 + 24, P2 + 24},
	 {P2 + 31, P2 + 31, P2 + 31, P2 + 31, P2 + 31, P2 + 31, P2 + 31,
	  P2 + 31, P2 + 31}
	 },
	{
	 {P2 + 38, P2 + 38, P2 + 38, P2 + 38, P2 + 38, P2 + 38, P2 + 38,
	  P2 + 38, P2 + 38},
	 {P2 + 37, P2 + 37, P2 + 37, P2 + 37, P2 + 37, P2 + 37, P2 + 37,
	  P2 + 37, P2 + 37},
	 {P2 + 36, P2 + 36, P2 + 36, P2 + 36, P2 + 36, P2 + 36, P2 + 36,
	  P2 + 44, P2 + 44},
	 {P2 + 35, P2 + 35, P2 + 35, P2 + 35, P2 + 35, P2 + 35, P2 + 35,
	  P2 + 43, P2 + 43},
	 {P2 + 34, P2 + 34, P2 + 34, P2 + 34, P2 + 34, P2 + 34, P2 + 34,
	  P2 + 42, P2 + 42},
	 {P2 + 33, P2 + 33, P2 + 33, P2 + 33, P2 + 33, P2 + 33, P2 + 33,
	  P2 + 41, P2 + 41},
	 {P2 + 32, P2 + 32, P2 + 32, P2 + 32, P2 + 32, P2 + 32, P2 + 32,
	  P2 + 40, P2 + 40},
	 {P2 + 39, P2 + 39, P2 + 39, P2 + 39, P2 + 39, P2 + 39, P2 + 39,
	  P2 + 39, P2 + 39}
	 },
	{
	 {P2 + 129, P2 + 129, P2 + 129, P2 + 129, P2 + 129, P2 + 129,
	  P2 + 129, P2 + 129, P2 + 129},
	 {P2 + 128, P2 + 128, P2 + 128, P2 + 128, P2 + 128, P2 + 128,
	  P2 + 128, P2 + 128, P2 + 128},
	 {P2 + 127, P2 + 127, P2 + 127, P2 + 127, P2 + 127, P2 + 127,
	  P2 + 127, P2 + 127, P2 + 127},
	 {P2 + 126, P2 + 126, P2 + 126, P2 + 126, P2 + 126, P2 + 126,
	  P2 + 126, P2 + 126, P2 + 126},
	 {P2 + 125, P2 + 125, P2 + 125, P2 + 125, P2 + 125, P2 + 125,
	  P2 + 125, P2 + 125, P2 + 125},
	 {P2 + 124, P2 + 124, P2 + 124, P2 + 124, P2 + 124, P2 + 124,
	  P2 + 124, P2 + 124, P2 + 124},
	 {P2 + 123, P2 + 123, P2 + 123, P2 + 123, P2 + 123, P2 + 123,
	  P2 + 123, P2 + 123, P2 + 123},
	 {P2 + 130, P2 + 130, P2 + 130, P2 + 130, P2 + 130, P2 + 130,
	  P2 + 130, P2 + 130, P2 + 130}
	 },
	{
	 {P2 + 175, P2 + 175, P2 + 175, P2 + 175, P2 + 175, P2 + 175,
	  P2 + 175, P2 + 175, P2 + 175},
	 {P2 + 174, P2 + 174, P2 + 174, P2 + 174, P2 + 174, P2 + 174,
	  P2 + 174, P2 + 174, P2 + 174},
	 {P2 + 173, P2 + 173, P2 + 173, P2 + 173, P2 + 173, P2 + 173,
	  P2 + 173, P2 + 181, P2 + 181},
	 {P2 + 172, P2 + 172, P2 + 172, P2 + 172, P2 + 172, P2 + 172,
	  P2 + 172, P2 + 180, P2 + 180},
	 {P2 + 171, P2 + 171, P2 + 171, P2 + 171, P2 + 171, P2 + 171,
	  P2 + 171, P2 + 179, P2 + 179},
	 {P2 + 170, P2 + 170, P2 + 170, P2 + 170, P2 + 170, P2 + 170,
	  P2 + 170, P2 + 178, P2 + 178},
	 {P2 + 169, P2 + 169, P2 + 169, P2 + 169, P2 + 169, P2 + 169,
	  P2 + 169, P2 + 177, P2 + 177},
	 {P2 + 176, P2 + 176, P2 + 176, P2 + 176, P2 + 176, P2 + 176,
	  P2 + 176, P2 + 176, P2 + 176}
	 }
};


const OffsetTable cBodyOffset[BODY_COUNT] = {
	{
	 {-8, -11},
	 {-8, -11},
	 {-6, -11},
	 {-8, -11},
	 {-8, -11},
	 {-8, -11},
	 {-5, -11},
	 {-5, -11}
	 },
	{
	 {-8, -11},
	 {-8, -11},
	 {-6, -11},
	 {-8, -11},
	 {-8, -11},
	 {-8, -11},
	 {-5, -11},
	 {-5, -11}
	 }
};

const OffsetTable cNeckOffset[BODY_COUNT] = {
	{
	 {0, -11},
	 {0, -11},
	 {0, -11},
	 {0, -11},
	 {0, -11},
	 {0, -11},
	 {1, -11},
	 {1, -11}
	 },
	{
	 {0, -11},
	 {0, -11},
	 {0, -11},
	 {0, -11},
	 {0, -11},
	 {0, -11},
	 {1, -11},
	 {1, -11}
	 }
};

const OffsetTable cGunHandOffset[BODY_COUNT] = {
	{
	 {6, -6},
	 {6, -6},
	 {2, -4},
	 {-7, -6},
	 {-7, -6},
	 {-7, -6},
	 {-3, -7},
	 {-3, -7}
	 },
	{
	 {6, -6},
	 {6, -6},
	 {2, -4},
	 {-7, -6},
	 {-7, -6},
	 {-7, -6},
	 {-3, -7},
	 {-3, -7}
	 }
};

const OffsetTable cHeadOffset[FACE_COUNT] = {
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -9},
	 {-5, -9},
	 {-5, -9},
	 {-5, -9},
	 {-5, -9},
	 {-5, -9},
	 {-5, -9},
	 {-5, -9}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7},
	 {-5, -7}
	 },
	{
	 {-5, -5},
	 {-5, -5},
	 {-5, -5},
	 {-5, -5},
	 {-5, -5},
	 {-5, -5},
	 {-5, -5},
	 {-5, -5}
	 },
	{
	 {-3, -7},
	 {-4, -7},
	 {-7, -7},
	 {-4, -8},
	 {-3, -8},
	 {-4, -8},
	 {-5, -7},
	 {-4, -7}
	 }
};

const TOffsetPic cGrenadePics[4] = {
	{-3, -3, 34},
	{-3, -3, 35},
	{-3, -3, 36},
	{-3, -3, 37}
};

const TOffsetPic cFlamePics[4] = {
	{-5, -8, 38},
	{-7, -8, 39},
	{-5, -8, 40},
	{-6, -8, 41}
};

const TOffsetPic cFireBallPics[FIREBALL_MAX] = {
	{-9, -9, 47},
	{-9, -9, 48},
	{-9, -9, 49},
	{-9, -9, 50},
	{-9, -9, 51},
	{-12, -10, 52},
	{-12, -10, 53},
	{-11, -10, 54},
	{-12, -10, 55},
	{-12, -10, 56},
	{-12, -10, 57},
	{-12, -10, 58},
	{-12, -10, 59},
	{-11, -9, 60},
	{-10, -8, 61},
	{-10, -8, 62}
};

const TOffsetPic cBeamPics[BEAM_PIC_COUNT][DIRECTION_COUNT] =
{
	// Normal beam
	{
		{ -1, -3, 43 },
		{ -2, -2, 46 },
		{ -3, -1, 44 },
		{ -2, -2, 45 },
		{ -1, -3, 43 },
		{ -2, -2, 46 },
		{ -3, -1, 44 },
		{ -2, -2, 45 }
	},
	// Bright beam
	{
		{ -1, -3, 362 },
		{ -2, -2, 365 },
		{ -3, -1, 363 },
		{ -2, -2, 364 },
		{ -1, -3, 362 },
		{ -2, -2, 365 },
		{ -3, -1, 363 },
		{ -2, -2, 364 }
	}
};

const char *WallTypeStr(const int w)
{
	switch (w)
	{
		T2S(WALL_SINGLE, "o");
		T2S(WALL_LEFT, "w");
		T2S(WALL_RIGHT, "e");
		T2S(WALL_TOP, "n");
		T2S(WALL_BOTTOM, "s");
		T2S(WALL_TOPLEFT, "nw");
		T2S(WALL_TOPRIGHT, "ne");
		T2S(WALL_BOTTOMLEFT, "sw");
		T2S(WALL_BOTTOMRIGHT, "se");
		T2S(WALL_LEFT_T, "wt");
		T2S(WALL_RIGHT_T, "et");
		T2S(WALL_TOP_T, "nt");
		T2S(WALL_BOTTOM_T, "st");
		T2S(WALL_VERTICAL, "v");
		T2S(WALL_HORIZONTAL, "h");
		T2S(WALL_CROSS, "x");
	default:
		return "";
	}
}
const char *IntWallStyle(const int i)
{
	static const char *wallStyles[] = {
		"steel", "brick", "carbon", "steelwood", "stone", "plasteel",
		"granite"
	};
	return wallStyles[abs(i) % WALL_STYLE_COUNT];
}

const char *IntTileType(const int i)
{
	static const char *tileTypes[] = { "shadow", "normal", "alt1", "alt2" };
	return tileTypes[abs(i) % FLOOR_TYPES];
}
const char *IntFloorStyle(const int i)
{
	static const char *floorStyles[] = {
		"recessed", "tile", "dirt", "flat", "striped", "smallsquare", "stone",
		"wood", "biggrid", "grid"
	};
	return floorStyles[abs(i) % FLOOR_STYLE_COUNT];
}
// Note: room styles subtly different from floor styles
const char *IntRoomStyle(const int i)
{
	static const char *roomStyles[] = {
		"recessed", "tile", "dirt", "flat", "striped", "smallsquare", "stone",
		"wood", "grid", "biggrid", "checker"
	};
	return roomStyles[abs(i) % ROOM_STYLE_COUNT];
}

const struct Offset cWallOffset = { 0, -12 };

const TOffsetPic cDeathPics[DEATH_MAX] = {
	{-8, -18, 112},
	{-8, -18, 113},
	{-8, -18, 114},
	{-8, -18, 115},
	{-8, -18, 116},
	{-8, -18, 117},
	{-8, -18, 118},
	{-8, -18, 119},
	{-8, -18, 120}
};
