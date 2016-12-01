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
#include "TileBG.h"

#ifdef __APPLE__
#include <libBAG_Debug.h>
#else
#include <debug/libBAG_Debug.h>
#endif


//ToDo: Add tiled text modes!

static inline char divVal(int dimension){
    int x = 1, multiplier = 1;
    while(x << multiplier != dimension)
        multiplier++;
    return multiplier;
}


void BAG_TileBG_SetProperties(TiledBG_t *bg, int rowOut, int colOut, int screenX, int screenY){
	bg->rowOut = rowOut;
	bg->colOut = colOut;
	bg->screenX = screenX;
	bg->screenY = screenY;
}

void BAG_TileBG_SetDefaultProperties(TiledBG_t *bg){
	BAG_TileBG_SetProperties(bg, SCREEN_HEIGHT >> bg->divY, SCREEN_WIDTH >> bg->divX, 0, 0);
}



/*============================================================================================================
Loading and deleting of backgrounds
============================================================================================================*/
void BAG_TileBG_DeleteMap(TiledBG_t *bg){
  if(bg->map && (bg->width || bg->height))
    free(bg->map);
  bg->map = NULL;

  if(bg->AnimData && bg->numTiles > 0)
      free(bg->AnimData);
  bg->AnimData = NULL;
}


unsigned int BAG_TileBG_BlankMap(TiledBG_t *bg, short tWd, short tHt, int wd, int ht){
  bg->width = wd;
  bg->height = ht;
  bg->tileWd = tWd;
  bg->tileHt = tHt;

  bg->divX = divVal(bg->tileWd);
  bg->divY = divVal(bg->tileHt);

  unsigned int mapSize = bg->width * bg->height;
  bg->map = calloc(4, mapSize);
  if(!bg->map){
    BAG_DBG_LibMsg(BUFFER_FAILED_MSG, 4*mapSize, "Tile Map");
    return 0;
  }
  return mapSize;
}

char BAG_TileBG_LoadMap(const char *tileMap, TiledBG_t *bg){
  //load tile map data here

  FILE *file = fopen(tileMap, "rb");
  if(!file){
    BAG_DBG_LibMsg(OPEN_FAIL_MSG, tileMap);
    return ERR_END;
  }

  BAG_TileBG_DeleteMap(bg);

  int wd = 0, ht = 0;
  short tWd = 0, tHt = 0;
  //BAG_DBG_Msg("reading tile data\n");
  fread(&wd, 1, 4, file);
  fread(&ht, 1, 4, file);
  fread(&tWd, 1, 2, file);
  fread(&tHt, 1, 2, file);

  unsigned int mapSize = BAG_TileBG_BlankMap(bg, tWd, tHt, wd, ht);
  if(!mapSize){
    fclose(file);
    return ERR_END;
  } else {
    fread(bg->map, 4, mapSize, file);
    fclose(file);
  }
  return ERR_NONE;
}

void BAG_TileBG_DeleteGfx(TiledBG_t *bg){
  BAG_Display_DeleteObj(&bg->tiles);
}

char BAG_TileBG_LoadGfx(const char *tileGFX, TiledBG_t *bg){
  BAG_TileBG_DeleteGfx(bg);
  //load tile graphics
  if(BAG_Display_LoadObjExt(tileGFX, &bg->tiles) != ERR_NONE){
    BAG_DBG_LibMsg(OPEN_FAIL_MSG, tileGFX);
    free(bg->map);
    return ERR_END;
  }
  BAG_Display_SetGfxFrameDim(&bg->tiles, bg->tileHt, bg->tileHt);

  BAG_TileBG_SetDefaultProperties(bg);

  //tile gfx Ht * Wd
  bg->numTiles = ((*BAG_Display_GetGfxWidth(&bg->tiles)) >> bg->divX) *
                  ((*BAG_Display_GetGfxHeight(&bg->tiles)) >> bg->divY);

  bg->AnimData = calloc(1, sizeof(TileAnimData) * (bg->numTiles + 1));
  if(!bg->AnimData){
    BAG_DBG_LibMsg(BUFFER_FAILED_MSG, sizeof(TileAnimData) * (bg->numTiles + 1), "Tile Animations");
    return ERR_END;
  }

  return ERR_NONE;
}

