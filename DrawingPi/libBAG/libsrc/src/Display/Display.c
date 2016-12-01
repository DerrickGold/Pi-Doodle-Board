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

#include "Display.h"
#include "../Filesystem/filesystem.h"

#include "Bmp/bmp.h"
#include "Png/png.h"
#include "Jpeg/jpeg.h"
/*=================================================================================================
Functions for accessing GFXObj_t data
=================================================================================================*/

/*===================================================
Access graphics flags
===================================================*/
u32 *BAG_Display_GetGfxFlags(GFXObj_t *object){
	return (u32*)&object->data.flags;
}

/*===================================================
Access and set blit position
===================================================*/
int *BAG_Display_GetGfxBlitX(GFXObj_t *object){
	return (int*)&object->blitX;
}

int *BAG_Display_GetGfxBlitY(GFXObj_t *object){
	return (int*)&object->blitY;
}

void BAG_Display_SetGfxBlitXY(GFXObj_t *object, int x, int y){
	VAL(BAG_Display_GetGfxBlitX(object)) = x;
	VAL(BAG_Display_GetGfxBlitY(object)) = y;
}


/*===================================================
Access and set the graphics type
(BMP, JPG, PNG, etc)
===================================================*/
s8 *BAG_Display_GetGfxType(GFXObj_t *object){
	return (s8*)&object->data.type;
}
/*===================================================
Access and set graphics bit depth
===================================================*/

//get color bitdepth of loaded image
s8 *BAG_Display_GetGfxBits(GFXObj_t *object){
	return (s8*)&object->data.bits;
}


/*===================================================
Access and set graphics dimensions
===================================================*/
inline s16 *BAG_Display_GetGfxWidth(GFXObj_t *object){
	return (s16*)&object->data.width;
}

inline s16 *BAG_Display_GetGfxHeight(GFXObj_t *object){
	return (s16*)&object->data.height;
}

void BAG_Display_SetGfxWidthHeight(GFXObj_t *object, s16 width, s16 height){
	VAL(BAG_Display_GetGfxWidth(object)) = width;
	VAL(BAG_Display_GetGfxHeight(object)) = height;
}

/*===================================================
Access and set the color value to use for
transparency
===================================================*/
inline u16 *BAG_Display_GetGfxTranspCol(GFXObj_t *object){
	return (u16*)&object->data.transp;
}

void BAG_Display_UseTransparentColor(GFXObj_t *object, s8 enable) {
  RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_USETRANSPARENT_COLOR);
  if (enable)
    SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_USETRANSPARENT_COLOR);
}

inline s8 BAG_Display_GetUseTransparentColor(GFXObj_t *object) {
  return GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_USETRANSPARENT_COLOR);
}


/*===================================================
Set color of font if object holds font graphics
===================================================*/
inline u16 *BAG_Display_GetFontColor(GFXObj_t *object){
	return (u16*)&object->data.fontCol;
}

void BAG_Display_SetFontColor(GFXObj_t *object, unsigned short color) {

	if (GET_FLAG(*BAG_Display_GetGfxFlags(object), OBJ_ISFONT))
		*BAG_Display_GetFontColor(object) = color;

}

/*===================================================
Access and set an offset in graphics BUFFER_FAILED_MSG
to blit from
===================================================*/
inline u16 *BAG_Display_GetGfxOffsetX(GFXObj_t *object){
	return (u16*)&object->data.offsetX;
}

inline u16 *BAG_Display_GetGfxOffsetY(GFXObj_t *object){
	return (u16*)&object->data.offsetY;
}

void BAG_Display_SetGfxOffsetXY(GFXObj_t *object, u16 offsetX, u16 offsetY){
	VAL(BAG_Display_GetGfxOffsetX(object)) = offsetX;
	VAL(BAG_Display_GetGfxOffsetY(object)) = offsetY;
}



/*===================================================
Access and set frame and animation data
===================================================*/
s8 *BAG_Display_GetGfxAnimStatus(GFXObj_t *object){
	return (s8*)&object->frame.animStart;
}

s16 *BAG_Display_GetGfxAnimTimer(GFXObj_t *object){
	return (s16*)&object->frame.animTimer;
}

//get frame details
inline s16 *BAG_Display_GetGfxFrameWd(GFXObj_t *object){
	return (s16*)&object->frame.width;
}

inline s16 *BAG_Display_GetGfxFrameHt(GFXObj_t *object){
	return (s16*)&object->frame.height;
}

void BAG_Display_SetGfxFrameDim(GFXObj_t *object, s16 wd, s16 ht){
	VAL(BAG_Display_GetGfxFrameWd(object)) = wd;
	VAL(BAG_Display_GetGfxFrameHt(object)) = ht;
}


//frame position both axis
inline s16 *BAG_Display_GetGfxFrameX(GFXObj_t *object){
	return (s16*)&object->frame.x;
}

inline s16 *BAG_Display_GetGfxFrameY(GFXObj_t *object){
	return (s16*)&object->frame.y;
}

void BAG_DisplaySetGfxFrameXY(GFXObj_t *object, s16 x, s16 y){
	VAL(BAG_Display_GetGfxFrameX(object)) = x;
	VAL(BAG_Display_GetGfxFrameY(object)) = y;
}


/*===================================================
Access and set graphic scaling dimensions
===================================================*/
inline u16 *BAG_Display_GetGfxScaleWd(GFXObj_t *object){
	return (u16*)&object->data.scaleWd;
}

