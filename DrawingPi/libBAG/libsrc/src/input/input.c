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

#include "input.h"

_BAG_Stylus Stylus;
static unsigned int _completeStylus, _exStylus;
static Display *__display = NULL;
static Window *__root = NULL;

//Button1Mask refers to left clicks which the touchscreen defaults to detecting as a touch
#define UPDATE_STYLUS(type, var) do{\
  type=(var & Button1Mask)>>8; \
}while(0)		       \




void BAG_Input_UpdateIN(void) {
  Window returned;
  int sx, sy, x, y;

  _exStylus = _completeStylus;
  
  XQueryPointer(__display, *__root, &returned, &returned, &sx, &sy, &x, &y, &_completeStylus);
  Stylus.X = sx;
  Stylus.Y = sy;

  
  UPDATE_STYLUS(Stylus.Released, (_exStylus & (~_completeStylus)));
  UPDATE_STYLUS(Stylus.Newpress, (_completeStylus & (~_exStylus)));

  Stylus.Held += (Stylus.Newpress - Stylus.Released);

  
  Stylus.Downtime *= (!Stylus.Newpress);
  Stylus.Downtime += Stylus.Held;
  Stylus.Uptime *= (!Stylus.Released);
  Stylus.Uptime += !Stylus.Held;
}

int BAG_Input_Init(Display *display, Window *root) {
  if (!display || !root) return 0;
  //store addresses here for reference so we don't need to keep passing
  //these variables into every function call for input
  __display = display;
  __root = root;

  
  XGrabPointer(__display, *__root, False, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
   	       GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

  XSelectInput(__display, *__root, ButtonPressMask | ButtonMotionMask | ButtonReleaseMask);

  return 1;
}

void BAG_Input_Clean(void) {
  __root = NULL;
 
  if(!__display) return;
  XUngrabPointer(__display, CurrentTime);
  
  __display = NULL;
}