void BAG_TileBG_DeleteBG(TiledBG_t *bg){
  BAG_TileBG_DeleteMap(bg);
  BAG_TileBG_DeleteGfx(bg);
  memset(bg, 0, sizeof(TiledBG_t));
}

char BAG_TileBG_LoadBGEx(const char *tilegfx, const char *tiledata, TiledBG_t *bg){
  if(BAG_TileBG_LoadMap(tiledata, bg)){
    if(BAG_TileBG_LoadGfx(tilegfx, bg))
      return ERR_NONE;
  }

  BAG_TileBG_DeleteBG(bg);
  BAG_DBG_LibMsg("BAG:Failed to load tile bg\n");
  return ERR_END;
}

char BAG_TileBG_LoadBG(const char *bgFile, TiledBG_t *bg){
  //char tilePath[MAX_PATH];
  size_t pathLen = strlen(bgFile) + strlen(".tbag") + 1;
	char *tilePath = calloc(sizeof(char), pathLen);
	if(!tilePath){
		BAG_DBG_LibMsg("BAG:Error generating tilebg name\n");
		return ERR_END;
	}
  	strncpy(tilePath, bgFile, pathLen);
  	strncat(tilePath, ".tbag", pathLen);
    printf("tile data: %s\n", tilePath);

  	int status = BAG_TileBG_LoadBGEx(bgFile, tilePath, bg);

  	free(tilePath);
  	return status;
}




/*char BAG_TileBG_LoadAsciiMap(const char *txtFile, TiledBG_t *bg){
  FILE *file = fopen(txtFile, "rb");
  if(!file)
    return 0;

  BAG_TileBG_DeleteMap(bg);

  fscanf (file, "[%d,%d]\n[%d,%d]\n", &bg->width, &bg->height, &bg->tileWd, &bg->tileHt);
  bg->divX = divVal(bg->tileWd);
  bg->divY = divVal(bg->tileHt);

  unsigned int mapSize = bg->width * bg->height;
  bg->map = calloc(4, mapSize);
  if(!bg->map){
    //BAG_DBG_Msg("failed to allocate memory for map\n");
    fclose(file);
    return 0;
  }
  int line = 0;

  int *tempWd = calloc(4, bg->width + 2);
  while(!feof(file)){
    char* temp = fgets (tempWd, bg->width, file);
    if(temp == NULL)
      break;
    memcpy(&bg->map[line * bg->width], tempWd, bg->width);
    line++;
  }

  free(tempWd);
  tempWd = NULL;

  fclose(file);
  return 1;
}*/

/*============================================================================================================
Accessing bg information
============================================================================================================*/




unsigned int BAG_TileBG_GetTile(const TiledBG_t *bg, int x, int y){
	if(x < 0 || x >= bg->width || y < 0 || y >= bg->height)
		return 0;

  unsigned int tile = bg->map[x + (y * bg->width)];
  tile &= ~(FLIPPED_VERTICALLY_FLAG | FLIPPED_HORIZONTALLY_FLAG | TRANSPARENT_FLAG);
	return tile;
}

void BAG_TileBG_SetTile(const TiledBG_t *bg, int x, int y, unsigned int tileNum){
    if(x < 0 || x >= bg->width || y < 0 || y >= bg->height)
        return;

    bg->map[x + (y * bg->width)] = tileNum;
}




unsigned int BAG_TileBG_GetPixTile(const TiledBG_t *bg, int x, int y){
	register int tileX = x >> bg->divX,
				 tileY = y >> bg->divY;

	return BAG_TileBG_GetTile(bg, tileX, tileY);
}


unsigned int BAG_TileBG_GetPixTileLocation(const TiledBG_t *bg, int x, int y){
    register int tileX = x >> bg->divX,
                 tileY = y >> bg->divY;

    return (tileX + (tileY * bg->width));
}

