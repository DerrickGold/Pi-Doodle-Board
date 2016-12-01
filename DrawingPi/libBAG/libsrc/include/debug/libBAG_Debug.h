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

#ifndef _BAG_DEBUG_
#define _BAG_DEBUG_

#ifdef __cplusplus
extern "C" {
#endif


#include <ds2sdk.h>
#include <stdarg.h>

#define OPEN_FAIL_MSG "BAG:File not found:\n%s\n"
#define OPEN_FAIL_WRITE_MSG "BAG:File can't be created:\n%s\n"
#define OPEN_INVALID_MSG "BAG:Invalid file:\n%s\n"
#define BUFFER_FAILED_MSG "BAG:Failed to allocate %d\nbytes for [%s] buffer\n"

#define BAG_DBG_ASSERT(x) \
  BAG_DBG_Assert((int)x, "Assertion fail\n%s @ %d\n", __FILE__, __LINE__);

#define BAG_DBG_MSG(msg) \
  BAG_DBG_Msg("BAG MSG:\n%s @ %d\n%s\n", __FILE__, __LINE__, msg);

typedef enum{
	DBG_ENABLE = (1<<0),
	DBG_SILENT = (1<<1),//debug system will not write to screen, only file
	DBG_FAT = (1<<2), //enable writing to log file
	DBG_HALT = (1<<3), //enable halting of program on common message
  DBG_HALT_LIB = (1<<4),//halt program on library message
  DBG_LIB = (1<<5), //enable debug messages for libBAG functions
}BAG_DBG_FLAGS;

typedef enum{
    ERR_END = 0,
    ERR_NONE = 1,
    _ERR_LIST_END
}BAG_DBG_FUNCTIONS;

struct BAG_DBG_SYS{
	long Flags;
	FILE *File;
	char *File_Name;
};




/** @defgroup Debug Debug
 *  Debug features
 *  @{
 */



/*! \fn void BAG_DBG_Init(const char *Log_File, bool Fat, bool hide)
    \brief
		Initiates the debug system which will write debug messages to a specified log file

	\~\param Log_File
		File to writedebug information to. If Fat is 0, can be set to NULL.
	\~\param Fat
		1 - enables log file writing, 0 - disables writing to the log file
	\~\param hide
		1 - disables writing errors to the supercard console, 0 will show errors
*/
extern void BAG_DBG_Init(const char *Log_File, long flags);




/*! \fn void BAG_DBG_Msg(const char* Message)
    \brief
		Write a debug message.
*/
extern void BAG_DBG_Msg(const char* info, ...);

extern void BAG_DBG_LibMsg(const char *info, ...);

extern void BAG_DBG_Assert(int condition, const char *msg, ...);
/** @} */ // end of debug

#ifdef __cplusplus
}
#endif

#endif
