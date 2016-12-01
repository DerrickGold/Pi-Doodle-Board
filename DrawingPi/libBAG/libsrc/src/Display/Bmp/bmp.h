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

#ifndef _BAG_BMPLOADING_
#define _BAG_BMPLOADING_

#include <ds2sdk.h>
#include "../Display.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int BAG_Display_LoadBmp(const char *filename, GFXObj_t *object);
extern int BAG_Display_ReloadBmp(const char *filename, GFXObj_t *object);
extern int BAG_Display_LoadBmpChunk( const char *filename, GFXObj_t *object, s16 pix_x1, s16 pix_y1, s16 pix_x2, s16 pix_y2);
extern s8 BAG_Display_ScrnCap(s8 Screen, const char *Directory);
extern s8 BAG_Display_GfxToBitmapFile(GFXObj_t *object, const char *Directory, const char *name);
extern void BAG_Display_ReadBmpSize(const char *filename, int *width, int *height);


#ifdef __cplusplus
}
#endif


#endif
