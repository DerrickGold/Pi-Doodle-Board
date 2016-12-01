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

//#include <libBAG.h>
#include <libBAG_types.h>
#include "Display.h"
#include "Draw.h"
void BAG_Effects_ScreenFX(u16* ScreenBuf, s8 filter_type){
	s16 x,y;
	switch(filter_type){
		case 1://grey scale
			for(y=0;y<SCREEN_HEIGHT;++y){
				for(x=0;x<SCREEN_WIDTH;++x)
					ScreenBuf[x+ (y<<8)] = BAG_Effects_ColorToGrey(ScreenBuf[x+ (y<<8)]);
			}
		break;
	}
}

u16 BAG_Effects_ColBrightness(u16 color, s8 brightness){
	//max brightness is 93 (31+31+31)
	register int red=color&31;
	register int green=color>>5&31;
	register int blue=color>>10&31;
	register int newbrightness = red+green+blue;

	while (newbrightness>=brightness && newbrightness > 0){
		red -= (red>0);
		green -= (green>0);
		blue -= (blue>0);
		newbrightness = red+green+blue;
	}
	while (newbrightness<brightness && newbrightness < 93){
    red+=(red<31);
		green+=(green<31);
		blue+=(blue<31);
		newbrightness = red+green+blue;
	}
	return RGB15(red,green,blue);
}

unsigned char BAG_Effects_GetColBrightness(u16 color){
  register int red=color>>11&31;
  register int green=color>>6&63;
  register int blue=color&31;
  return red+green+blue;
}

/*===========================================================================
Grey Scale Conversion
===========================================================================*/
u16 BAG_Effects_ColorToGrey(u16 Color){
	u16 Grey = ((Color&31) * 38 + ((Color>>5)&31) * 75 + ((Color>>10)&31) * 15) >> 7;
	return RGB15(Grey,Grey,Grey);
}

#define PIXAVG_R5G5B5(a,b) ((a + b + ((a ^ b) & 0x0421)) >> 1)
u16 BAG_Effects_PixSmoothing(const Smoothing_t *smooth){
  register int curColor[9];
  register int  x = smooth->x, y = smooth->y, wd = smooth->wd, ht = smooth->ht;

  //center pixel
  curColor[0] = smooth->buf[x + y * wd];
  //left pixel
  x--;
  x += (x < 0);
  curColor[1] = smooth->buf[x + y * wd];
  //right pixel
  x+=2;
  x -= (x > wd);
  curColor[2] = smooth->buf[x + y * wd];
  //reset x to main pix
  x--;
  //up pixel
  y--;
  y += (y < 0);
  curColor[3] = smooth->buf[x + y * wd];
  //down pixel
  y+=2;
  y -= (y > ht);
  curColor[4] = smooth->buf[x + y * wd];

  if(smooth->corners){
    //reset pos to center pixel
    y--;
    //left corners
    x--;
    x += (x < 0);

    //left top
    y--;
    y += (y < 0);
    curColor[5] = smooth->buf[x + y * wd];
    //left bottom
    y+=2;
    y -= (y > ht);
    curColor[6] = smooth->buf[x + y * wd];

    //right corners
    x+=2;
    x -= (x > wd);
    //right bottom
    curColor[7] = smooth->buf[x + y * wd];
    //right yop
    y-=2;
    y += (y < 0);
    curColor[8] = smooth->buf[x + y * wd];
    //8 pixel average(lol)
    return PIXAVG_R5G5B5(curColor[0], PIXAVG_R5G5B5(curColor[1], PIXAVG_R5G5B5(curColor[2], PIXAVG_R5G5B5(curColor[3],
                          //last normal pixel.......now the corner pixels
                          PIXAVG_R5G5B5(curColor[4], PIXAVG_R5G5B5(curColor[5], PIXAVG_R5G5B5(curColor[6], PIXAVG_R5G5B5(curColor[7], curColor[8]))))))));
  }
  //4 pixel average
  return PIXAVG_R5G5B5(curColor[0], PIXAVG_R5G5B5(curColor[1], PIXAVG_R5G5B5(curColor[2], PIXAVG_R5G5B5(curColor[3], curColor[4]))));
}

