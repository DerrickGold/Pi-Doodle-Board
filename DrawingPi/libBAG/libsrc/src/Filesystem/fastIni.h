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

#ifndef _fastIni_
#define _fastIni_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "libBAG_types.h"
#if defined(___SYS_DS2_)
	#include <ds2_malloc.h>
	#include <fs_api.h>
#endif

#define MAX_LINE_SIZE 1024
#define INI_COMMENT '#'

//#define DBG_TXT
/*
strings for C
*/
struct cstring
{
	char *string;
	u16 len;
} __attribute__ ((aligned (4)));

extern void cStringClear(struct cstring *src);
extern s8 cStringSet(struct cstring *dest, const char *source, int len);

/*
Ini stuff
*/
typedef enum{
	INI_NONENTRYS = (1 << 0),//read non-entry lines as well
	INI_SORT = (1 << 1),//sort entries and sections
	INI_ADDMISSING = (1 << 2),//adds missing entries to the ini file in ram
	INI_NOSPACES = (1 << 3),//removes spaces before and after = when writing ini file
}_iniModes;


struct _iniEntry{
	struct cstring Name;
	struct cstring Value;
} __attribute__ ((aligned (4)));

struct _iniSection{
	struct cstring Header;
	struct _iniEntry *Entry;
	int entryCount;//number of entries
} __attribute__ ((aligned (4)));

typedef struct _iniFile{
	struct _iniSection *Sections;
	int blocks;
	u8 mode, error;

	void  (*close)      (struct _iniFile *);
	int   (*load)       (struct _iniFile *, char *, u8);
	char    (*write)		(struct _iniFile *, const char *);
	int   (*getInt)     (struct _iniFile *, const char *, const char *, int);
	void  (*setInt)     (struct _iniFile *, const char *, const char *, int);
	void* (*getStr)     (struct _iniFile *, const char *, const char *, const char *);
	char* (*setStr)     (struct _iniFile *, const char *, const char *, const char *);
	s8    (*addEntry)	(struct _iniFile *, const char *, const char *);
	s8    (*newFile)	(struct _iniFile *, const char *);
	char* (*entryString)(struct _iniEntry *);
	char* (*entryName)	(struct _iniEntry *);
	struct _iniEntry *(*getEntry)	(const struct _iniFile *, int, int);

}INIObj_t __attribute__ ((aligned (4)));


extern void _ini_clean(INIObj_t *File);
extern int _ini_loadFile(INIObj_t *iniFile, const char *filePath, u8 readFlags);
extern s8 _ini_newFile(INIObj_t *iniFile, const char *filePath);
extern s8 _ini_addEntry(INIObj_t *File, const char *header, const char *input);
extern char _ini_writeAll(INIObj_t *ini, const char *fileName);
extern int _ini_getInt(INIObj_t *File, const char *header, const char *key, int def_value);
extern char *_ini_getStr_char(INIObj_t *File, const char *header, const char *key, const char * def_value);
extern char *_ini_getStr(INIObj_t *File, const char *header, const char *key, const char *def_value);

extern void initFastIni(INIObj_t *ini);


#ifdef DBG_TXT
extern void _ini_printAll(const INIObj_t *File);
#endif

#ifdef __cplusplus
}
#endif

#endif
