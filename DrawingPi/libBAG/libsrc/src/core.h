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

#ifndef _BAG_CORE_
#define _BAG_CORE_

#if defined(___SYS_DS2_)
  #include <ds2io.h>
#else
     #include <ds2sdk.h>
#endif


#include <string.h>
#include <libBAG_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BAG_DEFAULT_FPS 60
#define BAG_MAX_FUNCTIONS 10

extern unsigned short *SCREEN_ADDR;
  extern unsigned int SCREEN_SIZE;


typedef void (*BAG_Core_UpdateFunction)(void);
extern BAG_Core_UpdateFunction BAG_AsynchroFunc[BAG_MAX_FUNCTIONS];
extern BAG_Core_UpdateFunction BAG_SynchroFunc[BAG_MAX_FUNCTIONS];

extern unsigned short *BAG_GetScreen(void);
extern void BAG_ClearScreen(unsigned short color);
/** @defgroup lib_Core Library Core
 *  Library initialization stuff
 *  @{
 */



/*! \fn s8 BAG_Core_Init(s8 console)
    \brief
        Initializes the DSTwo IO layer, libBAG and or the the console

	\~\param console
		1 -enables console, 0 - does not enable the console

	\~\return
		1 if everything successfully initiates, 0 - if there is an error
*/
extern s8 BAG_Core_Init(s8 console);



/*! \fn s8 BAG_Core_InitEx(SCREEN_ID console_screen, s8 console, s8 enable_update)
    \brief
        Initializes the DSTwo IO layer, libBAG with some tweaks and the console

    \~\param console_screen
        The screen to enable the console on

	\~\param console
		1 -enables console, 0 - does not enable the console

    \~\param enable_update
        1-enables the use of hardware timer 0 to limit the frame rate and the use of asynchronous functions, 0 disables frame limiting and asynchronous functions

	\~\return
		1 if everything successfully initiates, 0 - if there is an error
*/
extern s8 BAG_Core_InitEx(s8 console_screen, s8 console, s8 enable_update);


/*! \fn void BAG_Core_SetFPS(s8 fps)
    \brief
        limits loop cycles with BAG_Core_UpdateAll() to set fps

	\~\param fps
		How many frames per second to limit loop to. Default is 60.

*/
extern void BAG_Core_SetFPS(s8 fps);



/*! \fn void BAG_Core_SetAsynchroFunction (void(*BAG_Core_UpdateFunction)(void))
    \brief
        Makes specified function updated asynchronously to the default frame rate of a loop. Function is updated in BAG_Core_UpdateAll(). Max number of functions that can be added is defined as BAG_MAX_FUNCTIONS

	\~\param function
		Function to set to update asynchronously with program
*/
extern void BAG_Core_SetAsynchroFunction (void(*BAG_Core_UpdateFunction)(void));



/*! \fn void BAG_Core_RemoveAsynchroFunction (void(*BAG_Core_UpdateFunction)(void))
    \brief
        Removes a function from being updated asynchronously

	\~\param function
		Function to remove from list
*/
extern void BAG_Core_RemoveAsynchroFunction (void(*BAG_Core_UpdateFunction)(void));



/*! \fn void BAG_Core_SetSynchroFunction (void(*BAG_Core_UpdateFunction)(void))
    \brief
        Makes specified function updated synchronously with default frame rate of a loop. Function is updated in BAG_Core_UpdateAll(). Max number of functions that can be added is defined as BAG_MAX_FUNCTIONS

	\~\param function
		Function to set to update synchronously with program
*/
extern void BAG_Core_SetSynchroFunction (void(*BAG_Core_UpdateFunction)(void));



/*! \fn void BAG_Core_RemoveSynchroFunction (void(*BAG_Core_UpdateFunction)(void))
    \brief
        Removes a function from being updated synchronously

	\~\param function
		Function to remove from list
*/
extern void BAG_Core_RemoveSynchroFunction (void(*BAG_Core_UpdateFunction)(void));



/*! \fn s8 BAG_Core_UpdateAll(void)
    \brief
        Updates all synchronous and asynchronous functions. Stylus and Audio updating are both synchronous functions that are updated here.
*/
extern s8 BAG_Core_UpdateAll(void);


extern void BAG_CloseFrameBuffer(void);

extern void BAG_Exit(int status);

extern void BAG_LaunchProgram(const char *command);


extern void BAG_Qsort(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *));
extern int BAG_binSearch(const void *data,  size_t dataCount, size_t dataSize, void *searchVal, int(*compare)(const void *, const void*));

//function short forms
#ifdef BAG_SHORTFORM
	#define BAG_Init					    BAG_Core_Init
  #define BAG_InitEx            BAG_Core_InitEx
	#define BAG_SetFPS				    BAG_Core_SetFPS
	#define BAG_SetAsync			    BAG_Core_SetAsynchroFunction
	#define BAG_RmAsync			      BAG_Core_RemoveAsynchroFunction
	#define BAG_SetSync			      BAG_Core_SetSynchroFunction
	#define BAG_RmSync			      BAG_Core_RemoveSynchroFunction
	#define BAG_Update				    BAG_Core_UpdateAll
#endif

/** @} */ // end of Core

#ifdef __cplusplus
}
#endif

#endif
