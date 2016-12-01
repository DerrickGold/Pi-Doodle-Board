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

#ifndef _BAG_DISPLAY_
#define _BAG_DISPLAY_

#include <ds2sdk.h>
#include <libBAG_types.h>
#ifdef __APPLE__
#include <libBAG_Debug.h>
#else
#include <debug/libBAG_Debug.h>
#endif
#include "../BAG_Math.h"
#include "Effects.h"


#ifndef SCREEN_WIDTH
	#define SCREEN_WIDTH 256
#endif

#ifndef SCREEN_HEIGHT
	#define SCREEN_HEIGHT 192
#endif

//#define MAGENTA 64543
#define MAGENTA RGB15(31, 0, 31)
#ifdef __cplusplus
extern "C" {
#endif


struct GFXRender_s{
	s16 OffsetX, OffsetY, Width, Height,
	ScrnOffsetX, ScrnOffsetY,
	X,Y,
	xFrame, yFrame,
	yFlip, xFlip, FlipMode;
};


typedef enum FRAME_DIR{
	FRAME_HOR = 1,
	FRAME_VERT
}
FRAME_DIR;

typedef enum ALPHA_MASK_MODE{
	ALPHA_FRAME = 1,
	ALPHA_FULL_OBJ,
	ALPHA_CUSTOM_DIM,
}
ALPHA_MASK_MODE;

typedef enum GFXTYPE{
	TYPE_NONE,
	TYPE_BMP,
	TYPE_JPG,
	TYPE_PNG,
}
GFX_TYPE;


typedef enum{
  OBJ_EXISTS = (1<<0),
  OBJ_ISCOPY = (1<<1),
  OBJ_XFLIP = (1<<2),
  OBJ_YFLIP = (1<<3),
  OBJ_ANIM_START = (1<<4),
  OBJ_ANIM_STOP = (1<<5),
  OBJ_GREY_SCALE = (1<<6),
  OBJ_ALPHA_BLEND = (1<<7),
  OBJ_ALPHA_MASK = (1<<8),
  OBJ_SCALING = (1<<9),
  OBJ_FASTRENDER = (1<<10),
  OBJ_4PIXSMOOTH = (1<<11),
  OBJ_8PIXSMOOTH = (1<<12),
  OBJ_BRIGHTNESS = (1<<13),
  OBJ_ISFONT = (1<<14),
  OBJ_SHRUNK = (1<<15),
  OBJ_USETRANSPARENT_COLOR = (1<<16),
}
OBJ_FLAGS;

struct BAG_GFX_DATA_s{
	s8 type, bits;
	s16 width, height, angle;
	u16 transp, offsetX, offsetY, scaleWd, scaleHt;
	u32 size, flags;
	s16 palCount;
	s16 alpha;
    s8 brightness;
    //if object is a font
    unsigned short fontCol;
} __attribute__ ((aligned (4)));

struct BAG_GFX_BUFFERS_s{
	u16 *pal, *gfx;
	u8 *alpha;
} __attribute__ ((aligned (4)));

struct BAG_GFX_FRAME_s{
	s8 animStart;
	s16 animTimer;
	s16 width, height, x, y;
} __attribute__ ((aligned (4)));



typedef struct GFXObj_s{
	int blitX, blitY;
	struct BAG_GFX_DATA_s data;
	struct BAG_GFX_BUFFERS_s buffer;
	struct BAG_GFX_FRAME_s frame;
	int error;
}GFXObj_t __attribute__ ((aligned (4)));


//================================gfx object info===========================================================

extern u32 *BAG_Display_GetGfxFlags(GFXObj_t *object);


//object blitting data
extern int *BAG_Display_GetGfxBlitX(GFXObj_t *object);
extern int *BAG_Display_GetGfxBlitY(GFXObj_t *object);
extern void BAG_Display_SetGfxBlitXY(GFXObj_t *object, int x, int y);

//type data
extern s8 *BAG_Display_GetGfxType(GFXObj_t *object);

//bit data
extern s8 *BAG_Display_GetGfxBits(GFXObj_t *object);

//dimension data
extern s16 *BAG_Display_GetGfxWidth(GFXObj_t *object);
extern s16 *BAG_Display_GetGfxHeight(GFXObj_t *object);
extern void BAG_Display_SetGfxWidthHeight(GFXObj_t *object, s16 width, s16 height);

//transparency data
extern u16 *BAG_Display_GetGfxTranspCol(GFXObj_t *object);
extern void BAG_Display_UseTransparentColor(GFXObj_t *object, s8 enable);
extern s8 BAG_Display_GetUseTransparentColor(GFXObj_t *object);
//font stuff
extern u16 *BAG_Display_GetFontColor(GFXObj_t *object);
extern void BAG_Display_SetFontColor(GFXObj_t *object, unsigned short color);

//gfx offset data
extern u16 *BAG_Display_GetGfxOffsetX(GFXObj_t *object);
extern u16 *BAG_Display_GetGfxOffsetY(GFXObj_t *object);
extern void BAG_Display_SetGfxOffsetXY(GFXObj_t *object, u16 offsetX, u16 offsetY);

//gfx scale
extern void BAG_Display_SetGfxScaleWd(GFXObj_t *object, u16 scale);
extern u16 *BAG_Display_GetGfxScaleWd(GFXObj_t *object);
extern void BAG_Display_SetGfxScaleHt(GFXObj_t *object, u16 scale);
extern u16 *BAG_Display_GetGfxScaleHt(GFXObj_t *object);
extern void BAG_Display_SetGfxScaleWdHt(GFXObj_t *object, u16 scaleWd, u16 scaleHt);
extern s8 BAG_Display_GetGfxScaled(GFXObj_t *object);

//animations
extern s8 *BAG_Display_GetGfxAnimStatus(GFXObj_t *object);
extern s16 *BAG_Display_GetGfxAnimTimer(GFXObj_t *object);
extern s16 *BAG_Display_GetGfxFrameWd(GFXObj_t *object);
extern s16 *BAG_Display_GetGfxFrameHt(GFXObj_t *object);
extern void BAG_Display_SetGfxFrameDim(GFXObj_t *object, s16 wd, s16 ht);
extern s16 *BAG_Display_GetGfxFrameX(GFXObj_t *object);
extern s16 *BAG_Display_GetGfxFrameY(GFXObj_t *object);
extern void BAG_DisplaySetGfxFrameXY(GFXObj_t *object, s16 x, s16 y);

//gfx flipping
extern void BAG_Display_SetGfxFlip(GFXObj_t *object, s8 axis);
extern s8 BAG_Display_GetGfxFlipX(GFXObj_t *object);
extern s8 BAG_Display_GetGfxFlipY(GFXObj_t *object);
extern void BAG_Display_SetGfxFlipX(GFXObj_t *object, char flip);
extern void BAG_Display_SetGfxFlipY(GFXObj_t *object, char flip);

//gfx rotating
extern s16 *BAG_Display_GetGfxRot(GFXObj_t *object);

//render mode
extern void BAG_Display_SetGfxRenderMode(GFXObj_t *object, s8 mode);
extern s8 BAG_Display_GetGfxRenderMode(GFXObj_t *object);
extern void BAG_Display_SetGfxPixSmoothing(GFXObj_t *object, u32 effect);
extern void BAG_Display_SetBrightnessOffset(GFXObj_t *object, s8 brightness);
extern s8 BAG_Display_GetBrightnessOffSet(GFXObj_t *object);


//palette handling
extern s16 *BAG_Display_GetGfxPalBufCount(GFXObj_t *object);
extern u16 *BAG_Display_GetGfxPalBuf(GFXObj_t *object);
extern void BAG_Display_FreeGfxPalBuf(GFXObj_t *object);
extern int BAG_Display_CreateGfxPalBuf(GFXObj_t *object, s16 palCount);
extern u16 *BAG_Display_GetGfxPalCol(GFXObj_t *object, s16 palIndex);

//graphics buffer handling
extern u32 *BAG_Display_GetGfxSize(GFXObj_t *object);
extern u16 *BAG_Display_GetGfxBuf(GFXObj_t *object);
extern void BAG_Display_FreeGfxBuf(GFXObj_t *object);
extern int BAG_Display_CreateGfxBuf(GFXObj_t *object, s16 width, s16 height);
extern u16 *BAG_Display_GetGfxPixelVal(GFXObj_t *object, s16 x, s16 y);
extern u16 *BAG_Display_GetGfxPixelCol(GFXObj_t *object, s16 x, s16 y);

//alpha mask handling
extern void BAG_Display_SetGfxAlpha(GFXObj_t *object, u8 alpha);
extern u8 *BAG_Display_GetGfxAlpha(GFXObj_t *object);
extern u8 *BAG_Display_GetGfxAlphaMask(GFXObj_t *object);
extern void BAG_Display_FreeGfxAlphaBuf(GFXObj_t *object);
extern int BAG_Display_CreateGfxAlphaBuf(GFXObj_t *object);
extern u8 *BAG_Display_GetGfxAlphaPix(GFXObj_t *object, s16 x, s16 y);

//grey scale handling
extern void BAG_Display_SetGfxGreyScale( GFXObj_t *object, int enable);
extern s8 BAG_Display_GetGfxGreyScale(GFXObj_t *object);


//================================Object handling API===========================================================

//object creation and removal
extern void BAG_Display_DeleteObj(GFXObj_t *object);
extern int BAG_Display_CreateObj(GFXObj_t *object, s8 bits, s16 width, s16 height, s16 frame_wd, s16 frame_ht);
extern int BAG_Display_CloneObj(GFXObj_t *dest, GFXObj_t *source);
extern int BAG_Display_CopyObj(GFXObj_t *dest, GFXObj_t *source);
extern int BAG_Display_ObjIsCopy(GFXObj_t *object);

//object animation
extern void BAG_Display_SetObjFrame(GFXObj_t *object, s8 dir, u16 frame);
extern void BAG_Display_ResetObjFrame(GFXObj_t *object);
extern void BAG_Display_StartAnim(GFXObj_t *object);
extern void BAG_Display_StopAnim(GFXObj_t *object);
extern void BAG_Display_SetObjOffset(GFXObj_t *object, s8 Frame_dir, u16 PixelOffset);
extern void BAG_Display_UpdateAnim(GFXObj_t *object, u16 First_Frame, u16 Last_Frame, u16 increment, u16 Frame_Delay, s8 Frame_dir);

//object blitting
extern void BAG_Display_DrawObjSlowEx(GFXObj_t *object, u16 *dest, s16 destWd, s16 destHt);
extern void BAG_Display_DrawObjSlow(GFXObj_t *object, u16* dest, s16 xpos, s16 ypos);
extern void BAG_Display_DrawObjFastEx(GFXObj_t *object, u16* dest, s16 destWd, s16 destHt);
extern void BAG_Display_DrawObjFast(GFXObj_t *object, u16* dest, s16 xpos, s16 ypos);
extern void BAG_Display_DrawObj(GFXObj_t *object, u16 * dest, s16 xpos, s16 ypos);

//object loading
extern int BAG_Display_LoadObjEx(const char *filename, GFXObj_t *object, s16 frame_wd, s16 frame_ht);
extern int BAG_Display_LoadObj(const char *filename, GFXObj_t *object);
extern int BAG_Display_LoadObjExt(const char *filename, GFXObj_t *object);

extern void BAG_Display_ReadObjSize(const char *filename, int *width, int *height);
extern void BAG_Display_ReadObjSizeExt(const char *filename, int *width, int *height);


#ifdef __cplusplus
}
#endif


#endif
