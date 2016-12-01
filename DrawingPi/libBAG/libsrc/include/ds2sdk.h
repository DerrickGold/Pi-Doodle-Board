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


#ifndef _DS2_SDK_
#define _DS2_SDK_

//SDK required files
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <libBAG_types.h>



#define MAX_PATH PATH_MAX

#if defined(USE_SDL)
  #include <sdlbridge.h>

#elif defined(___SYS_DS2_)

  #include <ds2io.h>
  #include <ds2_cpu.h>
  #include <ds2_malloc.h>
  #include <fs_api.h>
  #include <fs_unicode.h>
  #include <ds2_timer.h>
  #include <console.h>
  #include <ds2_dma.h>
  #include <ds2_cpuclock.h>

  #define DIR_ROOT "/"
  #define DIR_SEPARATOR '/'
  #define DIR_SEPARATOR_STR "/"
#endif

#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <fcntl.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>


#define fat_fopen fopen
#define fat_mkdir mkdir
#define fat_clearerr clearerr

#define DIR_ROOT "/"
#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_STR "/"


#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

#define BIT(a) (1<<a)
//#define RGB15(r,g,b) (((r)|((g)<<5)|((b)<<10))|BIT(15))
#define RGB565(r,g,b) (((b)|((g)<<5)|((r)<<11)))
#define RGB15(r,g,b) RGB565(r,g,b)

#ifndef MAX
	#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
	#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef VAL
	#define VAL(func) ((*func))
#endif


//unit of time
#define SYSTIME_UNIT    42667

//My library stuff
#define SET_FLAG(Var, Flag) ((Var)|=(Flag))
#define RESET_FLAG(Var, Flag) ((Var) &= ~(Flag))
#define CLEAR_FLAGS(Var,Val) ((Var)=(Val))
#define GET_FLAG(Var, Flag) (((Var) & (Flag))!=0)

#define ALIGN_MEM(ptr) \
    while(ptr&3)ptr++;



extern void *bag_malloc(unsigned long size);
extern void *bag_calloc(unsigned long count, unsigned long size);
extern void *bag_realloc(void *ptr, unsigned long size);

typedef enum{
  SIZE_KB = 1024,
  SIZE_MB = 1048576,
  SIZE_GB = 1073741824
}SIZE_DATA_UNITS;

/*
#ifdef malloc
    #undef malloc
#endif

#define malloc bag_malloc

#ifdef calloc
    #undef calloc
#endif

#define calloc bag_calloc

#ifdef realloc
    #undef realloc
#endif

#define realloc bag_realloc
*/



#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif

