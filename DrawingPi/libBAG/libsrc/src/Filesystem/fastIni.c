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

#include "fastIni.h"

//stores the default return string if value could not be found
static char __defStr[512];

static int _str_removeTrailing(char *buffer, int len){
	while(len > 0 && (buffer[len] == '\0' || buffer[len] == '\n' || buffer[len] == '\r' || buffer[len] == ' '))
		buffer[len--] = '\0';

	return len + 1;
}


/*========================================================================
Custom string handling
========================================================================*/
void cStringClear(struct cstring *src){
	if(src->string && src->len > 0)
		free(src->string);

	src->string = NULL;
	src->len = 0;
}

s8 cStringSet(struct cstring *dest, const char *source, int len){
	cStringClear(dest);
	dest->string = calloc(1, (len + 1) * sizeof(char));
	if(dest->string == NULL) return -1;
	dest->len = (len + 1);
	strncpy(dest->string, source, len);
	return 1;
}

/*s8 cStringAppend(struct cstring *dest, const char *source, int appendLen){
    //if there is no original string, then create a new one
    if(dest->string == NULL || dest->len == 0)
        return cStringSet(dest, source, appendLen);
    //otherwise...
    //resize the string to include the new append length
    char *temp = NULL;
    temp = realloc(dest->string, sizeof(char) * (dest->len + appendLen + 1));
    if(temp == NULL){
    	if(dest->string && dest->len) free(dest->string);
    	dest->string = NULL;
    	dest->len = 0;
        return -1;
    }
    dest->string = temp;
    temp = NULL;
    strncpy(&dest->string[dest->len + 1], source, appendLen);
    dest->len += appendLen;
    return 1;
}*/

/*========================================================================
In ram INI functions
========================================================================*/
/*
Entries are the value definitions ie. Screen Width = 256
*/
static void _ini_cleanEntryName(struct _iniEntry *entry){
	cStringClear(&entry->Name);
}

static void _ini_cleanEntryValue(struct _iniEntry *entry){
	cStringClear(&entry->Value);
}

static void _ini_cleanEntry(struct _iniEntry *entry){
	_ini_cleanEntryName(entry);
	_ini_cleanEntryValue(entry);
}

static void _ini_setEntryName(struct _iniEntry *entry, char *Name, int len){
	cStringSet(&entry->Name, Name, len);
}

static void _ini_setEntryValue(struct _iniEntry *entry, char *Value, int len, u8 mode){
	cStringSet(&entry->Value, Value, len);
}

static char *_ini_getEntryName(struct _iniEntry *entry){
	if(entry->Name.len == 0)
		return NULL;
	return entry->Name.string;
}

static char *_ini_getEntryString(struct _iniEntry *entry){
	if(entry->Value.len == 0)
		return NULL;
	return entry->Value.string;
}

static int _ini_getEntryInt(struct _iniEntry *entry){
	return atoi(_ini_getEntryString(entry));
}

/*
Each section is defined as a header ([Your Header]), and in each section are various entries
*/
static void _ini_cleanHeader(struct _iniSection *section){
	cStringClear(&section->Header);
}


static void _ini_cleanSection(struct _iniSection *section){
	_ini_cleanHeader(section);
	if(section->Entry == NULL || section->entryCount == 0)//check if there are any entries allocated or not
		return;

	int i = 0;
	for(; i < section->entryCount; i++)
		_ini_cleanEntry(&section->Entry[i]);

	section->entryCount = 0;
	free(section->Entry);
	section->Entry = NULL;
}

static void _ini_setHeader(struct _iniSection *section, char *header, int len){
	cStringSet(&section->Header, header, len);
}

static char *_ini_getHeader(struct _iniSection *section){
	return section->Header.string;
}

/*
An ini file is basically a bunch of sections within one file
*/

void _ini_clean(INIObj_t *File){
	if(File->Sections == NULL || File->blocks == 0)
		return;

	int i = 0;
	for(; i < File->blocks; i++)
		_ini_cleanSection(&File->Sections[i]);

	free(File->Sections);
	File->Sections = NULL;
}

