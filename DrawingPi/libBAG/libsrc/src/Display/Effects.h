/*---------------------------------------------------------------------------------
 libBAG  Copyright (C) 2010 - 2013
  BassAceGold - <BassAceGold@gmail.com>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.

  3. Restricting the usage of or portions of code in this library
     from other software developers and or content creators
     is strictly prohibited in applications with software creation capabilities
     and or with the intent to profit from said restrictions.

  4. This notice may not be removed or altered from any source
     distribution.
---------------------------------------------------------------------------------*/

#ifndef _BAG_DISPLAY_EFFECTS_
#define _BAG_DISPLAY_EFFECTS_



#ifdef __cplusplus
extern "C" {
#endif

typedef struct Smoothing_S{
  u16 *buf;
  int x, y, wd, ht;
  char corners;
}Smoothing_t;


extern u16 BAG_Effects_ColorToGrey(u16 Color);
extern u16 BAG_Effects_CalcAlpha(register int Color1, register int Color2, register int Alpha);
extern u16 BAG_Effects_PixSmoothing(const Smoothing_t *smooth);
extern u16 BAG_Effects_PixSmoothingFont(const Smoothing_t *smooth);
extern u16 BAG_Effects_ColBrightness(u16 color, s8 brightness);
extern unsigned char BAG_Effects_GetColBrightness(u16 color);
extern int BAG_Effects_GetScaledMultiplier(int input, int target);
extern int BAG_Effects_GetScaledResult(int currentValue, int multiplier);
extern int BAG_Effects_GetMultiplerTimes(int input, int times);
extern int BAG_Effects_GetMultiplierDiv(int input, int times);
#ifdef __cplusplus
}
#endif

#endif
