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

#include "filesystem.h"
#ifdef __APPLE__
#include <libBAG_Debug.h>
#else
#include <debug/libBAG_Debug.h>
#endif



/*============================================================
Basic Operations
============================================================*/
void strip_trailing_sep(char *path){
  //strip trailing characters from folder path
  while(BAG_Filesystem_TrailingSep(path)){
    int len = strlen(path);
    path[len]= '\0';
  }
}


void rewind_ex(FILE * fp){
	fseek(fp, 0L, SEEK_SET);
	fat_clearerr(fp);
}

void rmdir_ex(char *path){
    strip_trailing_sep(path);
    rmdir(path);
}



/*============================================================
Functions that read data
============================================================*/
s8 BAG_Filesystem_GetMagicNumbers(const char *path, u8 *dataBuf, s8 numBytes){
  FILE *temp = fopen(path, "rb");
  if(!temp){
    BAG_DBG_LibMsg(OPEN_FAIL_MSG, path);
    return ERR_END;
  }

  fread(dataBuf, 1, numBytes-1, temp);
  fclose(temp);
  return ERR_NONE;
}

u8 *BAG_Filesystem_LoadFileWithSize(const char *file, u32 *size){
    //get file size
    (*size) = BAG_Filesystem_GetFileSize(file);
    if(size == 0)
      return NULL;

    //open file
    FILE *temp = fopen(file, "rb");
    if(temp == NULL){
      BAG_DBG_LibMsg(OPEN_FAIL_MSG, file);
      return NULL;
    }

    //create buffer
    u8 *buffer = calloc(sizeof(u8), *size);
    if(buffer == NULL){
        BAG_DBG_LibMsg(BUFFER_FAILED_MSG, *size, "file");
        fclose(temp);
        return NULL;
    }

    fread(buffer, 1, *size, temp);
    fclose(temp);
    return buffer;
}

u8 *BAG_Filesystem_LoadFile(const char *file){
    u32 size = 0;
    return BAG_Filesystem_LoadFileWithSize(file, &size);
}


/*============================================================
Simple Checks
============================================================*/
s8 BAG_Filesystem_isDir(const char *path){
  struct stat st;
  if(!lstat(path, &st))
    return S_ISDIR(st.st_mode);
  return -1;
}

s8 BAG_Filesystem_TrailingSep(const char *path){
  int len = strlen(path);
  len -= (path[len] == '\0');
  return (path[len] == DIR_SEPARATOR && len > 1);
}

u32 BAG_Filesystem_GetFileSize(const char *file){
  if(file == NULL){
    BAG_DBG_LibMsg(OPEN_FAIL_MSG, file);
    return 0;
  }

  struct stat st;
  memset(&st, 0, sizeof(struct stat));
  lstat(file, &st);
  return st.st_size;
}

s8 BAG_Filesystem_FileExists(const char *path){
  struct stat st;
  return (lstat(path, &st) < 0) ? 0 : 1;
}

//searches to see if a file exists by checking the name with multiple extensions
static inline s8 _checkFileExt(const char *path, const char *ext, char *newPath, int newPathLen){
  strncpy(newPath, path, newPathLen);
  strncat(newPath, ext, newPathLen - strlen(newPath));
  return BAG_Filesystem_FileExists(newPath);
}

s8 BAG_Filesystem_FindFileExt(const char *path, char *extList[], int extCount, char *newPath, int newPathLen){
    for(int i = 0; i < extCount; i++){
        if(_checkFileExt(path, extList[i], newPath, newPathLen) == 1)
            return i;
    }
    BAG_DBG_LibMsg("BAG:Failed to find file with extensions\n");
    return -1;
}

//count how many levels a path leads through
unsigned char BAG_Filesystem_getDirLevels(char *path){
  unsigned char count = 0;
  for(int i = 0; i < strlen(path); i++)
    count += (path[i] == DIR_SEPARATOR);

  return count;
}

unsigned int BAG_Filesystem_ConvertSize(u64 size, u8 *unit){
  if(size > SIZE_GB){
    (*unit) = 3;
    return size / SIZE_GB;
  }
  if (size > SIZE_MB){
    (*unit) = 2;
    return size / SIZE_MB;
  }
  else if (size > SIZE_KB){
    (*unit) = 1;
    return size / SIZE_KB;
  }

  (*unit) = 0;
  return size;
}

/*============================================================
File path manipulations
============================================================*/
static char _bag_folderBuf[MAX_PATH];

static void _appendSeparator(void){
    char dirSeparator[2] = {DIR_SEPARATOR, '\0'};
    if(!BAG_Filesystem_TrailingSep(_bag_folderBuf))
      strcat(_bag_folderBuf, dirSeparator);
}