void BAG_Display_SetGfxScaleWd(GFXObj_t *object, u16 scale){
	SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_SCALING);
	VAL(BAG_Display_GetGfxScaleWd(object)) = scale;

	if(scale < (*BAG_Display_GetGfxFrameWd(object)))
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_SHRUNK);
	else
		RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_SHRUNK);
}

inline u16 *BAG_Display_GetGfxScaleHt(GFXObj_t *object){
	return (u16*)&object->data.scaleHt;
}

void BAG_Display_SetGfxScaleHt(GFXObj_t *object, u16 scale){
	SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_SCALING);
	VAL(BAG_Display_GetGfxScaleHt(object)) = scale;

	if(scale < (*BAG_Display_GetGfxFrameHt(object)))
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_SHRUNK);
	else
		RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_SHRUNK);
}

void BAG_Display_SetGfxScaleWdHt(GFXObj_t *object, u16 scaleWd, u16 scaleHt){
	BAG_Display_SetGfxScaleWd(object, scaleWd);
	BAG_Display_SetGfxScaleHt(object, scaleHt);
}

void BAG_Display_ResetGfxScale(GFXObj_t *object){
	BAG_Display_SetGfxScaleWdHt(object, (*BAG_Display_GetGfxWidth(object)), (*BAG_Display_GetGfxHeight(object)));
	RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_SCALING);
}

inline s8 BAG_Display_GetGfxScaled(GFXObj_t *object){
	return GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_SCALING);
}


/*===================================================
Access and set graphics flipping
===================================================*/
inline s8 BAG_Display_GetGfxFlipX(GFXObj_t *object){
	return GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_XFLIP);
}

inline s8 BAG_Display_GetGfxFlipY(GFXObj_t *object){
	return GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_YFLIP);
}

inline void BAG_Display_SetGfxFlipX(GFXObj_t *object, char flip){
	RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_XFLIP);
	if(flip)
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_XFLIP);
}

inline void BAG_Display_SetGfxFlipY(GFXObj_t *object, char flip){
	RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_YFLIP);
	if(flip)
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_YFLIP);
}

void BAG_Display_SetGfxFlip(GFXObj_t *object, s8 axis){
	u32 flag = 0;
	if(axis == FRAME_HOR) flag = OBJ_XFLIP;
	else 				  flag = OBJ_YFLIP;

	if(!GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), flag))
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), flag);
	else
		RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), flag);
}


/*===================================================
Access and set graphics rotation
===================================================*/

inline s16 *BAG_Display_GetGfxRot(GFXObj_t *object){
	return (s16*)&object->data.angle;
}
/*===================================================
Access and set graphics render mode
-per pixel
-per line
===================================================*/
void BAG_Display_SetGfxRenderMode(GFXObj_t *object, s8 mode){
	RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_FASTRENDER);
	if(mode)
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_FASTRENDER);
}

inline s8 BAG_Display_GetGfxRenderMode(GFXObj_t *object){
	return GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_FASTRENDER);
}

/*===================================================
Set object to render in grey scale
===================================================*/
void BAG_Display_SetGfxGreyScale( GFXObj_t *object, int enable){
	RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_GREY_SCALE);
	if(enable)
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_GREY_SCALE);
}

inline s8 BAG_Display_GetGfxGreyScale(GFXObj_t *object){
	return GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_GREY_SCALE);
}

/*======================================================
Object smoothing options for downscaling
=======================================================*/
void BAG_Display_SetGfxPixSmoothing(GFXObj_t *object, u32 effect){
	if(effect & OBJ_4PIXSMOOTH){//averages 4 surrounding pixels
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_4PIXSMOOTH);
		RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_8PIXSMOOTH);
	}
	else if(effect & OBJ_8PIXSMOOTH){//averages 4 surrounding and half of corner pixels
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_8PIXSMOOTH);
		RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_4PIXSMOOTH);
	}
	else{//turn off smoothing
		RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_4PIXSMOOTH);
		RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_8PIXSMOOTH);
	}
}
/*======================================================
Set object render brightness
=======================================================*/
s8 BAG_Display_GetBrightnessOffSet(GFXObj_t *object){
	return object->data.brightness;
}

void BAG_Display_SetBrightnessOffset(GFXObj_t *object, s8 brightness){
	SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_BRIGHTNESS);
	if(!brightness)
		RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_BRIGHTNESS);

	object->data.brightness = brightness;
}

/*=================================================================================================
Graphic data buffer handling
=================================================================================================*/

char BAG_Display_CheckRange(GFXObj_t *object, int x, int y){
	return(!(x < 0 || y < 0 || x > VAL(BAG_Display_GetGfxWidth(object)) ||
		   y > VAL(BAG_Display_GetGfxHeight(object)) ) );
}

/*===================================================
Access and set and create palette information
===================================================*/
void BAG_Display_SetGfxPalBufCount(GFXObj_t *object, s16 count){
	object->data.palCount = count;
}

s16 *BAG_Display_GetGfxPalBufCount(GFXObj_t *object){
	return (s16*)&object->data.palCount;
}


u16 *BAG_Display_GetGfxPalBuf(GFXObj_t *object){
	return object->buffer.pal;
}


void BAG_Display_FreeGfxPalBuf(GFXObj_t *object){
	if(BAG_Display_GetGfxPalBuf(object) &&
	   VAL(BAG_Display_GetGfxPalBufCount(object)) > 0 &&
	   !BAG_Display_ObjIsCopy(object)){

		free(object->buffer.pal);
	}

	object->buffer.pal = NULL;
	VAL(BAG_Display_GetGfxPalBufCount(object)) = 0;
}