#define GET_RED(x)((x&31))
#define AVG_ALPHA(a,b)((GET_RED(a) + GET_RED(b))>>1)
#define PIXAVG_ALPHA(a,b)(RGB15(AVG_ALPHA(a,b), 0, 0))
//averages alpha instead of color
u16 BAG_Effects_PixSmoothingFont(const Smoothing_t *smooth){
  register int curColor[9];
  register int  x = smooth->x, y = smooth->y, wd = smooth->wd, ht = smooth->ht;

  //center pixel
  curColor[0] = smooth->buf[x + y * wd];
  //left pixel
  x--;
  x += (x < 0);
  curColor[1] = smooth->buf[x + y * wd];
  //right pixel
  x+=2;
  x -= (x > wd);
  curColor[2] = smooth->buf[x + y * wd];
  //reset x to main pix
  x--;
  //up pixel
  y--;
  y += (y < 0);
  curColor[3] = smooth->buf[x + y * wd];
  //down pixel
  y+=2;
  y -= (y > ht);
  curColor[4] = smooth->buf[x + y * wd];

  if(smooth->corners){
    //reset pos to center pixel
    y--;
    //left corners
    x--;
    x += (x < 0);

    //left top
    y--;
    y += (y < 0);
    curColor[5] = smooth->buf[x + y * wd];
    //left bottom
    y+=2;
    y -= (y > ht);
    curColor[6] = smooth->buf[x + y * wd];

    //right corners
    x+=2;
    x -= (x > wd);
    //right bottom
    curColor[7] = smooth->buf[x + y * wd];
    //right yop
    y-=2;
    y += (y < 0);
    curColor[8] = smooth->buf[x + y * wd];
    //8 pixel average(lol)
    return PIXAVG_ALPHA(curColor[0], PIXAVG_ALPHA(curColor[1], PIXAVG_ALPHA(curColor[2], PIXAVG_ALPHA(curColor[3],
                          //last normal pixel.......now the corner pixels
                          PIXAVG_ALPHA(curColor[4], PIXAVG_ALPHA(curColor[5], PIXAVG_ALPHA(curColor[6], PIXAVG_ALPHA(curColor[7], curColor[8]))))))));
  }
  //4 pixel average
  return PIXAVG_ALPHA(curColor[0], PIXAVG_ALPHA(curColor[1], PIXAVG_ALPHA(curColor[2], PIXAVG_ALPHA(curColor[3], curColor[4]))));
}


/*===========================================================================
Alpha blending
===========================================================================*/
inline u16 BAG_Effects_CalcAlpha(register int Color1, register int Color2, register int Alpha){ // transp in .8 fixed point
  if(Alpha <= 0)
    return Color2;
  else if(Alpha >= 255)
    return Color1;

	register u32 newAlpha = 255 - Alpha;
	//return RGB15(  (((Color1&31)*Alpha) + (((Color2&31)*newAlpha)))>>8,//red values
	//	       ((((Color1>>5)&31)*Alpha) + (((Color2>>5)&31)*newAlpha))>>8, //green values
	//	       ((((Color1>>10)&31)*Alpha) + (((Color2>>10)&31)*newAlpha))>>8 //blue values
	//	       );
	//return RGB15( (((Color1&31) * Alpha) + (((Color2&31) * newAlpha)))>>8, //red
	//	      ((((Color1>>6)&63)*Alpha) + (((Color2>>6)&63)*newAlpha))>>8, //green
	//	      ((((Color1>>11)&31)*Alpha) + (((Color2>>11)&31)*newAlpha))>>8 //blue
	//		      );
	return RGB15( ((((Color1>>11)&31)*Alpha) + (((Color2>>11)&31)*newAlpha))>>8, //red
		      ((((Color1>>5)&63)*Alpha) + (((Color2>>5)&63)*newAlpha))>>8, //green
		      (((Color1&31) * Alpha) + (((Color2&31) * newAlpha)))>>8 //blue
		      );
}

/*===========================================================================
Scaling
===========================================================================*/

inline int BAG_Effects_GetScaledMultiplier(int input, int target){
  return ((input<<8)/target);
}

inline int BAG_Effects_GetScaledResult(int currentValue, int multiplier){
  return (currentValue * multiplier) >> 8;
}

inline int BAG_Effects_GetMultiplerTimes(int input, int times){
  return ((input<<8) / (input* times));
}

inline int BAG_Effects_GetMultiplierDiv(int input, int times){
  return ((input<<8) / (input / times));
}