//Builds a file path with arguments provided
char *BAG_Filesystem_BuildPath(int count, ...){
    memset(_bag_folderBuf, 0, sizeof(_bag_folderBuf));
    va_list argsptr;
    va_start(argsptr, count);
    for(int i = 0; i < count - 1; i++){
        char *temp = va_arg(argsptr, char*);
        if(temp == NULL || temp[0] == '\0')
          continue;

        //remove any leading separators from path
        while(*temp == DIR_SEPARATOR)
          temp++;

        if(strlen(_bag_folderBuf) + strlen(temp) + 1 < MAX_PATH){
            _appendSeparator();
            strcat(_bag_folderBuf, temp);
        }
    }
    char *final = va_arg(argsptr, char*);
    while(*final == DIR_SEPARATOR)
      final++;

    if(strlen(_bag_folderBuf) + strlen(final) + 1 < MAX_PATH){
      _appendSeparator();
      strcat(_bag_folderBuf, final);
    }

    va_end(argsptr);
    return (char*)&_bag_folderBuf;
}

//separate directory/file name from a full file path
int BAG_Filesystem_SplitPathName(const char *fullPath, char *outDir, char *outFile){
  char tempPath[MAX_PATH];
  strcpy(tempPath, fullPath);
  long length = BAG_Filesystem_StripTrailingDirs(1, tempPath);
  //BAG_Filesystem_dirBack(tempPath);
    if(outFile){
      while(fullPath[length] == DIR_SEPARATOR)
        length++;

      strcpy(outFile, &fullPath[length]);
      strcat(outFile, "\0");
    }
    if(outDir){
      while(BAG_Filesystem_TrailingSep(tempPath))
        BAG_Filesystem_StripTrailingDirs(1, tempPath);

      strcpy(outDir, tempPath);
      strcat(outDir, "\0");
    }
  return 1;
}

//removes pre-appended levels of directory from a path
void BAG_Filesystem_StripLeadingDirs(int levels, char *inPath){
  char * outPtr = inPath,
       *start = NULL,
       *end = &inPath[strlen(inPath)];

  while(levels > 0 && start <= end && inPath <= end){
    if(*inPath == DIR_SEPARATOR){
      levels--;
      start = inPath;
    }
    inPath++;
  }

  while(*start == DIR_SEPARATOR)
    start++;

  if(start && start <= end)
    strcpy(outPtr, start);
}


//removes trailing directories from a path
static long _BAG_Filesystem_dirBack(char *origPath){
  long len = strlen(origPath);
  len -= (origPath[len] == '\0');
  while(origPath[len] != DIR_SEPARATOR && len > 0){
    origPath[len] = '\0';len--;
  }

  //remove the separator now
  if(origPath[len] == DIR_SEPARATOR){
    origPath[len--] = '\0';
  }

  //hit root directory
  if(len <= 0){
    len = 0;
    origPath[0] = DIR_SEPARATOR;
    origPath[1] = '\0';
  }
  return len + 1;
}

long BAG_Filesystem_StripTrailingDirs(int levels, char *inPath){
  int len = 0;
  while(levels-- > 0 && strlen(inPath))
    len += _BAG_Filesystem_dirBack(inPath);

  return len;
}

//get a file extension from a name to a buffer
static char getSmallChar(char letter){
  if((letter >= 'A')&&(letter <= 'Z'))
    return (letter-'A'+'a');

   return letter;
}

unsigned long BAG_Filesystem_getFileExt(const char *filename, char *ext_buf, int bufsize){
  // go to end of name
  size_t len = strlen(filename);
  int i = len;
  while(filename[i] != '.' && i > 0)
    i--;
  if(i <= 0)
    return -2;

  int j = i + 1;
  i = j;
  do {
        if( (i - j) >= bufsize)
      return -2;

        ext_buf[i-j] = getSmallChar(filename[i]);
  }while( i++ < len);
  ext_buf[i - j] = '\0';
  return j - 1;//the . for the extension
}



/*============================================================
File/Directory Creation
============================================================*/
//makes all the directories in a path
int BAG_Filesystem_mkdir(const char *path){
    char directoryPath[MAX_PATH], pathBuf[MAX_PATH];
    strcpy(directoryPath, path);
    strip_trailing_sep(directoryPath);

    strcpy(pathBuf, directoryPath);

    //check if directory exists
    //char key[2] = {DIR_SEPARATOR, '\0'};
    char made[MAX_PATH];
    memset(made, 0, MAX_PATH);

    char *curPath = strtok (directoryPath, DIR_SEPARATOR_STR);
    strcat(made, DIR_SEPARATOR_STR);
    while(!BAG_Filesystem_FileExists(pathBuf) && curPath){
        strcat(made, curPath);
        //check if the current path exists, if not make new dir
        if(!BAG_Filesystem_FileExists(made) && fat_mkdir(made, 777) == -1)
            return 0;

        strcat(made, DIR_SEPARATOR_STR);
        curPath = strtok (NULL, DIR_SEPARATOR_STR);
    }
    return 1;
}


//opens a file, if in write mode, creates the path necessary
FILE *BAG_Filesystem_fopen(const char *path, const char *mode){
    //write mode
    if(mode[0] == 'w' || mode[1] == 'w'){
        char targetFolder[MAX_PATH];
        memset(targetFolder, 0, MAX_PATH);
        BAG_Filesystem_SplitPathName(path, targetFolder, NULL);
        //make path if it doesn't exist
        if(!BAG_Filesystem_mkdir(targetFolder))
            return NULL;//if failed to open path
    }

    return fat_fopen(path, mode);
}