int BAG_Display_CreateGfxPalBuf(GFXObj_t *object, s16 palCount){
	//free data
	BAG_Display_FreeGfxPalBuf(object);
	//allocate memory
	VAL(BAG_Display_GetGfxPalBufCount(object)) = palCount;
	object->buffer.pal = (u16*)calloc( sizeof(u16), VAL(BAG_Display_GetGfxPalBufCount(object)));

	if(!BAG_Display_GetGfxPalBuf(object)){
		BAG_DBG_LibMsg(BUFFER_FAILED_MSG, sizeof(u16) * VAL(BAG_Display_GetGfxPalBufCount(object)), "GFX Pal");
		return ERR_END;
	}

	return ERR_NONE;
}

u16 *BAG_Display_GetGfxPalCol(GFXObj_t *object, s16 palIndex){
	u16 *pal = BAG_Display_GetGfxPalBuf(object);

	if(palIndex > VAL(BAG_Display_GetGfxPalBufCount(object)) || palIndex < 0)
		return 0;

	return (u16*)&pal[palIndex];
}


/*===================================================
Access and set and create main graphics buffer
===================================================*/
u32 *BAG_Display_GetGfxSize(GFXObj_t *object){
	return (u32*)&object->data.size;
}

u16 *BAG_Display_GetGfxBuf(GFXObj_t *object){
	if(VAL(BAG_Display_GetGfxSize(object)) > 0)
		return object->buffer.gfx;
	return NULL;
}

void BAG_Display_FreeGfxBuf(GFXObj_t *object){
	if(BAG_Display_GetGfxBuf(object) && !BAG_Display_ObjIsCopy(object) &&
	   VAL(BAG_Display_GetGfxSize(object)) > 0)
	{
		if(object->buffer.gfx)
			free(object->buffer.gfx);
	}

	object->buffer.gfx = NULL;
	VAL(BAG_Display_GetGfxSize(object)) = 0;
}


int BAG_Display_CreateGfxBuf(GFXObj_t *object, s16 width, s16 height){
	//free data
	BAG_Display_FreeGfxBuf(object);
	//allocate memory
	BAG_Display_SetGfxWidthHeight(object, width, height);
	VAL(BAG_Display_GetGfxSize(object)) = (VAL(BAG_Display_GetGfxWidth(object)) *
										   VAL(BAG_Display_GetGfxHeight(object))) << 1;

	object->buffer.gfx = (u16*)calloc( sizeof(char), VAL(BAG_Display_GetGfxSize(object)));

	if(!object->buffer.gfx){
		BAG_DBG_LibMsg(BUFFER_FAILED_MSG, VAL(BAG_Display_GetGfxSize(object)), "GFX");
		return ERR_END;
	}

	return ERR_NONE;
}

u16 *BAG_Display_GetGfxPixelVal(GFXObj_t *object, s16 x, s16 y){
	u16 *buf = BAG_Display_GetGfxBuf(object);
	if(!buf || !BAG_Display_CheckRange(object, x, y))
		return 0;

	return (u16*)&buf[ x + (y * VAL(BAG_Display_GetGfxWidth(object)))];
}

u16 *BAG_Display_GetGfxPixelCol(GFXObj_t *object, s16 x, s16 y){
	if(VAL(BAG_Display_GetGfxBits(object)) == 8)
		return BAG_Display_GetGfxPalCol(object, VAL(BAG_Display_GetGfxPixelVal(object, x,y)));

	return BAG_Display_GetGfxPixelVal(object, x,y);
}


/*===================================================
Access and set create alpha mask buffers
===================================================*/
inline u8 *BAG_Display_GetGfxAlpha(GFXObj_t *object){
	return (u8*)&object->data.alpha;
}

void BAG_Display_SetGfxAlpha(GFXObj_t *object, u8 alpha){
	RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ALPHA_BLEND);
	VAL(BAG_Display_GetGfxAlpha(object)) = alpha;
	if(alpha < 255)
		SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ALPHA_BLEND);
}

inline u8 *BAG_Display_GetGfxAlphaMask(GFXObj_t *object){
	return object->buffer.alpha;
}

void BAG_Display_FreeGfxAlphaBuf(GFXObj_t *object){
	//do not attempt to free buffer if object doesn't exist, or is a clone
	if(BAG_Display_GetGfxAlphaMask(object) &&
	   GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ALPHA_MASK) &&
	   !BAG_Display_ObjIsCopy(object))
	{
		free(object->buffer.alpha);
	}

	object->buffer.alpha = NULL;
	BAG_Display_SetGfxAlpha(object, 255);
	RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ALPHA_MASK);
}

int BAG_Display_CreateGfxAlphaBuf(GFXObj_t *object){
	BAG_Display_FreeGfxAlphaBuf(object);
	object->buffer.alpha = (u8 *)calloc( VAL(BAG_Display_GetGfxSize(object)), sizeof(char));
	if(BAG_Display_GetGfxAlphaMask(object) == NULL){
		BAG_DBG_LibMsg(BUFFER_FAILED_MSG, VAL(BAG_Display_GetGfxSize(object)) >> 1, "GFX alpha");
		return ERR_END;
	}
	memset(BAG_Display_GetGfxAlphaMask(object), 255, VAL(BAG_Display_GetGfxSize(object)) >> 1);
	SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ALPHA_MASK);
	return ERR_NONE;
}

u8 *BAG_Display_GetGfxAlphaPix(GFXObj_t *object, s16 x, s16 y){
	u8 *mask = BAG_Display_GetGfxAlphaMask(object);
	if(!mask) return NULL;

	if(!BAG_Display_CheckRange(object, x, y))
		return NULL;

	return (u8*)&mask[x + (y * VAL(BAG_Display_GetGfxWidth(object)))];
}

