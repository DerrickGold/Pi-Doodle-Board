#include "Draw.h"
#include <stdlib.h>

inline void BAG_Draw_BlitPixel(u16 *Dest, int destWd, int destHt, int x, int y, u16 color){
	if(x >= 0 && x < destWd && y >= 0 && y < destHt)
		Dest[x+(y * destWd)] = color;
}

inline u16 BAG_Draw_GetPixel(u16 *Dest, int destWd, int destHt, int x, int y){
	u16 color = 0;

	if(x >= 0 && x < destWd && y >= 0 && y < destHt)
		color = Dest[x+(y * destWd)];

	return color;
}


void BAG_Draw_BlitLine(u16 *Dest, int destWd, int destHt, int x, int y, int x2, int y2, u16 color){
   	s8 yLonger = FALSE;
	int shortLen= y2 - y;
	int longLen= x2 - x;
	int j = 0;
	if (abs(shortLen)>abs(longLen)) {
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;
		yLonger = TRUE;
	}
	int decInc = 0;

	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0){
			longLen+=y;
			for (j=0x8000+(x<<16);y<=longLen;++y) {
				BAG_Draw_BlitPixel(Dest, destWd, destHt, j>>16, y, color);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for ( j=0x8000+(x<<16);y>=longLen;--y) {
			BAG_Draw_BlitPixel(Dest, destWd, destHt, j>>16, y, color);
			j-=decInc;
		}
		return;
	}

	if (longLen>0){
		longLen+=x;
		for (j=0x8000+(y<<16);x<=longLen;++x) {
			BAG_Draw_BlitPixel(Dest, destWd, destHt, x, j>>16, color);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for ( j=0x8000+(y<<16);x>=longLen;--x) {
		BAG_Draw_BlitPixel(Dest, destWd, destHt, x, j>>16, color);
		j-=decInc;
	}
}


void BAG_Draw_BlitLineEx(u16 *Dest, int destWd, int destHt, s16 basex, s16 basey, s16 endx, s16 endy, u16 color, u8 size) {
	if(basex < 0) basex = 0;
	if(basey<0) basey = 0;

	int low = (size >> 1) - size + 1;
	int high = (size >> 1) + 1;
	int i = 0, j = 0;
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	for (i = low; i < high; i++) {
		for (j = low; j < high; j++) {
			if ((basex + i >= 0) && (basey + j >= 0) && (basex + i < endx) && (basey + j < endy))
				BAG_Draw_BlitPixel(Dest, destWd, destHt, basex + i, basey + j, color);
		}
	}

	for (i = low; i < high; i++) {
		j = low;
		x1 = basex + i; x2 = endx + i; y1 = basey + j; y2 = endy + j;
		BAG_Draw_BlitLine(Dest, destWd, destHt, x1, y1, x2,  y2, color);

		j = high - 1;
		x1 = basex + i; x2 = endx + i; y1 = basey + j; y2 = endy + j;
		BAG_Draw_BlitLine(Dest, destWd, destHt, x1, y1, x2,  y2, color);
	}

	for (j = low; j < high; j++) {
		i = low;
		x1 = basex + i; x2 = endx + i; y1 = basey + j; y2 = endy + j;
		BAG_Draw_BlitLine(Dest, destWd, destHt, x1, y1, x2,  y2, color);
		i = high - 1;
		x1 = basex + i; x2 = endx + i; y1 = basey + j; y2 = endy + j;
		BAG_Draw_BlitLine(Dest, destWd, destHt, x1, y1, x2,  y2, color);
	}
}

void BAG_Draw_Rect(unsigned short *dest, int destWd, int destHt, int x1, int y1, int x2, int y2, unsigned short color){
    x1 = (x1 < 0) ? 0 : x1;
    y1 = (y1 < 0) ? 0 : y1;
	for(int curX = x1; curX < x2 && curX < destWd; curX++){
		for(int curY = y1; curY < y2 && curY < destHt; curY++){
			dest[curX + (curY * destWd)] = color;
		}
	}
}

