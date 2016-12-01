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

#ifndef _BAG_Math_
#define _BAG_Math_

/*
	Various math functions... mostly ripped from PAlib
*/
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


extern const short BAG_MATH_SIN[512];

/** @defgroup Math Math
 *  Math related functions
 *  @{
 */

/*! \def BAG_Math_Cos(angle)
    \brief
        Returns the Cos value for an angle.

	\~\param angle
		An angle value between or equal to 0 and 511
*/
#define BAG_Math_Cos(angle) BAG_MATH_SIN[((angle) + 128)&511]


/*! \def BAG_Math_Sin(angle)
    \brief
          Returns the Sin value for an angle.

	\~\param angle
		An angle value between or equal to 0 and 511
*/
#define BAG_Math_Sin(angle) BAG_MATH_SIN[((angle))&511]





/*! \fn unsigned long long BAG_Math_GetDist(int x1, int y1, int x2, int y2)
    \brief
          Get the distance in pixels between two coordinates.

	\~\param x1
		Coordinate 1 X position
	\~\param y1
		Coordinate 1 Y position
	\~\param x2
		Coordinate 2 X position
	\~\param y2
		Coordinate 2 Y position

	\~\return
		thesquared distance between two points
*/
extern unsigned long long BAG_Math_GetDist(int x1, int y1, int x2, int y2);





/*! \fn unsigned short BAG_Math_AdjustAngle(unsigned short angle, short anglerot, int startx, int starty, int targetx, int targety)
    \brief
          Adjust an angle, for example to calculate in which direction an object should turn.

	\~\param angle
		Initial angle (0 - 511)
	\~\param anglerot
		How much to adjust the intial angle
	\~\param startx
		Initial X position
	\~\param starty
		Initial Y position
	\~\param targetx
		Target X position
	\~\param targety
		Target Y position

	\~\return
		A new calculated angle from the initial angle and the adjustment supplied
*/
extern unsigned short BAG_Math_AdjustAngle(unsigned short angle, short anglerot, int startx, int starty, int targetx, int targety);



/*! \fn unsigned short BAG_Math_GetAngle(int startx, int starty, int targetx, int targety)
    \brief
          Get an angle between two coordinates.

	\~\param startx
		Coordinate 1 X position
	\~\param starty
		Coordinate 1 Y position
	\~\param targetx
		Coordinate 2 X position
	\~\param targety
		Coordinate 2 Y position

	\~\return
		an angle value between or equal to 0 - 511.
*/
extern unsigned short BAG_Math_GetAngle(int startx, int starty, int targetx, int targety);


/*! \fn void BAG_Math_srand(unsigned int seed)
    \brief
          Set the seed for libBAG's built in random function.

	\~\param seed
		Seed value to use.

*/
extern void BAG_Math_srand(unsigned int seed);


/*! \fn unsigned int BAG_Math_rand(void)
    \brief
		Generates a random value.

	\~\return
      Returns a random value based on a seed set by
	  BAG_Math_srand().


*/
extern unsigned int BAG_Math_rand(void);

//function short forms
#ifdef BAG_SHORTFORM

	#define BAG_Sin 				BAG_Math_Sin
	#define BAG_Cos 				BAG_Math_Cos
	#define BAG_GetDist 			BAG_Math_GetDist
	#define BAG_AdjustAngle			BAG_Math_AdjustAngle
	#define BAG_GetAngle			BAG_Math_GetAngle

#endif
/** @} */ // end of Math


#ifdef __cplusplus
}
#endif

#endif
