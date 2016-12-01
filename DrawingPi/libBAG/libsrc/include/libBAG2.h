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

#ifndef _BAG_LIB2_
#define _BAG_LIB2_

#ifdef __cplusplus
extern "C" {
#endif

#include "strings/wString.h"
#include "../src/core.h"
#include "../src/input/input.h"
#include "debug/libBAG_Debug.h"
#include "../src/BAG_Math.h"

#if defined(___SYS_DS2_)
  #include "../src/Display/sdk.h"
#endif


#include "../src/Filesystem/filesystem.h"
  //#include "../src/Filesystem/filepath.h"

#if defined(___SYS_DS2_)
  #include "../src/Filesystem/directory_walk.h"
#endif


  //#include "../src/Filesystem/fastIni.h"
#include "../src/Display/Draw.h"
#include "../src/Display/Display.h"
#include "../src/Display/TileBG.h"

#if defined(BMP_MODULE)
  #include "../src/Display/Bmp/bmp.h"
#endif

#if defined(PNG_MODULE)
  #include "../src/Display/Png/png.h"
#endif

#if defined(JPG_MODULE)
  #include "../src/Display/Jpeg/jpeg.h"
#endif


#include "../src/Display/text.h"

#if defined(COMPILE_AUDIO)
  #include "../src/Audio/Audio_Core.h"
#endif


#ifdef fopen
  #undef fopen
  #define fopen BAG_Filesystem_fopen
#endif




#ifdef __cplusplus
}
#endif

#endif