/*=================================================================================================
Functions for Creating graphic objects
=================================================================================================*/

/*===================================================
Delete and clear all buffers associated with an
object
===================================================*/
void BAG_Display_DeleteObj(GFXObj_t *object){
	BAG_Display_FreeGfxPalBuf(object);
	BAG_Display_FreeGfxBuf(object);
	BAG_Display_FreeGfxAlphaBuf(object);
	BAG_Display_ResetGfxScale(object);
	RESET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_EXISTS);
	memset(&object->data, 0, sizeof(struct BAG_GFX_DATA_s));
	memset(object, 0, sizeof(GFXObj_t));
	VAL(BAG_Display_GetGfxFlags(object)) = 0;
}


/*===================================================
Create a blank object
===================================================*/
int BAG_Display_CreateObj(GFXObj_t *object, s8 bits, s16 width, s16 height, s16 frame_wd, s16 frame_ht){
	int err = ERR_NONE;
	BAG_Display_DeleteObj(object);
	VAL(BAG_Display_GetGfxBits(object)) = bits;

	//create a palette buffer for 8 bit objects
	if(VAL(BAG_Display_GetGfxBits(object)) == 8){
		err = BAG_Display_CreateGfxPalBuf(object, 256);
		if(err != ERR_NONE)
			goto ERROR;
	}

	//create main graphics buffer
	err = BAG_Display_CreateGfxBuf(object, width, height);
	if(err != ERR_NONE)
		goto ERROR;

	SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_EXISTS);
	BAG_Display_SetGfxFrameDim(object, frame_wd, frame_ht);
	VAL(BAG_Display_GetGfxAnimStatus(object)) = -1;
	BAG_Display_ResetGfxScale(object);
	BAG_Display_SetGfxRenderMode(object, 0);
	BAG_Display_SetBrightnessOffset(object, 0);
	return ERR_NONE;

	ERROR:
	BAG_DBG_LibMsg("BAG:Failed to create graphics object\n");
	BAG_Display_DeleteObj(object);
	return err;
}


/*===================================================
Copy one object to a new one
===================================================*/
int BAG_Display_CloneObj(GFXObj_t *dest, GFXObj_t *source){
	//delete old destination object
	BAG_Display_DeleteObj(dest);

	//check if anything is loaded in the source object
	if(BAG_Display_GetGfxBuf(source) == NULL){
		BAG_DBG_LibMsg("BAG:Cloning from NULL GFX pointer!\n");
		return ERR_END;
	}

	int err = ERR_NONE;
	//delete and recreate the dest object to prepare for complete copy
	err = BAG_Display_CreateObj(dest, VAL(BAG_Display_GetGfxBits(source)),
								VAL(BAG_Display_GetGfxWidth(source)),
								VAL(BAG_Display_GetGfxHeight(source)),
								VAL(BAG_Display_GetGfxFrameWd(source)),
								VAL(BAG_Display_GetGfxFrameHt(source)));

	if(err != ERR_NONE)
		goto ERROR;

	//copy draw locations
	dest->blitX = source->blitX;
	dest->blitY = source->blitY;

	//now start copying one object to the other
	memcpy(&dest->data, &source->data, sizeof(struct BAG_GFX_DATA_s));
	memcpy(&dest->frame, &source->frame, sizeof(struct BAG_GFX_FRAME_s));

	//copy buffers now
	memcpy(BAG_Display_GetGfxBuf(dest), BAG_Display_GetGfxBuf(source),
		   VAL(BAG_Display_GetGfxSize(source)));

	memcpy(BAG_Display_GetGfxPalBuf(dest), BAG_Display_GetGfxPalBuf(source),
		   VAL(BAG_Display_GetGfxPalBufCount(source)) << 1);

	//now check if alpha buffer needs to be copied
	if(BAG_Display_GetGfxAlphaMask(source)){
		err = BAG_Display_CreateGfxAlphaBuf(dest);
		if(err != ERR_NONE)
			goto ERROR;

		memcpy(BAG_Display_GetGfxAlphaMask(dest), BAG_Display_GetGfxAlphaMask(source),
			   VAL(BAG_Display_GetGfxSize(dest)) >> 1);
	}

	return ERR_NONE;

	ERROR:
	BAG_Display_DeleteObj(dest);
	BAG_DBG_LibMsg("BAG:Error cloning graphics object\n");
	return err;
}

/*===================================================
Copy object data from one to another and share
buffers
===================================================*/
int BAG_Display_ObjIsCopy(GFXObj_t *object){
	return GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ISCOPY);
}

static void BAG_Display_SetObjIsCopy(GFXObj_t *object){
	SET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ISCOPY);
}

int BAG_Display_CopyObj(GFXObj_t *dest, GFXObj_t *source){
	BAG_Display_DeleteObj(dest);

	//copy draw locations
	dest->blitX = source->blitX;
	dest->blitY = source->blitY;

	//copy all object data
	memcpy(&dest->data, &source->data, sizeof(struct BAG_GFX_DATA_s));
	memcpy(&dest->frame, &source->frame, sizeof(struct BAG_GFX_FRAME_s));

	//now share buffers
	dest->buffer.gfx = BAG_Display_GetGfxBuf(source);
	dest->buffer.pal = BAG_Display_GetGfxPalBuf(source);
	dest->buffer.alpha = BAG_Display_GetGfxAlphaMask(source);
	BAG_Display_SetObjIsCopy(dest);
	return ERR_NONE;
}


