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


/*
Stores a string in fixed width as specified by
the WSTRING_TYPE.

*/


#ifndef _BAG_wString_
#define _BAG_wString_


//#include <stdlib.h>
#include <ds2sdk.h>

#if defined(___SYS_DS2_)
  #define WSTRING_ALLOC
#endif

#define WSTRING_ASCII
#define WSTRING_TYPE unsigned long

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wString{
    #ifdef WSTRING_ALLOC
        WSTRING_TYPE *array;
        char *ascii;
    #else
        WSTRING_TYPE array[MAX_PATH<<1];
        #if defined(WSTRING_ASCII)
          char ascii[(MAX_PATH<<1) * sizeof(WSTRING_TYPE)];
        #endif
    #endif

    int len, oldLen, asciiLen, strtokStart, nextStrTok;

    WSTRING_TYPE *(*string)(struct wString *);
    int (*length)(struct wString *);

    int (*new)(struct wString *, int);
    void (*concat)(struct wString *, struct wString*);
    void (*charCat)(struct wString *, WSTRING_TYPE);
    void (*Ncopy)(struct wString *, struct wString *, int);
    void (*copy)(struct wString *, struct wString *);
    int (*Ncmp)(struct wString *, struct wString*, int);
    int (*cmp)(struct wString *, struct wString *);
    void (*del)(struct wString *);
    int (*getWord)(struct wString *, int, struct wString *);
    void (*toUpper)(struct wString *);
    void (*toLower)(struct wString *);
    int (*resize)(struct wString *, int);

    int (*atoiN)(struct wString *, int);
    int (*atoi)(struct wString *);

    #if defined(WSTRING_ASCII)
      char *(*toAscii)(struct wString *);
      void (*delAscii)(struct wString *);
    #endif

    void (*setLength)(struct wString *, int);
    void (*restoreLength)(struct wString *);

    void (*printf)(struct wString *, const char *fmt, ...);
    void (*vprintf)(struct wString *, const char *fmt, va_list *);
    int (*strtok)(struct wString *, const WSTRING_TYPE *, int);
    void (*resetTok)(struct wString *);


}wString __attribute__ ((aligned (4)));



extern void wString_new(wString *string);





#ifdef __cplusplus
}
#endif

#endif