unsigned int *BAG_TileBG_SetTile_GetTileAddr(const TiledBG_t *bg, int x, int y){
  if(x < 0 || x >= bg->width || y < 0 || y >= bg->height)
    return NULL;
  return (unsigned int*)&bg->map[x + (y * bg->width)];
}

unsigned int *BAG_TileBG_SetTile_GetTilePixAddr(const TiledBG_t *bg, int x, int y){
  register int tileX = x >> bg->divX,
         tileY = y >> bg->divY;

  return BAG_TileBG_SetTile_GetTileAddr(bg, tileX, tileY);
}


void BAG_TileBG_SetPixTile(const TiledBG_t *bg, int x, int y, unsigned int tileNum){
    register int tileX = x >> bg->divX,
                 tileY = y >> bg->divY;

    BAG_TileBG_SetTile(bg, tileX, tileY, tileNum);
}




/*============================================================================================================
Animated tiles
============================================================================================================*/

void BAG_TileBG_SetTileAnimation(const TiledBG_t *bg, unsigned int tileNumber, char numFrames, int speed, int frameCount, char oneshot){
    tileNumber -= (tileNumber > 0);
    TileAnimData *data = &bg->AnimData[tileNumber];

    data->frames = numFrames;
    data->increment = speed;
    data->speed = frameCount;
    data->timer = 0;
    data->frameDiff = 0;
    data->oneshot = oneshot;
    if(data->oneshot == 0)
      data->oneshot=2;
}


//returns new tileNumber
static unsigned int updateAnim(TiledBG_t *bg, unsigned int tileNum){
  TileAnimData *data = &bg->AnimData[tileNum];
  if(!data->frames)//check if tile is animated
      return tileNum;


  data->timer += (data->increment * (1 - data->updated));
  if(data->timer >= data->speed && data->oneshot > 0){
      data->frameDiff++;
      if(data->frameDiff > data->frames){
        data->frameDiff = 0;
        if(data->oneshot == 1){
          data->oneshot--;
          data->frameDiff = data->frames;//stop on last frame
        }
      }
      data->timer = 0;
  }
  data->updated = 1;

  return tileNum + data->frameDiff;
}

static inline void refreshAnims(TiledBG_t *bg){
  TileAnimData *data = bg->AnimData;
  register int i = 0;
  for(i = 0; i < bg->numTiles; i++){
      data[i].updated = 0;
  }
}





/*============================================================================================================
Background drawing information
============================================================================================================*/

static inline void _BAG_TileBG_GetTileGfxOffset(TiledBG_t *bg, unsigned int tileNum, int *x, int *y){
  int tilesPerRow = (*BAG_Display_GetGfxWidth(&bg->tiles)) >> bg->divX;
  *y = tileNum / tilesPerRow;
  *x = abs(tileNum - ((*y) * tilesPerRow));

  *x *= bg->tileWd;
  *y *= bg->tileHt;
}

