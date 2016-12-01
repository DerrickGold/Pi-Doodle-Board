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
#ifdef __APPLE__
#include <libBAG_Debug.h>
#else
#include <debug/libBAG_Debug.h>
#endif

//#include "../Display.h"
#include "bmp.h"
#include "core.h"
#include <time.h>

#define RGB_SCREEN_FTM(src)				\
  b = ((src) >> 3)&31;					\
  g = ((src) >> 2)&63;				\
  r = ((src) >> 3)&31;

#define RGB_SCREEN_FTM_32(src)			\
  b = ((src) >> 3)&31;				\
  g = ((src) >> 2) &63;				\
  r = ((src) >> 3) &31;


#define BMP_BYTES_LINE(width,bits) ((((width * bits) +31)>>5)<<2)

//byte order
struct BMPHeader{
    char bfType[2];       /* "BM" */
    int bfSize;           /* Size of file in bytes */
    int bfReserved;       /* set to 0 */
    int bfOffBits;        /* Byte offset to actual bitmap data (= 54) */
    int biSize;           /* Size of BITMAPINFOHEADER, in bytes (= 40) */
    int biWidth;          /* Width of image, in pixels */
    int biHeight;         /* Height of images, in pixels */
    s16 biPlanes;       /* Number of planes in target device (set to 1) */
    s16 biBitCount;     /* Bits per pixel*/
    int biCompression;    /* Type of compression (0 if no compression) */
    int biSizeImage;      /* Image size, in bytes (0 if no compression) */
    int biXPelsPerMeter;  /* Resolution in pixels/meter of display device */
    int biYPelsPerMeter;  /* Resolution in pixels/meter of display device */
    int biClrUsed;        /* Number of colors in the color table (if 0, use
                             maximum allowed by biBitCount) */
    int biClrImportant;   /* Number of important colors.  If 0, all colors
                             are important */
} __attribute__ ((__packed__));

struct BMPLOADING{
    u8 * bytes;
    s16 bytesPerLine;
    int x, y, width, height;
    int hWidth, hHeight;
} __attribute__ ((aligned (4)));

/*===========================================================================
Bitmap Loading
===========================================================================*/
static struct BMPHeader ReadBmpHeader(const char *filename){
	struct BMPHeader header;
	FILE* bitmapFile=fopen(filename, "rb");
	if(bitmapFile){
		//offset to image data
		fseek(bitmapFile, 10, SEEK_SET);
		fread(&header.bfOffBits, sizeof(int),1, bitmapFile);
		//read the width
		fseek(bitmapFile, 18, SEEK_SET);
		fread(&header.biWidth, sizeof(int),1, bitmapFile);
		//read the height
		fseek(bitmapFile, 22, SEEK_SET);
		fread(&header.biHeight, sizeof(int), 1, bitmapFile);
		//bit count
		fseek(bitmapFile, 28, SEEK_SET);
		fread(&header.biBitCount, sizeof(s16), 1, bitmapFile);
		//get image size
		fseek(bitmapFile, 34, SEEK_SET);
		fread(&header.biSizeImage, sizeof(int), 1, bitmapFile);
		//num of colors used - 8bit
		fseek(bitmapFile, 46, SEEK_SET);
		fread(&header.biClrUsed, sizeof(int), 1, bitmapFile);
		fclose(bitmapFile);
	}
	return header;
}


static s8 _LoadBMP_Pal (FILE * bitmapFile, struct BMPHeader *header, GFXObj_t *object){
	s8 *Temp_Pal = bag_calloc(1024, sizeof(s8));
	if(Temp_Pal){
		//load the palette
		fseek(bitmapFile, 54, SEEK_SET);
		fread(Temp_Pal,1024,1,bitmapFile);
		//detect how many colors are in the palette
		s16 colorCount = header->biClrUsed;
		if(colorCount == 0)
			colorCount = 256;

		int err = BAG_Display_CreateGfxPalBuf(object, colorCount);
		if( err == ERR_NONE){
			s16 i = 0;
			for(i = 0; i < colorCount; i++) {
				u8 r = (Temp_Pal[ (i<<2)+2 ] >>3) & 31;
				u8 g = (Temp_Pal[ (i<<2)+1 ] >>3) & 31;
				u8 b = (Temp_Pal[ i<<2 ] >>3 & 31);
				VAL(BAG_Display_GetGfxPalCol(object, i)) = RGB15(r,g,b);
			}
		}
		else
			return err;

		//cleanup
		free(Temp_Pal);
		Temp_Pal = NULL;
		return ERR_NONE;
	}
    BAG_DBG_LibMsg(BUFFER_FAILED_MSG, 1024, "BMP Pal");
	return ERR_END;
}


