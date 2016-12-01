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

#ifndef _BAG_FS_
#define _BAG_FS_

#include <ds2sdk.h>

#define MAX_NAME_LEN 256


#ifdef __cplusplus
extern "C" {
#endif

//simple operations
extern void rewind_ex( FILE * fp);
extern void rmdir_ex(char *path);

//functions that read data
extern s8 BAG_Filesystem_GetMagicNumbers(const char *path, u8 *dataBuf, s8 numBytes);
extern u8 *BAG_Filesystem_LoadFile(const char *file);
extern u8 *BAG_Filesystem_LoadFileWithSize(const char *file, u32 *size);

//simple checks
extern s8 BAG_Filesystem_TrailingSep(const char *path);
extern s8 BAG_Filesystem_isDir(const char *path);
extern u32 BAG_Filesystem_GetFileSize(const char *file);
extern s8 BAG_Filesystem_FileExists(const char *path);
extern s8 BAG_Filesystem_FindFileExt(const char *path, char *extList[], int extCount, char *newPath, int newPathLen);
extern unsigned char BAG_Filesystem_getDirLevels(char *path);
extern unsigned int BAG_Filesystem_ConvertSize(u64 size, u8 *unit);

//file path manipulation
extern char *BAG_Filesystem_BuildPath(int count, ...);
extern int BAG_Filesystem_SplitPathName(const char *fullPath, char *outDir, char *outFile);
extern void BAG_Filesystem_StripLeadingDirs(int levels, char *inPath);
extern long BAG_Filesystem_StripTrailingDirs(int levels, char *inPath);
extern unsigned long BAG_Filesystem_getFileExt(const char *filename, char *ext_buf, int bufsize);

//Directory/file creation
extern int BAG_Filesystem_mkdir(const char *path);
extern FILE *BAG_Filesystem_fopen(const char *path, const char *mode);



#ifdef __cplusplus
}
#endif


#endif