/*=================================================================================================
Object animations
=================================================================================================*/
void BAG_Display_SetObjFrame(GFXObj_t *object, s8 dir, u16 frame){
	switch(dir){
		case FRAME_HOR:
			VAL(BAG_Display_GetGfxFrameX(object)) = frame;
		break;
		case FRAME_VERT:
			VAL(BAG_Display_GetGfxFrameY(object)) = frame;
		break;
	}
}


void BAG_Display_ResetObjFrame(GFXObj_t *object){
	BAG_Display_SetGfxFrameDim(object, VAL(BAG_Display_GetGfxWidth(object)),
							   VAL(BAG_Display_GetGfxHeight(object)));
}


void BAG_Display_StartAnim(GFXObj_t *object){
	VAL(BAG_Display_GetGfxAnimStatus(object)) = 0;
}


void BAG_Display_StopAnim(GFXObj_t *object){
	VAL(BAG_Display_GetGfxAnimStatus(object)) = -1;
}


void BAG_Display_SetObjOffset(GFXObj_t *object, s8 Frame_dir, u16 PixelOffset){
	switch(Frame_dir){
		case FRAME_HOR:
			VAL(BAG_Display_GetGfxOffsetX(object)) = PixelOffset;
		break;
		case FRAME_VERT:
			VAL(BAG_Display_GetGfxOffsetY(object)) = PixelOffset;
		break;
	}
}


void BAG_Display_UpdateAnim(GFXObj_t *object, u16 First_Frame, u16 Last_Frame, u16 increment, u16 Frame_Delay, s8 Frame_dir){
	//object not animated yet
	if(VAL(BAG_Display_GetGfxAnimStatus(object)) == 0){
		switch(Frame_dir){
			case FRAME_HOR:
				VAL(BAG_Display_GetGfxFrameX(object)) = First_Frame;
			break;
			case FRAME_VERT:
				VAL(BAG_Display_GetGfxFrameY(object)) = First_Frame;
			break;
		}
		VAL(BAG_Display_GetGfxAnimTimer(object)) = 0;
		VAL(BAG_Display_GetGfxAnimStatus(object)) = 1;
	}
	else if(VAL(BAG_Display_GetGfxAnimStatus(object)) == 1){
		VAL(BAG_Display_GetGfxAnimTimer(object))+= increment;
		if(VAL(BAG_Display_GetGfxAnimTimer(object)) >= Frame_Delay){
			int CurFrame = 0;
			switch(Frame_dir){
				case FRAME_HOR:
					VAL(BAG_Display_GetGfxFrameX(object))++;
					if(VAL(BAG_Display_GetGfxFrameX(object)) > Last_Frame)
						VAL(BAG_Display_GetGfxFrameX(object)) = First_Frame;

					CurFrame = VAL(BAG_Display_GetGfxFrameX(object));
				break;
				case FRAME_VERT:
					VAL(BAG_Display_GetGfxFrameY(object))++;
					if(VAL(BAG_Display_GetGfxFrameY(object)) > Last_Frame)
						VAL(BAG_Display_GetGfxFrameY(object)) = First_Frame;

					CurFrame = VAL(BAG_Display_GetGfxFrameY(object));
				break;
			}
			VAL(BAG_Display_GetGfxAnimTimer(object)) = 0;
			BAG_Display_SetObjFrame(object, Frame_dir, CurFrame);
		}
	}
}


/*===========================================================================
Bitmap Rendering
===========================================================================*/
//this function calculates how much of an image to draw rather
//than drawing the full image regardless if it is on screen or not
void _calculate_RenderDimensions(GFXObj_t *object, struct GFXRender_s *render, int destWd, int destHt){
	s16 TempWd = VAL(BAG_Display_GetGfxFrameWd(object)),
		TempHt = VAL(BAG_Display_GetGfxFrameHt(object));

	//set image width and height to scaled values  if set
	if(BAG_Display_GetGfxScaled(object)){
		TempWd = VAL(BAG_Display_GetGfxScaleWd(object));
		TempHt = VAL(BAG_Display_GetGfxScaleHt(object));
	}
	//printf("draw width: %d, draw height: %d\n", TempWd, TempHt);
	render->Width=TempWd;
	render->Height=TempHt;
	render->OffsetX = 0,render->OffsetY = 0;

	s16 xpos = VAL(BAG_Display_GetGfxBlitX(object)),
		ypos = VAL(BAG_Display_GetGfxBlitY(object));

	//x position calculations
	if(xpos < 0 ){
		if(BAG_Display_GetGfxFlipX(object))
			render->OffsetX = -xpos;
		else
			render->OffsetX = -xpos<<1;
		render->Width = TempWd  + xpos;
	}
	if(xpos + TempWd >=destWd){
		render->Width = (TempWd  -((xpos + TempWd ) - destWd));
		if(BAG_Display_GetGfxFlipX(object))
			render->OffsetX = (TempWd - render->Width);
		if(render->Width>destWd)render->Width = destWd;
	}

	//y position calculations
	if(ypos<0){
		if(BAG_Display_GetGfxFlipY(object))
			render->OffsetY = -ypos;
		else
			render->OffsetY = -ypos<<1;
		render->Height = TempHt + ypos;
	}
	if(ypos + TempHt >= destHt){
		render->Height = (TempHt - ((ypos + TempHt)-destHt));
		if(BAG_Display_GetGfxFlipY(object))
			render->OffsetY = (TempHt - render->Height);
		if(render->Height>destHt)render->Height = destHt;
	}

	render->ScrnOffsetX = xpos;
	render->ScrnOffsetY = ypos;


	render->ScrnOffsetX = (render->ScrnOffsetX < 0) ? 0 : render->ScrnOffsetX;
	render->ScrnOffsetY = (render->ScrnOffsetY < 0) ? 0 : render->ScrnOffsetY;

	render->X = (render->ScrnOffsetX == xpos) ? 0 : xpos;
	render->Y = (render->ScrnOffsetY == ypos) ? 0 : ypos;

	//calculate the frame position in the bitmap, does not need to take into account scaling
	s16 TempFrameX = VAL(BAG_Display_GetGfxFrameX(object)) * VAL(BAG_Display_GetGfxFrameWd(object)),
		TempFrameY = VAL(BAG_Display_GetGfxFrameY(object)) * VAL(BAG_Display_GetGfxFrameHt(object));

	render->xFrame = (TempFrameX < VAL(BAG_Display_GetGfxWidth(object)) && TempFrameX >= 0) ? TempFrameX : 0;
	render->yFrame = (TempFrameY < VAL(BAG_Display_GetGfxHeight(object)) && TempFrameY >= 0) ? TempFrameY : 0;

}

