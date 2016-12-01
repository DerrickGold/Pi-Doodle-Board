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

#ifndef __DRAW_H__
#define __DRAW_H__

#include <libBAG_types.h>

#ifdef __cplusplus
extern "C" {
#endif


extern void BAG_Draw_BlitPixel(u16 *Dest, int destWd, int destHt, int x, int y, u16 color);
extern u16 BAG_Draw_GetPixel(u16 *Dest, int destWd, int destHt, int x, int y);
extern void BAG_Draw_BlitLine(u16 *Dest, int destWd, int destHt, int x, int y, int x2, int y2, u16 color);
extern void BAG_Draw_BlitLineEx(u16 *Dest, int destWd, int destHt, s16 basex, s16 basey, s16 endx, s16 endy, u16 color, u8 size);
extern void BAG_Draw_Rect(unsigned short *dest, int destWd, int destHt, int x1, int y1, int x2, int y2, unsigned short color);

#ifdef __cplusplus
}
#endif


#endif