static int _Read_BMP_8b(FILE *bitmapFile, struct BMPHeader *header, GFXObj_t *object, struct BMPLOADING *loadInfo){
	//read palette
	s8 test = _LoadBMP_Pal (bitmapFile, header, object);
	if(test == ERR_NONE){
		//seek to bitmap data
		fseek(bitmapFile, header->bfOffBits, SEEK_SET);

        int offset = loadInfo->hHeight - (loadInfo->y + VAL(BAG_Display_GetGfxHeight(object)));
        if(offset)
            fseek(bitmapFile, offset * loadInfo->bytesPerLine, SEEK_CUR);

		//load image data
		s16 Xload=0, Yload=0, xoffset = loadInfo->x;
		u16 *buf = BAG_Display_GetGfxBuf(object);

		for (Yload = VAL(BAG_Display_GetGfxHeight(object)) - 1; Yload > -1; Yload--, Xload = xoffset){
			fread(loadInfo->bytes, 1, loadInfo->bytesPerLine, bitmapFile);
			do
				buf[ (Xload - xoffset) + (Yload  * VAL(BAG_Display_GetGfxWidth(object)))] = loadInfo->bytes[Xload];
			while(++Xload < xoffset + VAL(BAG_Display_GetGfxWidth(object)));
		}
		return ERR_NONE;
	}
	else{
		BAG_DBG_LibMsg("BAG:Failed to read 8 bit bmp\n");
		return ERR_END;
	}
}


static int _Read_BMP_16b(FILE *bitmapFile, GFXObj_t *object, struct BMPLOADING *loadInfo){
	s16 Xload = 0,Yload = 0, xoffset = loadInfo->x;
	//u8 r = 0, g = 0, b = 0;
	u16 lobyte = 0, hibyte = 0, color = 0;

    int offset = loadInfo->hHeight - (loadInfo->y + VAL(BAG_Display_GetGfxHeight(object)));
    if(offset)
        fseek(bitmapFile, offset * loadInfo->bytesPerLine, SEEK_CUR);

	u16 *buf = BAG_Display_GetGfxBuf(object);
	for (Yload = VAL(BAG_Display_GetGfxHeight(object)) - 1; Yload > - 1; Yload--){
		fread(loadInfo->bytes,1,loadInfo->bytesPerLine,bitmapFile);
		for (Xload = xoffset; Xload < xoffset + VAL(BAG_Display_GetGfxWidth(object)); Xload++) {
			lobyte = loadInfo->bytes[(Xload<<1)];
			hibyte = loadInfo->bytes[((Xload<<1) + 1)];
			color = (hibyte << 8) +lobyte;
			buf[(Xload-xoffset) + (Yload  * VAL(BAG_Display_GetGfxWidth(object)))] =
				RGB15( (color>>10)&31, (color>>5)&31, (color & 31));
		}
	}
	return ERR_NONE;
}


static int _Read_BMP_24b(FILE *bitmapFile, GFXObj_t *object,  struct BMPLOADING *loadInfo){
	s16 Xload = 0,Yload = 0,i = 0, xoffset = loadInfo->x;
	u8 r=0,g=0,b=0;

    int offset = loadInfo->hHeight - (loadInfo->y + VAL(BAG_Display_GetGfxHeight(object)));
    if(offset)
        fseek(bitmapFile, offset * loadInfo->bytesPerLine, SEEK_CUR);

    int xbit = (xoffset * 3);

	u16 *buf = BAG_Display_GetGfxBuf(object);
	for (Yload = VAL(BAG_Display_GetGfxHeight(object)) - 1; Yload > -1; Yload--, i = xbit){
		fread(loadInfo->bytes,1,loadInfo->bytesPerLine,bitmapFile);
		for (Xload = xoffset; Xload <  xoffset + VAL(BAG_Display_GetGfxWidth(object)); Xload++) {

		  RGB_SCREEN_FTM(loadInfo->bytes[i++]);
		  //	b = (loadInfo->bytes[i++] >> 3)&31;
		  //	g = (loadInfo->bytes[i++] >> 3)&31;
		  //	r = (loadInfo->bytes[i++] >> 3)&31;
			buf[(Xload-xoffset) + (Yload  * VAL(BAG_Display_GetGfxWidth(object)))] =  RGB15(r,g,b);
		}
		while(i&3) i++;
	}
	return ERR_NONE;
}


