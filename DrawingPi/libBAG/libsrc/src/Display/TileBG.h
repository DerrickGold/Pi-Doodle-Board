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

#ifndef __TILEBG_H__
#define __TILEBG_H__

#include <libBAG_types.h>
#include "Display.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLIPPED_HORIZONTALLY_FLAG 0x80000000
#define FLIPPED_VERTICALLY_FLAG 0x40000000
#define TRANSPARENT_FLAG 0x20000000

typedef struct TileAnimData{
  char frames, frameDiff, updated, oneshot;//how many frames to use
  unsigned short increment, timer, speed;
}TileAnimData;

typedef struct TiledBG_t{

	unsigned int width, height;//in tiles
	unsigned short tileWd, tileHt;
	GFXObj_t tiles;
	unsigned int *map;
  unsigned int numTiles;
  TileAnimData *AnimData;
  char divX, divY;
  char forceMode;

	//display properties
	int rowOut, colOut,
		screenX, screenY;
}TiledBG_t;

extern void BAG_TileBG_SetProperties(TiledBG_t *bg, int rowOut, int colOut, int screenX, int screenY);
extern void BAG_TileBG_SetDefaultProperties(TiledBG_t *bg);
extern char BAG_TileBG_LoadBGEx(const char *tilegfx, const char *tiledata, TiledBG_t *bg);
extern char BAG_TileBG_LoadBG(const char *bgFile, TiledBG_t *bg);
extern void BAG_TileBG_DeleteBG(TiledBG_t *bg);
extern void BAG_TileBG_DrawTile(unsigned short *dest, TiledBG_t *bg, unsigned int tileNum, int x, int y, int destWd, int destHt);
extern unsigned int BAG_TileBG_GetTile(const TiledBG_t *bg, int x, int y);
extern unsigned int BAG_TileBG_GetPixTile(const TiledBG_t *bg, int x, int y);
extern void BAG_TileBG_DrawBG(unsigned short *dest, TiledBG_t *bg, int x, int y);
extern void BAG_TileBG_DrawBGEx(unsigned short *dest, TiledBG_t *bg, int x, int y, int destWd, int destHt);
extern void BAG_TileBG_SetTile(const TiledBG_t *bg, int x, int y, unsigned int tileNum);
extern void BAG_TileBG_SetPixTile(const TiledBG_t *bg, int x, int y, unsigned int tileNum);
extern unsigned int BAG_TileBG_GetPixTileLocation(const TiledBG_t *bg, int x, int y);
extern void BAG_TileBG_SetTileAnimation(const TiledBG_t *bg, unsigned int tileNumber, char numFrames, int speed, int frameCount, char oneshot);
extern unsigned int *BAG_TileBG_SetTile_GetTileAddr(const TiledBG_t *bg, int x, int y);
extern unsigned int *BAG_TileBG_SetTile_GetTilePixAddr(const TiledBG_t *bg, int x, int y);


#ifdef __cplusplus
}
#endif


#endif