static s8 _ini_allocSection(INIObj_t *File, int sectionNum){
	File->blocks = sectionNum + 1;
    struct _iniSection *tempSection = NULL;

    tempSection = realloc(File->Sections, File->blocks * sizeof(struct _iniSection));
	if(tempSection == NULL)
		return -1;
    File->Sections = tempSection;
	//memset(&File->Sections[sectionNum], 0, sizeof(struct _iniSection));
    File->Sections[sectionNum].Entry = NULL;
    File->Sections[sectionNum].Header.string = NULL;
    File->Sections[sectionNum].Header.len = 0;
    File->Sections[sectionNum].entryCount = 0;
	#ifdef DBG_TXT
	printf("_new section_\n");
	#endif
	return 1;
}

static s8 _ini_allocEntry(INIObj_t *File, int sectionNum, int entryNum){
	File->Sections[sectionNum].entryCount = entryNum + 1;
    struct _iniEntry *tempEntry = NULL;
    tempEntry = realloc(File->Sections[sectionNum].Entry, File->Sections[sectionNum].entryCount * sizeof(struct _iniEntry));
	if(tempEntry == NULL)
		return -1;
    File->Sections[sectionNum].Entry = tempEntry;
    File->Sections[sectionNum].Entry[entryNum].Name.string = NULL;
    File->Sections[sectionNum].Entry[entryNum].Name.len = 0;
    File->Sections[sectionNum].Entry[entryNum].Value.string = NULL;
    File->Sections[sectionNum].Entry[entryNum].Value.len = 0;
	//memset(&File->Sections[sectionNum].Entry[entryNum], 0, sizeof(struct _iniEntry));
	#ifdef DBG_TXT
	printf("_new entry_\n");
	#endif
	return 1;
}


/*
Load Ini file
*/

//checks if a buffer contains a header
// eg. [System]
static s8 _ini_checkHeader(INIObj_t *File, char *buffer, int curSection){
	if(buffer[0] != '[')
		return 0;

	int i = 0, len = strlen(buffer);
	while(i < len && buffer[i++] != ']');//find the end bracket

	if(_ini_allocSection(File, curSection) == -1)
		return -1;

	_ini_setHeader(&File->Sections[curSection], &buffer[1], i - 2);
	#ifdef DBG_TXT
	printf("[%s]\n", _ini_getHeader(&File->Sections[curSection]));
	#endif
	return 1;
}

/*
Check for a valid entry and its value
*/
static s8 _ini_checkEntry(INIObj_t *File, char *buffer, int sectionNum, int curEntry){
    #ifdef DBG_TXT
        printf("%s\n", buffer);
    #endif

	//check for the equal sign for a valid entry
	char *test = strchr (buffer, '=');
	if(!test){
        #ifdef DBG_TXT
            printf("missing '='\n");
        #endif
		return 0;
    }

	if(_ini_allocEntry(File, sectionNum, curEntry) == -1){
        #ifdef DBG_TXT
            printf("failed to allocate entry\n");
        #endif
		return -1;
    }

	char _ini_tempbuf[MAX_LINE_SIZE];
	memset(_ini_tempbuf, 0, MAX_LINE_SIZE);

	while(*test == '=' || *test == ' ' || *test == '\n')test--;
	int len = (int)(test - buffer) + 1;
	strncpy(_ini_tempbuf, buffer, len);
	_ini_setEntryName(&File->Sections[sectionNum].Entry[curEntry], _ini_tempbuf, len);


	#ifdef DBG_TXT
	   printf("Entry: %s\n", _ini_getEntryName(&File->Sections[sectionNum].Entry[curEntry]));
	#endif

	//scan for values now
	memset(_ini_tempbuf, 0, MAX_LINE_SIZE);

	char *start = strchr (buffer, '=');
	while(*start == ' ' || *start == '=') start++;
	len = strlen(start);
	strncpy(_ini_tempbuf, start, len);
	int finalLen = _str_removeTrailing(_ini_tempbuf, len);
	_ini_setEntryValue(&File->Sections[sectionNum].Entry[curEntry], _ini_tempbuf, finalLen, File->mode);

	#ifdef DBG_TXT
		printf("Value: %s\n", _ini_getEntryString(&File->Sections[sectionNum].Entry[curEntry]));
	#endif
	return 1;
}