static int _Read_BMP_32b(FILE *bitmapFile, GFXObj_t *object,  struct BMPLOADING *loadInfo){
	//create alpha mask
	if(BAG_Display_CreateGfxAlphaBuf(object) == ERR_NONE){
		s16 Xload = 0,Yload = 0,i = 0, xoffset = loadInfo->x;
		u8 r = 0, g = 0, b = 0;

        int offset = loadInfo->hHeight - (loadInfo->y + loadInfo->height);
        if(offset)
            fseek(bitmapFile, offset * loadInfo->bytesPerLine, SEEK_CUR);

        int xbit = (xoffset * 3);

		u16 *buf = BAG_Display_GetGfxBuf(object);
		for (Yload = VAL(BAG_Display_GetGfxHeight(object)) - 1; Yload > -1; Yload--, i = xbit){
			fread(loadInfo->bytes,1,loadInfo->bytesPerLine,bitmapFile);
			for (Xload = loadInfo->x; Xload <  xoffset + VAL(BAG_Display_GetGfxWidth(object)); Xload++){
			  VAL(BAG_Display_GetGfxAlphaPix(object, Xload - xoffset, Yload)) = loadInfo->bytes[i++];
			  RGB_SCREEN_FTM_32(loadInfo->bytes[i++]);
			  buf[(Xload-xoffset) + (Yload  * VAL(BAG_Display_GetGfxWidth(object)))] = RGB15(r,g,b);
				
			}
		}
		return ERR_NONE;
	}
    else{
        BAG_DBG_LibMsg("BAG:Failed to read 32 bit bmp\n");
        return ERR_END;
    }
}


static int _LoadBMP_(struct BMPHeader *header, GFXObj_t *object, struct BMPLOADING *loadInfo, const char* filename){
	if(header == NULL)
		return ERR_END;

	FILE* bitmapFile=fopen(filename, "rb");
	if(bitmapFile){
		 int err = BAG_Display_CreateObj(object, header->biBitCount, loadInfo->width, loadInfo->height, loadInfo->width, loadInfo->height);
		 if(err != ERR_NONE)
			return err;

		//seek to bitmap data
		fseek(bitmapFile, header->bfOffBits, SEEK_SET);
		//initialize object buffer
		if(BAG_Display_GetGfxBuf(object) != NULL){
			loadInfo->bytesPerLine = BMP_BYTES_LINE(VAL(BAG_Display_GetGfxWidth(object)), VAL(BAG_Display_GetGfxBits(object)));
			loadInfo->bytes = bag_calloc((loadInfo->bytesPerLine), sizeof(u8));

			err = ERR_NONE;
			switch(header->biBitCount){
				case 8:
					err = _Read_BMP_8b(bitmapFile, header, object, loadInfo);
				break;
				case 16:
					err = _Read_BMP_16b(bitmapFile, object, loadInfo);
				break;
				case 24:
					err = _Read_BMP_24b(bitmapFile, object, loadInfo);
				break;
				case 32:
					err = _Read_BMP_32b(bitmapFile, object, loadInfo);
				break;
			}
			//clean up
			free(loadInfo->bytes);
			loadInfo->bytes=NULL;
			fclose(bitmapFile);
			return err;
		}
		fclose(bitmapFile);
		return ERR_END;
	}
    BAG_DBG_LibMsg(OPEN_FAIL_MSG, filename);
	return ERR_END;
}


int BAG_Display_ReloadBmp(const char *filename, GFXObj_t *object){
    int tempWidth = VAL(BAG_Display_GetGfxFrameWd(object));
    int tempHeight = VAL(BAG_Display_GetGfxFrameHt(object));

	int err = BAG_Display_LoadBmp( filename, object);
	BAG_Display_SetGfxFrameDim(object, tempWidth, tempHeight);
	return err;
}