/*===========================================================================
Slow Bitmap Rendering
	-use this method for any objects that need:
		-flipping and or rotations,
		-alpha blending
		-any color effects (e.g. grey scale)
		-drawing with transparent color
===========================================================================*/

static void _calculate_FlipMode(struct GFXRender_s *render, s8 xflip, s8 yflip){
	if(xflip)
		render->xFlip = render->Width;

	if(yflip)
		render->yFlip =  render->Height;

	if(xflip && !yflip) render->FlipMode = 1;
	else if(!xflip && yflip) render->FlipMode = 2;
	else if(xflip && yflip)render->FlipMode = 3;
	else render->FlipMode=0;
}

static int _set_FlipMode(struct GFXRender_s *render, int destWd,  int x, int y){
	int dest = 0;
	switch(render->FlipMode){
		default: //No Flip
			dest =  (x+render->ScrnOffsetX + ((y + render->ScrnOffsetY ) * destWd));
		break;

		case 1://X flip
			dest =  (render->xFlip - ( x - render->ScrnOffsetX) + ((y + render->ScrnOffsetY ) * destWd));
		break;

		case 2://Y flip
			dest =  (x+render->ScrnOffsetX + ((render->yFlip - (y - render->ScrnOffsetY)) * destWd));
		break;

		case 3://X and Y Flip
			dest =  (render->xFlip - ( x - render->ScrnOffsetX) + ((render->yFlip - (y - render->ScrnOffsetY)) * destWd));
		break;
	}
	return dest;
}

static void _draw_Pixel(u16* dest, GFXObj_t *object,struct GFXRender_s *render, u16 color,
						int SrcIndex,int DestIndex)
{

  if(!BAG_Display_GetUseTransparentColor(object) && color == VAL(BAG_Display_GetGfxTranspCol(object)))
    return;

  //if 8 bit color then use the color from the color palette
  if (VAL(BAG_Display_GetGfxBits(object)) == 8)
    color = VAL(BAG_Display_GetGfxPalCol(object, color));

  //adjust colors if we have a font
  if(GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ISFONT)){
    //font colors are provided as grey scale values for alpha
    register int alpha = (color&31)<<3;
    color = VAL(BAG_Display_GetFontColor(object));
    color = BAG_Effects_CalcAlpha(color, dest[DestIndex], alpha);
  }

  //adjust brightness if necessary
  if(GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_BRIGHTNESS)){
    register int bright =  BAG_Effects_GetColBrightness(color);
    color = BAG_Effects_ColBrightness(color, bright + BAG_Display_GetBrightnessOffSet(object));
  }
  //apply grey scale if toggled
  if(GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_GREY_SCALE))
    color = BAG_Effects_ColorToGrey(color);
  //calculate alpha blending
  if(GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ALPHA_BLEND))
    color = BAG_Effects_CalcAlpha(color, dest[DestIndex], VAL(BAG_Display_GetGfxAlpha(object)));
  //check alpha mask
  if(BAG_Display_GetGfxAlphaMask(object)){
    u8 *alpha = BAG_Display_GetGfxAlphaMask(object);
    color = BAG_Effects_CalcAlpha(color, dest[DestIndex], alpha[SrcIndex]);
  }

  //finally place the newly processed color to the destination buffer
  dest[DestIndex] = color;
}

static void _calc_Rotation(GFXObj_t *object, const struct GFXRender_s *render,
							int drawX, int drawY, int cos1, int sin1, int *x, int *y)
{
	VAL(x)=((cos1*(drawX-(render->Width>>1))-sin1*(drawY-(render->Height>>1))+(render->Width>>1))>>8);
	VAL(y)=((sin1*(drawX-(render->Width>>1))+cos1*(drawY-(render->Height>>1))+(render->Height>>1))>>8);
}

