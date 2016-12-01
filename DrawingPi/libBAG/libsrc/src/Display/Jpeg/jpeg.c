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

#include <ds2sdk.h>
#include "jpeg.h"
#include "../../Filesystem/filesystem.h"
#include "nanojpeg.h"


static void jpegFillObject(unsigned short *dest){
    unsigned int pos = 0;//buffer position
    unsigned char *image = njGetImage();//jpeg buffer

    unsigned char red = 0, green = 0, blue = 0;
    while(pos < njGetImageSize()){
        red = ((*image) >> 3) & 31;image++;
        green = ((*image) >> 2) & 63;image++;
        blue = ((*image) >> 3) & 31;image++;

        (*dest) = RGB15(red, green, blue);
        dest++;
        pos+=3;
    }
}


//Loads a jpeg and converts it to a bitmap object for sprite and background use
int BAG_Display_LoadJpgFromBuf(u8 *jpgData, unsigned int size, GFXObj_t *object){
  njInit();//initiate (clear memory)
  //decode image
  if(njDecode(jpgData, size)){
    BAG_DBG_LibMsg("BAG:Error decoding jpg.");
    return ERR_END;
  }
  //create object
  int err = BAG_Display_CreateObj(object, 16, njGetWidth(), njGetHeight(), njGetWidth(), njGetHeight());
  if(err != ERR_NONE)
    return err;
  //fill object pixel buffer
  jpegFillObject(BAG_Display_GetGfxBuf(object));
  //done!
  njDone();
  return ERR_NONE;
}


int BAG_Display_LoadJpg(const char *filename, GFXObj_t *object){
	//load the entire jpg file into a buffer
  u32 size = 0;
	u8 *buffer = BAG_Filesystem_LoadFileWithSize(filename, &size);
	if(buffer == NULL || size == 0){
    BAG_DBG_LibMsg(OPEN_FAIL_MSG, filename);
		return ERR_END;
  }

  int err = BAG_Display_LoadJpgFromBuf(buffer, size, object);
	if(buffer && size) free(buffer);
	return err;
}

void BAG_Display_GetJpgSize(unsigned char *jpgData, unsigned int size, int *width, int *height){
  njInit();
  int err = njReadHeader(jpgData,  size);
  if(!err){
    (*width) = njGetWidth();
    (*height) = njGetHeight();
  }
  njDone();
}


void BAG_Display_ReadJpgSize(const char *filename, int *width, int *height){
  u32 size = 0;
  u8 *buffer = BAG_Filesystem_LoadFileWithSize(filename, &size);
  if(buffer == NULL || size == 0){
    BAG_DBG_LibMsg(OPEN_FAIL_MSG, filename);
    return;
  }

  BAG_Display_GetJpgSize(buffer, size, width, height);
  if(buffer && size) free(buffer);
}