int BAG_Display_LoadBmp(const char *filename, GFXObj_t *object){
	struct BMPHeader BitmapInfo;
	BitmapInfo = ReadBmpHeader(filename);
	if( BitmapInfo.biWidth * BitmapInfo.biHeight > 0 ){
        struct BMPLOADING bitmapload;
        //load the whole bitmap rather than a chunk of it
        bitmapload.x = 0;
        bitmapload.y = 0;
        bitmapload.hWidth = bitmapload.width = BitmapInfo.biWidth;
        bitmapload.hHeight = bitmapload.height = BitmapInfo.biHeight;

		return _LoadBMP_(&BitmapInfo, object, &bitmapload, filename);
	}
    BAG_DBG_LibMsg(OPEN_INVALID_MSG, filename);
	return ERR_END;
}

int BAG_Display_LoadBmpChunk( const char *filename, GFXObj_t *object, s16 pix_x1, s16 pix_y1, s16 pix_x2, s16 pix_y2){
	struct BMPHeader BitmapInfo;
	BitmapInfo = ReadBmpHeader(filename);
	if( BitmapInfo.biWidth * BitmapInfo.biHeight > 0 ){
        struct BMPLOADING bitmapload;
        bitmapload.hWidth =  BitmapInfo.biWidth;
        bitmapload.hHeight = BitmapInfo.biHeight;
        //set image chunk dimensions
        bitmapload.x = pix_x1;
        bitmapload.y = pix_y1;

        int width = abs(pix_x2 - pix_x1);
        if(pix_x1 + width <= BitmapInfo.biWidth)
            bitmapload.width = width;
        else
            bitmapload.width = (pix_x1 + width) - BitmapInfo.biWidth;

        int height = abs(pix_y2 - pix_y1);
        if(pix_y1 + height <= BitmapInfo.biHeight)
            bitmapload.height = height;
        else
            bitmapload.height = (pix_y1 + height) - BitmapInfo.biHeight;

		return _LoadBMP_(&BitmapInfo, object, &bitmapload, filename);
	}
    BAG_DBG_LibMsg(OPEN_INVALID_MSG, filename);
	return ERR_END;
}

void BAG_Display_ReadBmpSize(const char *filename, int *width, int *height){
    struct BMPHeader info = ReadBmpHeader(filename);
    (*width) = info.biWidth;
    (*height) = info.biHeight;
}


/*===========================================================================
Screen Capture
===========================================================================*/
s8 BAG_Display_ScrnCap(s8 Screen, const char *Directory){
  //get date for file name
  //struct rtc Date;
  //ds2_getTime(&Date);

  char ScreenCapName[MAX_PATH];
  time_t t = time(NULL);
  struct tm Date = *localtime(&t);

  
  sprintf(ScreenCapName, "%s/20%d-%02d-%02d_%02d.%02d.%02d.bmp", Directory, Date.tm_year, Date.tm_mon, Date.tm_mday,
	  Date.tm_hour, Date.tm_min, Date.tm_sec);

  FILE* file = fopen(ScreenCapName, "wb");
  if(file == NULL){
    BAG_DBG_LibMsg(OPEN_FAIL_WRITE_MSG, ScreenCapName);
    return ERR_END; //error
  }

  int width = SCREEN_WIDTH;
  int height = SCREEN_HEIGHT;

  int Bits = 24;
  int bytesPerLine = (((width* Bits) +31)/32)*4;;
  //u8 *line = NULL;

  struct BMPHeader bmph;

  //fill the header
  strcpy(bmph.bfType, "BM");
  bmph.bfOffBits = 54;
  bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
  bmph.bfReserved = 0;
  bmph.biSize = 40;
  bmph.biWidth = width;
  bmph.biHeight = height;
  bmph.biPlanes = 1;
  bmph.biBitCount = Bits;
  bmph.biCompression = 0;
  bmph.biSizeImage = bytesPerLine * height;
  bmph.biXPelsPerMeter = 0;
  bmph.biYPelsPerMeter = 0;
  bmph.biClrUsed = 0;
  bmph.biClrImportant = 0;
  
  //write header
  fwrite(&bmph.bfType, 2, 1, file);
  fwrite(&bmph.bfSize, 4, 1, file);
  fwrite(&bmph.bfReserved, 4, 1, file);
  fwrite(&bmph.bfOffBits, 4, 1, file);
  fwrite(&bmph.biSize, 4, 1, file);
  fwrite(&bmph.biWidth, 4, 1, file);
  fwrite(&bmph.biHeight, 4, 1, file);
  fwrite(&bmph.biPlanes, 2, 1, file);
  fwrite(&bmph.biBitCount, 2, 1, file);
  fwrite(&bmph.biCompression, 4, 1, file);
  fwrite(&bmph.biSizeImage, 4, 1, file);
  fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
  fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
  fwrite(&bmph.biClrUsed, 4, 1, file);
  fwrite(&bmph.biClrImportant, 4, 1, file);
  
  unsigned char line[bytesPerLine];
  int x, y;
  s16 red, green, blue;
  u16 color = 0;

  unsigned short *screen = BAG_GetScreen();
  
  
  for (y = height - 1; y >= 0; y--){
    for (x = 0; x < width+1; x++){
      color = (u16) screen[ x +( y * width)];

      blue=(color&31)<<3;
      green=((color>>6)&63)<<2;
      red=((color>>11)&31)<<3;
      line[3*x] = red;
      line[3*x+1] = green;
      line[3*x+2] = blue;
    }
    fwrite(line, 1, bytesPerLine, file);
  }
  
  fclose(file);
  return ERR_NONE;
}