void BAG_TileBG_DrawTile(unsigned short *dest, TiledBG_t *bg, unsigned int tileNum, int x, int y, int destWd, int destHt){

  //check if tile is being drawn in range
  if(x + bg->tileWd < 0 || x > destWd || y + bg->tileHt < 0 || y > destHt)
    return;

  //set default tile properties
  BAG_Display_SetGfxFrameDim(&bg->tiles, bg->tileWd, bg->tileHt);
  BAG_Display_SetGfxFlipX(&bg->tiles, 0);
  BAG_Display_SetGfxFlipY(&bg->tiles, 0);
  BAG_Display_SetGfxRenderMode(&bg->tiles, 1);//use fast render mode to draw tiles

  //now to grab the proper tile properties
  if(tileNum & FLIPPED_HORIZONTALLY_FLAG)
    BAG_Display_SetGfxFlipX(&bg->tiles, 1);
  if(tileNum & FLIPPED_VERTICALLY_FLAG)
    BAG_Display_SetGfxFlipY(&bg->tiles, 1);
  if(tileNum & TRANSPARENT_FLAG || bg->forceMode == 1)//transparency needed, use slow rendering for tile
    BAG_Display_SetGfxRenderMode(&bg->tiles, 0);

  //get the tile number to plot
  tileNum &= ~(FLIPPED_VERTICALLY_FLAG | FLIPPED_HORIZONTALLY_FLAG | TRANSPARENT_FLAG);

  //find the graphics based on the tile
  int offX = 0, offY = 0;
  //if tile is animated, get the new tile frame
  tileNum = updateAnim(bg, tileNum);
  //grab offset in graphics for the tile and set the current graphics to this offset
  _BAG_TileBG_GetTileGfxOffset(bg, tileNum, &offX, &offY);
  BAG_Display_SetGfxOffsetXY(&bg->tiles, offX, offY);

  //clip tile graphics that are out of bounds
  int newX = 0, newY = 0;
  //y clipping
  if(y < bg->screenY){
    int ht = bg->tileHt - abs(bg->screenY - y);
    (*BAG_Display_GetGfxFrameHt(&bg->tiles)) = ht;
    newY = abs(bg->screenY - y);
    (*BAG_Display_GetGfxOffsetY(&bg->tiles)) += newY;
  }
  else if(y + bg->tileHt >= bg->screenY + (bg->rowOut * bg->tileHt)){
    int ht = abs((bg->rowOut * bg->tileHt) + bg->screenY - y);
    (*BAG_Display_GetGfxFrameHt(&bg->tiles)) = ht;
  }

  //x clipping
  if(x < bg->screenX){
    int wd = bg->tileWd - abs(bg->screenX - x);
    (*BAG_Display_GetGfxFrameWd(&bg->tiles)) = wd;
    newX = abs(bg->screenX - x);
    (*BAG_Display_GetGfxOffsetX(&bg->tiles)) += newX;
  }
  else if(x + bg->tileWd >= bg->screenX + (bg->colOut * bg->tileWd)){
    int wd = abs((bg->colOut * bg->tileWd) + bg->screenX - x);
    (*BAG_Display_GetGfxFrameWd(&bg->tiles)) = wd;
  }

  //draw the tile
  BAG_Display_SetGfxBlitXY(&bg->tiles, x + newX, y + newY);
  if(!BAG_Display_GetGfxRenderMode(&bg->tiles))
    BAG_Display_DrawObjSlowEx(&bg->tiles, dest, destWd, destHt);
  else
    BAG_Display_DrawObjFastEx(&bg->tiles, dest, destWd, destHt);

}

void BAG_TileBG_DrawBGEx(unsigned short *dest, TiledBG_t *bg, int x, int y, int destWd, int destHt){
  //BAG_DBG_Msg("scrollX: %d\nscrollY: %d\n", x, y);
  register int i = 0, j = 0, drawX = 0, drawY = 0;
  unsigned int tileNum = 0;
  //draw tiles to screen
  for(i = 0; i <= bg->colOut; i++){
    for(j = 0; j <= bg->rowOut; j++){

      if(!(tileNum = BAG_TileBG_GetTile(bg, ((x) >> bg->divX) + i, ((y) >> bg->divY) + j)))
        continue;

      //0 counts as a tile on the graphics, so subtract one from tile number to get proper tile
      tileNum--;
            //account for partially displayed tiles that may or may not be off screen
      drawX = -((bg->screenX + (x)) & ((1<<bg->divX)-1));
      drawY = -((bg->screenY + (y)) & ((1<<bg->divY)-1));
      BAG_TileBG_DrawTile(dest, bg, tileNum, bg->screenX + drawX + (i << bg->divX), bg->screenY + drawY + (j << bg->divY), destWd, destHt);
    }
  }
  refreshAnims(bg);
}



void BAG_TileBG_DrawBG(unsigned short *dest, TiledBG_t *bg, int x, int y){
  BAG_TileBG_DrawBGEx(dest, bg, x, y, SCREEN_WIDTH, SCREEN_HEIGHT);
}