/*
*	Just copy the line as text to a value
*/
static s8 _ini_checkLine(INIObj_t *File, char *buffer, int sectionNum, int curEntry){
    //allocate a new entry
    if(_ini_allocEntry(File, sectionNum, curEntry) == -1)
        return -1;

	char _ini_tempbuf[MAX_LINE_SIZE >> 1];
	//memset(_ini_tempbuf, 0, sizeof(_ini_tempbuf));
	strcpy(_ini_tempbuf, buffer);
	int newlen = _str_removeTrailing(_ini_tempbuf, strlen(_ini_tempbuf));
	_ini_setEntryValue(&File->Sections[sectionNum].Entry[curEntry], _ini_tempbuf, newlen, File->mode);

    #ifdef DBG_TXT
        printf("Line: %s\n", _ini_getEntryString(&File->Sections[sectionNum].Entry[curEntry]));
    #endif

	return 1;
}

/*========================================================================================================
Sort Ini for faster searching
========================================================================================================*/
//sorts the headers
static int sortSectionsCheck(struct _iniSection *dest, struct _iniSection *src){
  return strcasecmp(_ini_getHeader(dest), _ini_getHeader(src));
}

static int sortSections(struct _iniSection *array, int left, int right){
    struct _iniSection pivot = *((struct _iniSection*)array + left);

    while(left < right){
        while(sortSectionsCheck((void*)((struct _iniSection*)array+left), (void*)((struct _iniSection*)array+right)) < 0) {
            right--;
        }

        if(right== left) break;
        *((struct _iniSection*)array + left) = *((struct _iniSection*)array + right);
        *((struct _iniSection*)array + right) = pivot;

        if(left < right){
            left++;
            if(right== left) break;
        }

        while(sortSectionsCheck((void*)((struct _iniSection*)array+right), (void*)((struct _iniSection*)array+left)) > 0) {
            left++;
        }

        if(left== right) break;
        *((struct _iniSection*)array + right) = *((struct _iniSection*)array + left);
        *((struct _iniSection*)array + left) = pivot;
        right--;
    }
    return left;
}

static void sortSectionsList(void *array, int left, int right){
    if(left < right){
        int mid= sortSections(array, left, right);
        sortSectionsList(array, left, mid-1);
        sortSectionsList(array, mid+1, right);
    }
}


//sorts the entries
static int sortEntrysCheck(struct _iniEntry *dest, struct _iniEntry *src){
  return strcasecmp(_ini_getEntryName(dest), _ini_getEntryName(src));
}

static int sortEntrys(struct _iniEntry *array, int left, int right){
    struct _iniEntry pivot = *((struct _iniEntry*)array + left);

    while(left < right){
        while(sortEntrysCheck((void*)((struct _iniEntry*)array+left), (void*)((struct _iniEntry*)array+right)) < 0) {
            right--;
        }

        if(right== left) break;
        *((struct _iniEntry*)array + left) = *((struct _iniEntry*)array + right);
        *((struct _iniEntry*)array + right) = pivot;

        if(left < right){
            left++;
            if(right== left) break;
        }

        while(sortEntrysCheck((void*)((struct _iniEntry*)array+right), (void*)((struct _iniEntry*)array+left)) > 0) {
            left++;
        }

        if(left== right) break;
        *((struct _iniEntry*)array + right) = *((struct _iniEntry*)array + left);
        *((struct _iniEntry*)array + left) = pivot;
        right--;
    }
    return left;
}

static void sortEntrysList(void *array, int left, int right){
    if(left < right){
        int mid= sortEntrys(array, left, right);
        sortEntrysList(array, left, mid-1);
        sortEntrysList(array, mid+1, right);
    }
}

