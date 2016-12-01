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

#ifndef _BAG_INPUT_
#define _BAG_INPUT_


#ifdef __cplusplus
extern "C" {
#endif


#include "ds2sdk.h"

#define BAG_DEFAULT_DBLCLICK 45


typedef struct {
	unsigned char Held, Released, Newpress, DblClick;
	int X, Y, Downtime, Uptime, ClickTime;
} _BAG_Stylus;
extern _BAG_Stylus Stylus;



/** @defgroup Input Input
 *  Everything Input
 *  @{
 */


/*! \fn void BAG_Input_UpdateIN(void)
    \brief
		Updates Stylus and Pad input
*/
extern void BAG_Input_UpdateIN(void);


/*! \fn void BAG_Input_ResetStylusDblClick(void)
    \brief
		Reset the double click status of the stylus
*/
extern void BAG_Input_ResetStylusDblClick(void);



/*! \fn void BAG_Input_SetDblClickTime(int time)
    \brief
		Set the time required for two Stylus newpresses to be considered a double click. Default is defined as BAG_DEFAULT_DBLCLICK
*/
extern void BAG_Input_SetDblClickTime(int time);



/*! \def BAG_Input_StylusZone(x1, y1, x2, y2)
    \brief
		 Checks if stylus is in area defined by x1, x2, y1,y2. Does not check for a new press or hold.
*/
#define BAG_Input_StylusZone(x1, y1, x2, y2) ((Stylus.X>=x1)&&(Stylus.Y>=y1)&&(Stylus.X<x2)&&(Stylus.Y<y2))

extern int BAG_Input_Init(Display *display, Window *root);
extern void BAG_Input_Clean(void);


//function short forms
#ifdef BAG_SHORTFORM

	#define BAG_UpdateIN				BAG_Input_UpdateIN
	#define BAG_ResetDblClick			BAG_Input_ResetStylusDblClick
	#define BAG_SetClickTime			BAG_Input_SetDblClickTime
	#define BAG_StylusZone				BAG_Input_StylusZone

#endif

/** @} */ // end of Input

#ifdef __cplusplus
}
#endif

#endif
