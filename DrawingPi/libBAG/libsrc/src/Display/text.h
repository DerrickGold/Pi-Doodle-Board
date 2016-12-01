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

#ifndef _BAG_FNT_
#define _BAG_FNT_

//#define dbgOut

#define NO_SETTING (-99987988)
#define ASCII_START 'A'
#define ASCII_END 'z'

#ifdef __APPLE__
#include <wString.h>
#else
#include <strings/wString.h>
#endif

#include "Display.h"

#ifdef __cplusplus
extern "C" {
#endif

//textbox whole string
#define TBOX_WHOLESTR -1

typedef enum{
  WRAP_NONE,
  WRAP_LETTER,
  WRAP_WORD,
}FNT_WRAP_MODE;

typedef enum{
  ALIGN_LEFT,
  ALIGN_RIGHT,
  ALIGN_CENTERX,
  ALIGN_CENTERY,
  ALIGN_CENTER,
}FNT_ALIGN_MODE;



typedef struct fntHeader{
  char *faceName;
  int size;
  short lineHeight, base;
  int pages;
  char **pageID;
  int charCount;
  //percent value
  int scale;
  //a flag to mark the beginning of ascii characters to cut down on searches
  int asciiPos;
}fntHeader __attribute__ ((aligned (4)));


typedef struct fntChar{
  int number;
  //x y position in bitmap
  int x, y;
  //width and height of character
  int width, height;
  //offset to draw character in relation to its size
  int xoffset, yoffset;
  //advance along the line
  int xadvance;
  //what page the character is on
  int page;
  //channel?
  int chnl;
}fntChar __attribute__ ((aligned (4)));

typedef struct FNTObj_s{
  fntHeader Header;
  fntChar *Character;
  //number of images == number of pages
  GFXObj_t *image;
}FNTObj_t __attribute__ ((aligned (4)));


typedef struct TextBox_s{
  int x1, y1, x2, y2;
  int alignment, wrapMode;
  int charCount;

  unsigned short *dest;
  int destWd, destHt;

  int printX, printY;

  int numLines, curLine, maxLines;
  //unsigned short linLen[32];
}TextBox_t;



extern void BAG_Font_Unload(FNTObj_t *font);
extern int BAG_Font_Load(const char *filePath, FNTObj_t *font);
extern void BAG_Font_ScaleFont(FNTObj_t *font, int percent);
extern int BAG_Font_GetStrSize(FNTObj_t *font, wString *string);
extern int BAG_Font_GetCharWd(FNTObj_t *font, int character);
extern int BAG_Font_GetCharHt(FNTObj_t *font);
extern void BAG_Font_PrintCharInfo(int character, FNTObj_t *font);
extern int BAG_Font_PrintChar(FNTObj_t *font, int character, int x, int y, unsigned short *dest, int destWd, int destHt);
extern void BAG_Font_SetFontColor(FNTObj_t *font, unsigned short color);
extern void BAG_Font_SetSmoothing(FNTObj_t *font, int smoothing);
extern void BAG_Font_PrintEx(TextBox_t *tbox, wString *string, FNTObj_t *font);
extern void BAG_Font_Print(u16 *screen, TextBox_t *tbox, wString *string, FNTObj_t *font);
extern void BAG_Font_Print2(unsigned short *dest, TextBox_t *tbox, wString *string, FNTObj_t *font);

extern void BAG_Font_TextBoxSetCharLim(TextBox_t *box, int charLim);
extern void BAG_Font_TextBoxSetMaxLines(TextBox_t *box, int lines);
extern void BAG_Font_TextBoxNewLine(TextBox_t *box, FNTObj_t *font);
extern void BAG_Font_TextBoxSetLimits(TextBox_t *box, int x1, int y1, int x2, int y2);
extern void BAG_Font_TextBoxResetPrintX(TextBox_t *box);
extern void BAG_Font_TextBoxResetPrintY(TextBox_t *box);
extern void BAG_Font_TextBoxResetPrintXY(TextBox_t *box);
extern void BAG_Font_TextBoxSetOutEx(TextBox_t *box, unsigned short *dest, int destWd, int destHt);
extern void BAG_Font_TextBoxSetOut(TextBox_t *box, GFXObj_t *object);
extern void BAG_Font_TextBoxAlign(TextBox_t *box, int alignment);
extern void BAG_Font_TextBoxWrap(TextBox_t *box, int wrap);
extern void BAG_FontTextBoxMove(TextBox_t *box, int newX, int newY);


#ifdef __cplusplus
}
#endif

#endif

