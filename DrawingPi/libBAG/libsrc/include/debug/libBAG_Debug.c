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

#include "libBAG_Debug.h"

static struct BAG_DBG_SYS _BAG_Debug;


void __dbg_msg_(const char *info, va_list argsptr) {
  
  size_t bufSize = strlen(info);
  char logLine[bufSize];


  
  vsnprintf(logLine, bufSize, info, argsptr);
  va_end(argsptr);



  //always print to screen unless specified
  if(!GET_FLAG(_BAG_Debug.Flags, DBG_SILENT)){
    printf("%s", logLine);
    //write to fat if enabled
    if(GET_FLAG(_BAG_Debug.Flags, DBG_FAT)){
      fprintf(_BAG_Debug.File, "%s", logLine);
      fflush(_BAG_Debug.File);
    }
  }
  if(GET_FLAG(_BAG_Debug.Flags, DBG_HALT)){
    printf("**Program has been stopped.\n Please reboot.**");
    while(1);
  }
}



void BAG_DBG_Init(const char *outFile, long flags){
    _BAG_Debug.Flags = flags;
    SET_FLAG(_BAG_Debug.Flags, DBG_ENABLE);

    //open and create log file if necessary
    if(outFile == NULL)
        return;

    //strncpy(_BAG_Debug.File_Name, outFile, 255);
      _BAG_Debug.File_Name = calloc(sizeof(char), strlen(outFile));
      if(!_BAG_Debug.File_Name){
        printf("Error initializing debug output file\n");
        _BAG_Debug.Flags = 0;
        return;
      }

      strncpy(_BAG_Debug.File_Name, outFile, strlen(outFile));
    _BAG_Debug.File = fopen(_BAG_Debug.File_Name,  "ab");
    //error opening log file
    if(_BAG_Debug.File == NULL)
        return;

    if(GET_FLAG(_BAG_Debug.Flags, DBG_FAT)){
      /*struct rtc Date;
        ds2_getTime(&Date);

        char logentry[256];
        sprintf(logentry, "[%d-%02d-%02d_%02d.%02d]",Date.year,Date.month,Date.day,Date.hours,Date.minutes);
        fprintf(_BAG_Debug.File, "%s\n", logentry);
        fflush(_BAG_Debug.File);
	//fclose(_BAG_Debug.File);*/
    }
}

void BAG_DBG_Msg(const char *info, ...){
  if(!info) return;

  //check if debug mode is enabled or not
  if(!GET_FLAG(_BAG_Debug.Flags, DBG_ENABLE))
    return;
  va_list argsptr;
  va_start(argsptr, info);
  __dbg_msg_(info, argsptr);
}



void BAG_DBG_Assert(int condition, const char *msg, ...){
  //if condition is true, then use
  if(condition || !msg)
    return;

  size_t bufSize = strlen(msg);
  char logLine[bufSize];

  va_list argsptr;
  va_start(argsptr, msg);
  //if(argsptr){
    vsnprintf(logLine, bufSize, msg, argsptr);
    va_end(argsptr);
    //}

  //otherwise, print error and exit on keypress
  printf("%s", logLine);

  /*struct key_buf rawin;
  ds2_getrawInput(&rawin);

  //wait for keys to be released
  while(rawin.key){
    mdelay(12);
    ds2_getrawInput(&rawin);
  }

  //then wait for new key presses
  while(!rawin.key){
    mdelay(12);
    ds2_getrawInput(&rawin);
  }

  ds2_plug_exit();*/
}


//these messages will only be displayed when the dbg library flag is set
void BAG_DBG_LibMsg(const char *info, ...){
  if(!info) return;

    if(!GET_FLAG(_BAG_Debug.Flags, DBG_LIB))
        return;

    va_list argsptr;
    va_start(argsptr, info);

    if(GET_FLAG(_BAG_Debug.Flags, DBG_HALT_LIB))
      SET_FLAG(_BAG_Debug.Flags, DBG_HALT);

    //BAG_DBG_Msg(info, argsptr);
    //va_end(argsptr);
    __dbg_msg_(info, argsptr);
}