static void sortFile(INIObj_t *iniFile){
    if(iniFile->mode & INI_SORT){
        sortSectionsList(iniFile->Sections, 0, iniFile->blocks - 1);//sort sections
        int z = 0;
        for(; z < iniFile->blocks; z++)
            sortEntrysList(iniFile->Sections[z].Entry, 0, iniFile->Sections[z].entryCount - 1);
    }
}

/*
Read the ini file
*/


int _ini_loadFile(INIObj_t *iniFile, const char *filePath, u8 readFlags){
	_ini_clean(iniFile);//clear out ram if it has been previously used for another file
	FILE *file = fopen(filePath, "rb");
	if(!file){
        #ifdef DBG_TXT
            printf("file does not exist\n");
        #endif
        iniFile->error = 1;
		return -1;
    }

	iniFile->mode = readFlags;
	char tempBuf[MAX_LINE_SIZE];
	int totalBlocks = 0, totalEntry = 0;
	s8 err = 0;
	while(!feof(file)){
		//memset(tempBuf, 0, MAX_LINE_SIZE);
		char *test = fgets(tempBuf, MAX_LINE_SIZE - 1, file);
		//skip the blank lines
		if(test != NULL && tempBuf[0] != '\n' && tempBuf[0] != '\r' && tempBuf[0] != '\t' && tempBuf[0] != '\0' && tempBuf[0] != INI_COMMENT){
			if(_ini_checkHeader(iniFile, tempBuf, totalBlocks) == 1){
				totalBlocks++;
				totalEntry = 0;//reset entries for new section
			}
			else{
				if(_ini_checkEntry(iniFile, tempBuf, totalBlocks - 1, totalEntry) == 1)
					totalEntry++;
				else{
					//if enabled, read all lines of an ini file regardless if a valid entry
					if((iniFile->mode & INI_NONENTRYS) && _ini_checkLine(iniFile, tempBuf, totalBlocks - 1, totalEntry)> 0)
                        totalEntry++;
                    else{
                        err++;
                        break;
                    }
				}
			}
		}
	}
	fclose(file);
	if(err > 0){
        iniFile->error = 1;
		_ini_clean(iniFile);
		return -2;
	}
    sortFile(iniFile);
	return 1;
}

s8 _ini_newFile(INIObj_t *iniFile, const char *filePath){
    _ini_clean(iniFile);//clear out ram if it has been previously used for another file
    FILE * file = fopen(filePath, "wb");
    if(file == NULL){
        iniFile->error = 1;
        return -1;
    }
    fclose(file);
    iniFile->mode = INI_SORT;
    iniFile->error = 0;
    return 1;
}



/*
Helpful functions for finding entries within the ram array
*/
static inline int _ini_getSortedSection(struct _iniSection *Sections, int count, const char *header){
    register int min = 0, max = count-1, mid = 0, cmp = 0;
    if(max < 0)
        return -2;
    do{
        mid = (min + max) >> 1;
        if(mid > count)
            break;
        char *str = Sections[mid].Header.string;
        if((cmp = strcasecmp(header, str)) == 0)
            return mid;
        else if(cmp < 0)
            max = mid - 1;
        else
            min = mid + 1;
    }while(min <= max);
    return -1;
}

static int _ini_getSection(const INIObj_t *File, const char *header){
    if(File->mode & INI_SORT)//if sort is enabled, then do a quick search
        return _ini_getSortedSection(File->Sections, File->blocks, header);

	int i = 0, test = 0;
	while( i < File->blocks && (test = strcasecmp(header, _ini_getHeader(&File->Sections[i++]))) != 0);
	if(test == 0)
		return i - 1;

	return -1;
}


static inline int _ini_getSortedEntry(const INIObj_t *File, int section, const char *entry){
    register int min = 0, max = File->Sections[section].entryCount - 1, mid = 0, cmp = 0;
    if(max < 0)
        return -2;
    do{
        mid = (min + max) >> 1;
        if(mid > File->Sections[section].entryCount)
            break;
        if((cmp = strcasecmp(entry, _ini_getEntryName(&File->Sections[section].Entry[mid]))) == 0)
            return mid;
        else if(cmp < 0)
            max = mid - 1;
        else
            min = mid + 1;
    }while(min <= max);
    return -1;
}