/*===========================================================================
Screen Capture
===========================================================================*/
s8 BAG_Display_GfxToBitmapFile(GFXObj_t *object, const char *Directory, const char *name){

  char ScreenCapName[MAX_PATH];

  if(name) {
    sprintf(ScreenCapName, "%s/%s.bmp", Directory, name);
  } else {
    time_t t = time(NULL);
    struct tm Date = *localtime(&t);

    sprintf(ScreenCapName, "%s/20%d-%02d-%02d_%02d.%02d.%02d.bmp", Directory, Date.tm_year, Date.tm_mon, Date.tm_mday,
	    Date.tm_hour, Date.tm_min, Date.tm_sec);
  }
  
  if (!strlen(ScreenCapName)) return ERR_END; //error

  FILE* file = fopen(ScreenCapName, "wb");
  if(file == NULL){
    BAG_DBG_LibMsg(OPEN_FAIL_WRITE_MSG, ScreenCapName);
    return ERR_END; //error
  }

  int width = *BAG_Display_GetGfxWidth(object);
  int height = *BAG_Display_GetGfxHeight(object);

  //for now, just write out all graphics in 24 bit format
  int Bits = 24;
  int bytesPerLine = (((width* Bits) +31)/32)*4;;
  
  struct BMPHeader bmph = {
    .bfOffBits = 54,
    .bfSize = bmph.bfOffBits + bytesPerLine * height,
    .bfReserved = 0,
    .biSize = 40,
    .biWidth = width,
    .biHeight = height,
    .biPlanes = 1,
    .biBitCount = Bits,
    .biCompression = 0,
    .biSizeImage = bytesPerLine * height,
    .biXPelsPerMeter = 0,
    .biYPelsPerMeter = 0,
    .biClrUsed = 0,
    .biClrImportant = 0,
    
  };
  //fill the header
  strcpy(bmph.bfType, "BM");

  //write header
  fwrite(&bmph, bmph.bfOffBits, 1, file);
  
 
  unsigned char line[bytesPerLine];
  int x, y;
  u16 color = 0;
  unsigned short *screen =  BAG_Display_GetGfxBuf(object);
  
  for (y = height - 1; y >= 0; y--){
    for (x = 0; x < width; x++){
      color = (u16) screen[ x +( y * width)];
       
      line[3*x+2] = ((color>>11)&31)<<3;//blue
      line[3*x+1] = ((color>>5)&63)<<2;//green
      line[3*x] = (color&31)<<3;//red
    }
    fwrite(line, 1, bytesPerLine, file);
  }
  
  fclose(file);
  return ERR_NONE;
}