void BAG_Display_DrawObjSlowEx(GFXObj_t *object, u16 *dest, s16 destWd, s16 destHt){
	//determine rendering proportions of graphic on screen
	struct GFXRender_s Render;
	memset(&Render, 0, sizeof(struct GFXRender_s));
	_calculate_RenderDimensions(object, &Render, destWd, destHt);
	if(Render.Width <= 0 || Render.Height <= 0 || !BAG_Display_GetGfxBuf(object))
		return;

	int drawY=0, drawX=0, destIndex = 0, srcIndex = 0,
		scaledX = VAL(BAG_Display_GetGfxFrameWd(object)),
		scaledY = VAL(BAG_Display_GetGfxFrameHt(object));

	 _calculate_FlipMode(&Render, BAG_Display_GetGfxFlipX(object), BAG_Display_GetGfxFlipY(object));

 	//check if graphic is to be resized
	int xscaling = 256, yscaling = 256;//set default scale to 1
	if(BAG_Display_GetGfxScaled(object)){
		xscaling = (VAL(BAG_Display_GetGfxFrameWd(object)) << 8) / VAL(BAG_Display_GetGfxScaleWd(object));
		yscaling = (VAL(BAG_Display_GetGfxFrameHt(object)) << 8) / VAL(BAG_Display_GetGfxScaleHt(object));
	}

	//precalculate sin and cos for rotation
	int cos1 = 0, sin1 = 0, x2 = 0, y2 = 0;
	cos1 = BAG_Math_Cos(-VAL(BAG_Display_GetGfxRot(object)));
	sin1 = BAG_Math_Sin(-VAL(BAG_Display_GetGfxRot(object)));


	//calculate some constant values used through out the drawing process
	int xConsts = Render.xFrame + VAL(BAG_Display_GetGfxOffsetX(object)) + Render.X + abs(Render.OffsetX);
	int yConsts = abs(Render.OffsetY)+ Render.Y + Render.yFrame + VAL(BAG_Display_GetGfxOffsetY(object));
	u16 *buf = BAG_Display_GetGfxBuf(object);
	u16 drawCol = 0;
	//draw loop
	int temp = 0;
	//y looping
	do{
		temp = drawY + Render.ScrnOffsetY;
		if(temp >= 0){

			//rest of image is offscreen if true, so stop drawing
			if(temp > destHt)
				break;
			//x looping
			drawX = 0;
			do{
				temp = drawX + Render.ScrnOffsetX;
				if(temp >= 0){
					if(temp > destWd)
						break;

					//calculate x and y positions based on rotations
					_calc_Rotation(object, &Render, drawX, drawY, cos1, sin1, &x2, &y2);
					//calculate destination positions for flipped gfx
					destIndex = _set_FlipMode(&Render, destWd, x2+(Render.Width>>1), y2+(Render.Height>>1));

					//calculate x and y scaled values
					if(BAG_Display_GetGfxScaled(object)){
						scaledX = (drawX * xscaling)>>8;
						scaledY = (drawY * yscaling)>>8;
					}
					else{
						scaledX = drawX;
						scaledY = drawY;
					}

					//draw pixel
					srcIndex = (xConsts+ scaledX + ((yConsts + scaledY ) * VAL(BAG_Display_GetGfxWidth(object))));
					drawCol = buf[srcIndex];

					//apply smoothing
					if(BAG_Display_GetGfxScaled(object) && VAL(BAG_Display_GetGfxBits(object)) > 8 &&
					   (drawCol != VAL(BAG_Display_GetGfxTranspCol(object)) || !BAG_Display_GetUseTransparentColor(object) ))
					{
						Smoothing_t pix = {
							buf,//buffer
							xConsts + scaledX,//x pos
							yConsts + scaledY,//y pos
							VAL(BAG_Display_GetGfxWidth(object)), //width
							VAL(BAG_Display_GetGfxHeight(object)), //height
							0
						};

						if(GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_4PIXSMOOTH)){
							if(GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ISFONT))
								drawCol = BAG_Effects_PixSmoothingFont(&pix);
							else drawCol = BAG_Effects_PixSmoothing(&pix);
						}
						else if(GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_8PIXSMOOTH)){
							pix.corners = 1;
							if(GET_FLAG(VAL(BAG_Display_GetGfxFlags(object)), OBJ_ISFONT))
								drawCol = BAG_Effects_PixSmoothingFont(&pix);
							else drawCol = BAG_Effects_PixSmoothing(&pix);
						}
					}
					_draw_Pixel(dest, object, &Render, drawCol, srcIndex, destIndex);
				}
			}while(++drawX < Render.Width);
		}
	}while(++drawY < Render.Height);
}


void BAG_Display_DrawObjSlow(GFXObj_t *object, u16* dest, s16 xpos, s16 ypos){
	BAG_Display_SetGfxBlitXY(object, xpos, ypos);
	BAG_Display_DrawObjSlowEx(object, dest, SCREEN_WIDTH, SCREEN_HEIGHT);
}


/*===========================================================================
Fast Bitmap Rendering
	-use this method for a line by line direct copy of an image to the screen.
	-can be used to draw images converted to effects
===========================================================================*/
void BAG_Display_DrawObjFastEx(GFXObj_t *object, u16* dest, s16 destWd, s16 destHt){
	//determine rendering proportions of graphic on screen
	struct GFXRender_s Render;
	memset(&Render, 0, sizeof(struct GFXRender_s));
	_calculate_RenderDimensions(object, &Render, destWd, destHt);
	u16 *buf = BAG_Display_GetGfxBuf(object);
	if(Render.Width <= 0 || Render.Height <= 0 || !buf)
		return;

	//draw the bitmap
	int drawY = -1,  destIndex = 0, srcIndex = 0;
	//pre calculate constant values before the loop
	int xConsts = Render.xFrame + VAL(BAG_Display_GetGfxOffsetX(object)) + Render.X + Render.OffsetX;
	int yConsts = Render.OffsetY+ Render.Y + Render.yFrame + VAL(BAG_Display_GetGfxOffsetY(object));


	while(++drawY < Render.Height){
		if(drawY + Render.ScrnOffsetY > destHt)
			break;

		destIndex = (Render.ScrnOffsetX+ ((drawY + Render.ScrnOffsetY) * destWd));
		srcIndex = ( xConsts + ((yConsts + drawY ) * VAL(BAG_Display_GetGfxWidth(object))));
		memcpy (&dest[destIndex], &buf[srcIndex],Render.Width<<1);
	}
}