static int _ini_getEntry(const INIObj_t *File, int section, const char *entry){
	if(File->mode & INI_SORT)
		return _ini_getSortedEntry(File, section, entry);
	else{
		int i = 0, test = 0;
	    char *Name = NULL;
		while(((Name = _ini_getEntryName(&File->Sections[section].Entry[i++])) != NULL) && (i < File->Sections[section].entryCount) && ((test = strcasecmp(entry, Name)) != 0));
		if(test == 0)
			return i - 1;
	}

	return -1;
}


/*
Get a number value from the ini file
*/
void _ini_addMissingInt(INIObj_t *File, const char *header, const char *key, int value){
    char tempBuf[MAX_LINE_SIZE];
    memset(tempBuf, 0, MAX_LINE_SIZE);
    sprintf(tempBuf, "%s=%d\0", key, value);
    _ini_addEntry(File, header, tempBuf);
}

int _ini_getInt(INIObj_t *File, const char *header, const char *key, int def_value){
    if(File->error) goto _no_exist;
	int section = _ini_getSection(File, header);
	if(section < 0) goto _no_exist;

	int entry = _ini_getEntry(File, section, key);
	if(entry < 0) goto _no_exist;

	return _ini_getEntryInt(&File->Sections[section].Entry[entry]);

	_no_exist:
		if(File->mode & INI_ADDMISSING)
        	_ini_addMissingInt((INIObj_t*)File, header, key, def_value);
		return def_value;
}

void _ini_setInt(INIObj_t *File, const char *header, const char *key, int value){
	int section = _ini_getSection(File, header);
	if(section < 0) goto _no_exist;

	int entry = _ini_getEntry(File, section, key);
	if(entry < 0) goto _no_exist;

	char temp[32];
	sprintf(temp, "%d\0", value);
	_ini_setEntryValue(&File->Sections[section].Entry[entry], temp, strlen(temp), File->mode);
	return;

	_no_exist:
        if(File->mode & INI_ADDMISSING)
            _ini_addMissingInt(File, header, key, value);
		return;
}

/*
Get a string from ini file
*/
void _ini_addMissingStr(INIObj_t *File, const char *header, const char *key, const char *value){
    char tempBuf[MAX_LINE_SIZE];
    memset(tempBuf, 0, MAX_LINE_SIZE);
    sprintf(tempBuf, "%s=%s", key, value);
    _ini_addEntry(File, header, tempBuf);
}

char *_ini_getStr(INIObj_t *File, const char *header, const char *key, const char *def_value){
    if(File->error) goto _no_exist;
	int section = _ini_getSection(File, header);
	if(section < 0) goto _no_exist;

	int entry = _ini_getEntry(File, section, key);
	if(entry < 0) goto _no_exist;

	return _ini_getEntryString(&File->Sections[section].Entry[entry]);

	_no_exist:
		if(File->mode & INI_ADDMISSING)
        	_ini_addMissingStr(File, header, key, def_value);


       	strncpy(__defStr, def_value, 512);
		return (char*)__defStr;
}


void _ini_setStr(INIObj_t *File, const char *header, const char *key, const char *string){
	int section = _ini_getSection(File, header);
	if(section < 0) goto _no_exist;

	int entry = _ini_getEntry(File, section, key);
	if(entry < 0) goto _no_exist;

	_ini_setEntryValue(&File->Sections[section].Entry[entry], (char*)string, strlen(string), File->mode);
	return;

	_no_exist:
        if(File->mode & INI_ADDMISSING)
            _ini_addMissingStr((INIObj_t*)File, header, key, string);
		return;
}




/*
Add a new entry to an ini file
*/

s8 _ini_addEntry(INIObj_t *File, const char *header, const char *input){
    char tempBuf[MAX_LINE_SIZE];
    //find section, if section doesn't exist, add it
    int section = 0;
    if((section = _ini_getSection(File, header)) < 0){
        memset(tempBuf, 0, MAX_LINE_SIZE);
        sprintf(tempBuf, "[%s]", header);
        //no header so we must make it
        if(_ini_checkHeader(File, tempBuf, File->blocks) != 1){
            #ifdef DBG_TXT
                printf("Failed adding new section!\n");
            #endif
            return -1;
        }else{
        	section = File->blocks-1;
        }
    }
    memset(tempBuf, 0, MAX_LINE_SIZE);
    strncpy(tempBuf, input, MAX_LINE_SIZE);
    int entry = File->Sections[section].entryCount;
    if(_ini_checkEntry(File, tempBuf, section, entry) != 1){
        #ifdef DBG_TXT
            printf("Failed adding new entry!\n");
        #endif
        return -2;
    }
    sortFile(File);
    return 1;
}

/*
Write INI data to a file
*/
char _ini_writeAll(INIObj_t *ini, const char *fileName){
	FILE *iniFile = fopen(fileName, "wb");
	if(iniFile == NULL)
		return 0;

    char format[12];
    if(ini->mode & INI_NOSPACES)
        strcpy(format, "%s=%s\n");
    else
        strcpy(format, "%s = %s\n");

	int blocks = 0;
	for(blocks = 0; blocks < ini->blocks; blocks++){
        if(blocks>0)
            fprintf(iniFile, "\n");

        if(!ini->Sections)
        	goto error;

		fprintf(iniFile, "[%s]\n", ini->Sections[blocks].Header.string);
		int entry = 0;
		for(entry = 0; entry < ini->Sections[blocks].entryCount; entry++){
			if(!ini->Sections[blocks].Entry)
				goto error;

            //now to write the name and value
			char *Name = _ini_getEntryName(&ini->Sections[blocks].Entry[entry]);
			char *Value = _ini_getEntryString(&ini->Sections[blocks].Entry[entry]);
			if(Value){
				if(Name)//if there is a name, then it is a valid entry)
					fprintf(iniFile, format, Name, Value);
				else//otherwise it's just a line of text
					fprintf(iniFile, "%s\n", Value);
			}
			else
				fprintf(iniFile, "\n");
		}
	}
	fclose(iniFile);
	return 1;

	error:
        #ifdef DBG_TXT
		  printf("ERROR WRITING\n");
        #endif
		fclose(iniFile);
		return 0;
}

struct _iniEntry *_ini_getEntryNum(const INIObj_t *ini, int sectionNum, int entryNum){
	return &ini->Sections[sectionNum].Entry[entryNum];
}


/*
Function initialization
*/
void initFastIni(INIObj_t *ini){
	//memset(ini, 0, sizeof(INIObj_t));
	ini->close =      (void*)&_ini_clean;
	ini->load =       (void*)&_ini_loadFile;
	ini->write =      (void*)&_ini_writeAll;
	ini->getInt =     (void*)&_ini_getInt;
	ini->getStr =     (void*)&_ini_getStr;
    ini->setInt =     (void*)&_ini_setInt;
    ini->setStr =     (void*)&_ini_setStr;
    ini->addEntry =   (void*)&_ini_addEntry;
    ini->newFile =    (void*)&_ini_newFile;

    ini->getEntry =	  (void*)&_ini_getEntryNum;
    ini->entryName=	  (void*)&_ini_getEntryName;
    ini->entryString= (void*)&_ini_getEntryString;
}


//#ifdef DBG_TXT
void _ini_printAll(const INIObj_t *File){
	int blocks = 0;
	for(blocks = 0; blocks < File->blocks; blocks++){
		printf("[%s]\n", File->Sections[blocks].Header);
		int entry = 0;
		for(entry = 0; entry < File->Sections[blocks].entryCount; entry++)
			printf("%s = %s\n", _ini_getEntryName(&File->Sections[blocks].Entry[entry]),_ini_getEntryString(&File->Sections[blocks].Entry[entry]));
	}
}
//#endif