void BAG_Display_DrawObjFast(GFXObj_t *object, u16* dest, s16 xpos, s16 ypos){
	BAG_Display_SetGfxBlitXY(object, xpos, ypos);
	BAG_Display_DrawObjFastEx(object, dest, SCREEN_WIDTH, SCREEN_HEIGHT);
}



void BAG_Display_DrawObj(GFXObj_t *object, u16 * dest, s16 xpos, s16 ypos){
	if(BAG_Display_GetGfxRenderMode(object))
		BAG_Display_DrawObjFast(object, dest, xpos, ypos);
	else
		BAG_Display_DrawObjSlow(object, dest, xpos, ypos);
}



/*===========================================================================
Master Load object function - auto detects image type and loads
===========================================================================*/

int BAG_Display_LoadObjEx(const char *filename, GFXObj_t *object, s16 frame_wd, s16 frame_ht){
	u8 magic_number[5];
	memset(magic_number, 0, 5);

	if(BAG_Filesystem_GetMagicNumbers(filename, magic_number, 5) == -1){
		BAG_DBG_LibMsg(OPEN_FAIL_MSG, filename);
		return ERR_END;
	}
	int err = ERR_END;
	//file is bitmap
	if(magic_number[0] == 'B' && magic_number[1] == 'M') {
		err = BAG_Display_LoadBmp(filename, object);
	}
	//file is jpg
#if defined(JPG_MODULE)
	else if(magic_number[0] == 255 && magic_number[1] == 216)
		err = BAG_Display_LoadJpg(filename, object);
#endif
#if defined(PNG_MODULE)
	//file is png;
	else if(magic_number[0] == 137 && magic_number[1] == 80 &&
			magic_number[2] == 78 && magic_number[3] == 71)
	{
		err = BAG_Display_LoadPng(filename,  object);
	}
#endif
	else
		return ERR_END;

	//free object data if there is an error in loading
	if(err != ERR_NONE) {
		BAG_Display_DeleteObj(object);
		return ERR_END;
	}

	//set default transparent color
	VAL(BAG_Display_GetGfxTranspCol(object)) = MAGENTA;
	//reset frame data
	BAG_Display_ResetObjFrame(object);
	//set default frame dimensions if none given
	if(frame_wd > -1 || frame_ht > -1)
		BAG_Display_SetGfxFrameDim(object, frame_wd, frame_ht);
	//reset graphics scaling
	BAG_Display_ResetGfxScale(object);

	return ERR_NONE;
}

int BAG_Display_LoadObj(const char *filename, GFXObj_t *object){
	return BAG_Display_LoadObjEx(filename, object, -1, -1);
}


int BAG_Display_LoadObjExt(const char *filename, GFXObj_t *object){
	//char newFile[MAX_PATH];
	char *newFile = NULL;
	int bufSize = 0;

	char exts[3][5] = {
		".bmp\0",
		".jpg\0",
		".png\0",
	};

	char *extlist[3];
	int i = 0, longest = 0;
	for(; i < 3; i++) {
		extlist[i] = (char*)&exts[i];
		if(strlen(extlist[i]) > longest) longest = strlen(extlist[i]);
	}

	//make sure we can allocate enough space for the full file path
	bufSize = strlen(filename) + longest;
	newFile = calloc(sizeof(char), bufSize + 1);
	if(!newFile) {
		BAG_DBG_LibMsg(BUFFER_FAILED_MSG, "LoadObjExt");
		return ERR_END;
	}

	if(BAG_Filesystem_FindFileExt(filename, extlist, 3, newFile, bufSize) == -1){
		free(newFile);
		BAG_DBG_LibMsg(OPEN_FAIL_MSG, filename);
		return ERR_END;
	}

	int status = BAG_Display_LoadObjEx(newFile, object, -1, -1);
	free(newFile);
	return status;
}


void BAG_Display_ReadObjSize(const char *filename, int *width, int *height){
	u8 magic_number[4];
	memset(magic_number, 0, sizeof(magic_number));

	if(BAG_Filesystem_GetMagicNumbers(filename, magic_number, 4) == -1){
		BAG_DBG_LibMsg(OPEN_FAIL_MSG, filename);
		return;
	}

	if(magic_number[0] == 'B' && magic_number[1] == 'M'){
		//file is bitmap
		BAG_Display_ReadBmpSize(filename, width, height);
	}
#if defined(JPG_MODULE)
	else if(magic_number[0] == 255 && magic_number[1] == 216){
		//file is jpg
		BAG_Display_ReadJpgSize(filename, width, height);
	}
#endif
#if defined(PNG_MODULE)
	else if(magic_number[0] == 137 && magic_number[1] == 80 && magic_number[2] == 78 && magic_number[3] == 71){
		//file is png;
		BAG_Display_ReadPngSize(filename, width, height);
	}
#endif
}

void BAG_Display_ReadObjSizeExt(const char *filename, int *width, int *height){
	char newFile[MAX_PATH];

	char exts[3][5] = {
		".bmp\0",
		".jpg\0",
		".png\0",
	};

	char *extlist[3];
	int i = 0;
	for(; i < 3; i++)
		extlist[i] = (char*)&exts[i];


	if(BAG_Filesystem_FindFileExt(filename, extlist, 3, newFile, MAX_PATH) == -1){
		BAG_DBG_LibMsg(OPEN_FAIL_MSG, filename);
		return;
	}
	BAG_Display_ReadObjSize(filename, width, height);
}

