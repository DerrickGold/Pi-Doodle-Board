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

#include <ds2sdk.h>
#include "text.h"
#include "../core.h"
#include "../Filesystem/filesystem.h"

//#define dbgOut
/*=================================================================================

Some useful functions for reading and grabbing data from the .fnt file

=================================================================================*/
static int characterToNumber(char input){
    if(input > 47 && input < 58)
        return (input-48);
    return 0;
}

static int atoin(char *string, int len){
    int multiplier = 1;

    if(string[0] == '-'){
        multiplier = -1;
        string++;
        len--;
    }

    int total = characterToNumber(string[0]);
    for(int i = 1; i < len; i++){
        total *= 10;
        total += characterToNumber(string[i]);
    }
    return total*multiplier;
}


//get position on line to read settings value
static int fntFindSetting(char *line, const char *setting, int lineLen, int *inc){
    char *oldLine = line;
    lineLen--;

    while(strncmp(line, setting, strlen(setting)) && lineLen >= 0){
        //if setting isn't found, continue from the next caracter and compare
        line++;
        lineLen--;
    }
    //setting wasn't found on this line
    if(lineLen < 0){
        (*inc) = 0;
        return 0;
    }
    line += strlen(setting);//seek to value
    line += (*line == '=');//skip equal sign

    (*inc) = (abs(oldLine - line));
    return 1;
}


//returns an integer value from a setting
static int fntReadInt(char *line, const char *setting, int lineLen, int *inc){
    lineLen -= (*inc);//adjust for new line size

    //printf("searching for setting\n");
    //search line for the setting
    int offset = 0;
    if(!fntFindSetting(line, setting, lineLen, &offset))
        return NO_SETTING;

    line += offset;

    //printf("setting found!\n");
    //otherwise, yes it was found!
    //Now to store the value up to the space after it
    int finalValue = 0;
    //printf("%s\n", line);
    //char *commas = strchr(line, ',');
    int space = strcspn(line, " \n\r");
    int comma = strcspn(line, ",\0");
    if(space < comma){//no commas, so no lists of values
        finalValue = atoin(&line[0], space);
        (*inc) += (offset + space);
    }
    else{//there is a list of values
        char *value = (char*)&finalValue;
        int pos = 0;
        for(int i = 0; i < 4; i++){
            int len = strcspn (&line[pos], ", \n\0");
            value[i] = atoin(&line[pos], len);
            pos += len;
            if(line[pos] == ',')pos++;
            else break;
        }
        (*inc) += (offset + pos);
    }

    return finalValue;
}

//returns a string from a setting
static char *fntReadStr(char *line, const char *setting, int lineLen, int *inc){
    //search line for the setting
    int offset = 0;
    if(!fntFindSetting(line, setting, lineLen, &offset))
        return NULL;

    //now scan for string
    line += offset;
    if(*line == '"')line++;

    char *start = line;
    while(*line != '"')
        line++;

    //allocate space for string
    int len = (line - start);
    char *string = bag_calloc(len + 1, sizeof(char));//add one for null termination
    if(!string){
        BAG_DBG_LibMsg("Error allocating string!\n");
        return NULL;
    }

    //and finally copy the string
    strncpy(string, start, len);

    (*inc) += (offset + len + 2);
    return string;
}



/*=================================================================================

Now for reading the .fnt file itself

=================================================================================*/

/*============================
Start with the header
============================*/
static int fntFile_readHeader(FILE *file, FNTObj_t *font){
    char line[MAX_PATH];
    int curLine = 0;
    int pageCount=0;
    //reserve 11 lines for header
    while(!feof(file) && curLine < 4){
        //grab the first line
        char *result = fgets (line, sizeof(line), file);
        if(result == NULL || feof(file)){
            BAG_DBG_LibMsg("End of file reached in header\n");
            return ERR_END;
        }

        //set position in line to 0
        int linePos = 0;
        #if defined(___SYS_DS2_)
            __dcache_writeback_all();
        #endif
        switch(curLine){
            case 0:{//info to grab in the first line

                //get font name
                font->Header.faceName = fntReadStr(&line[linePos], "info face", MAX_PATH, &linePos);
                if(!font->Header.faceName)
                    return ERR_END;

                //font scale size
                font->Header.size = fntReadInt(&line[linePos], "size", MAX_PATH, &linePos);

                curLine++;
            }break;
            case 1://info to grab in the second line
                font->Header.lineHeight = fntReadInt(&line[linePos], "common lineHeight", MAX_PATH, &linePos);
                font->Header.base = fntReadInt(&line[linePos], "base", MAX_PATH, &linePos);
                font->Header.pages = fntReadInt(&line[linePos], "pages", MAX_PATH, &linePos);

                if(font->Header.pages > 0){
                    font->Header.pageID = bag_calloc(font->Header.pages, sizeof(char*));
                    if(!font->Header.pageID){
                       BAG_DBG_LibMsg("Failed to allocate page ids\n");
                       return ERR_END;
                    }
                }
                curLine++;
            break;
            //reading the page names
            case 2:
                #ifdef dbgOut
                    printf("reading font pages\n");
                #endif
                if(pageCount < font->Header.pages){
                    int id = fntReadInt(&line[linePos], "page id", MAX_PATH, &linePos);
                    if(id == NO_SETTING){
                        printf("improperly read id number\n");
                        printf("line: %s\n", &line[linePos]);
                    }
                    else{
                        font->Header.pageID[id] = fntReadStr(&line[linePos], "file", MAX_PATH, &linePos);
                        pageCount++;
                        if(pageCount >= font->Header.pages)
                            curLine++;
                        //read next line in the file
                        continue;
                    }
                }
                else
                    curLine++;
            break;
            case 3://character count and exit
                font->Header.charCount = fntReadInt(&line[0], "chars count", MAX_PATH, &linePos);
                curLine++;
            break;
        }
    }
    #ifdef dbgOut
        printf("name:%s\n", font->Header.faceName);
        printf("size: %d\n", font->Header.size);
        printf("commonlineheight: %d\n", font->Header.lineHeight);
        printf("base: %d\n", font->Header.base);
        printf("pages: %d\n", font->Header.pages);
        for(int z = 0; z < font->Header.pages; z++)
            printf("page name: %s\n", font->Header.pageID[z]);
        printf("char count: %d\n", font->Header.charCount);

    #endif
    #if defined(___SYS_DS2_)
        __dcache_writeback_all();
    #endif
    font->Header.scale = 100;//default size
    return ERR_NONE;
}


/*============================
Next we need to read the
characters, but they need to
be sorted so that they fit
within a reasonable amount of
memory and can be retrieved
faster.
============================*/
//sorting and retrieval of jump and variable lists for parsing
static int sort_function(const void *dest_str_ptr, const void *src_str_ptr){
    fntChar *src = (fntChar*)src_str_ptr;
    fntChar *dest = (fntChar*)dest_str_ptr;

    int destNum = src->number;
    int srcNum = (int)dest->number;

    return destNum-srcNum;
}



static void char_ListSort(fntChar *data, int count){
    BAG_Qsort(data, count - 1, sizeof(fntChar), &sort_function);
}



int binFind(const void *data, const void *cmpVal){
    fntChar *letter = (fntChar*)data;
    int *searchFor = (int*)cmpVal;
    return letter->number - *searchFor;
}



static int char_listGet(const fntChar *data, int count, int number){
    return BAG_binSearch((void*)data, count, sizeof(fntChar), (void*)&number, &binFind);
}



/*============================
Now to actually read the
characters
============================*/
static int fntFile_readCharacters(FILE *file, FNTObj_t *font){
    //allocate all the characters
    font->Character = calloc(font->Header.charCount, sizeof(fntChar));
    if(!font->Character){
       BAG_DBG_LibMsg("Failed to allocate %d character(s)\n", font->Header.charCount);
       return ERR_END;
    }

    #ifdef dbgOut
        printf("allocated characters\n");
    #endif

    char line[MAX_PATH];

    int curChar = 0;

    font->Header.asciiPos = 0;

    while(!feof(file) && curChar < font->Header.charCount){
        char *result = fgets (line, sizeof(line), file);
        if(result == NULL){
            //BAG_DBG_LibMsg("End of file reached in header\n");
            //return ERR_END;
            //printf("end of file hit before last character\n");
            break;
        }


        int linePos = 0;
        int idNum = fntReadInt(&line[linePos], "char id", MAX_PATH, &linePos);
        if(idNum == NO_SETTING){
            printf("no setting!\n");
           break;
        }

        font->Character[curChar].number = idNum;
        font->Character[curChar].x = fntReadInt(&line[linePos], "x=", MAX_PATH, &linePos);
        font->Character[curChar].y = fntReadInt(&line[linePos], "y", MAX_PATH, &linePos);
        font->Character[curChar].width = fntReadInt(&line[linePos], "width", MAX_PATH, &linePos);
        font->Character[curChar].height = fntReadInt(&line[linePos], "height", MAX_PATH, &linePos);
        font->Character[curChar].xoffset = fntReadInt(&line[linePos], "xoffset", MAX_PATH, &linePos);
        font->Character[curChar].yoffset = fntReadInt(&line[linePos], "yoffset", MAX_PATH, &linePos);
        font->Character[curChar].xadvance = fntReadInt(&line[linePos], "xadvance", MAX_PATH, &linePos);
        font->Character[curChar].page = fntReadInt(&line[linePos], "page", MAX_PATH, &linePos);
        curChar++;
    }
    char_ListSort(font->Character, font->Header.charCount);

    //set up ascii position
    font->Header.asciiPos = char_listGet(font->Character, font->Header.charCount, ASCII_START);

    #ifdef dbgOut
        printf("done collecting chars\n");
    #endif
    #if defined(___SYS_DS2_)
        __dcache_writeback_all();
    #endif
    return ERR_NONE;
}


/*============================
Get the image files with
the font frames.
============================*/
static int fntFile_getImages(const char *filePath, FNTObj_t *font){
    #ifdef dbgOut
        printf("allocating gfx\n");
    #endif
    if(font->Header.pages <= 0)
        return ERR_END;

    //then load graphics
    font->image = calloc(font->Header.pages, sizeof(GFXObj_t));
    if(!font->image){
        BAG_DBG_LibMsg("Error allocating text graphics\n");
        return ERR_END;
    }

    #ifdef dbgOut
        printf("gfx allocated, loading\n");
    #endif
    //create the new file names
    char newFile[MAX_PATH];
    memset(newFile, 0, sizeof(newFile));
    strncpy(newFile, filePath, MAX_PATH);
    for(int i = 0; i < font->Header.pages; i++){
        BAG_Filesystem_StripTrailingDirs(1, newFile);
        strcat(newFile, DIR_SEPARATOR_STR);
        strcat(newFile, font->Header.pageID[i]);
        BAG_Display_LoadObj(newFile, &font->image[i]);
        //(*BAG_Display_GetGfxTranspCol(&font->image[i])) = RGB15(0,0,0);
        BAG_Display_SetFontColor(&font->image[i], RGB15(31,63,31));
        SET_FLAG(VAL(BAG_Display_GetGfxFlags(&font->image[i])), OBJ_ISFONT);
        #if defined(___SYS_DS2_)
            __dcache_writeback_all();
        #endif

    }
    #ifdef dbgOut
        printf("graphics loaded\n");
    #endif

    return ERR_NONE;
}


/*============================
Some behind the scenes
functions
============================*/
static int fntFile_scale(int input, int percent){
    if(percent == 100)
        return input;
    //can't use decimals, so use integer values
    else if(percent < 100 && abs(input) <= 1)
        return input;

    return ((input * percent)/100);
}

static inline int fntFile_getScale(FNTObj_t *font){
    return font->Header.scale;
}



static void fntFile_scaleLetterGfx(FNTObj_t *font, int percent, fntChar *letter){
    if(percent == 100)//no change
        return;

    GFXObj_t *page = &font->image[letter->page];

    int newWd = fntFile_scale(letter->width, percent);
    int newHt = fntFile_scale(letter->height, percent);

    BAG_Display_SetGfxScaleWdHt(page, newWd, newHt);
}

static inline void fntFile_newLine(TextBox_t *tbox, FNTObj_t *font){
    tbox->printY += fntFile_scale(font->Header.lineHeight, font->Header.scale);
    tbox->printX = tbox->x1;
    tbox->curLine++;
}

static inline fntChar *fntFile_getChar(FNTObj_t *font, int character){
    if(character >= ASCII_START && character <= ASCII_END){
        int newPos = character - ASCII_START;
        return &font->Character[font->Header.asciiPos + newPos];
    }

    int idNum = char_listGet(font->Character, font->Header.charCount, character);
    if(idNum < 0)
        return NULL;
    return &font->Character[idNum];
}

static inline int textBox_checkWordWrap(FNTObj_t *font, TextBox_t *tbox, wString *string, int stringOldLen){
    int pixLength = BAG_Font_GetStrSize(font, string);
    if(tbox->printX + pixLength > tbox->x2){
        while(tbox->printX + pixLength > tbox->x2 && string->len > string->strtokStart){
            string->len--;
            //find beginning of last word that fits
            if(string->array[string->len] == ' ' || string->array[string->len] == '\n')//once we have the word, then we can update the length
                pixLength = BAG_Font_GetStrSize(font, string);
            //newLine++;
        }
        if(string->len <= string->strtokStart){//if we can't find a word to wrap with, restore length, then try letter wrapping
            string->len = stringOldLen;
            return 0;
        }
        else//otherwise break; the flow and go as normal
            return 1;
    }
    return 0;
}

static inline int textBox_checkLetterWrap(FNTObj_t *font, TextBox_t *tbox, wString *string){
    int pixLength = BAG_Font_GetStrSize(font, string);
    int newLine = 0;
    while(tbox->printX + pixLength > tbox->x2){
        string->len--;
        pixLength = BAG_Font_GetStrSize(font, string);
        newLine++;
    }
    return newLine > 0;
}

//returns if a new line is needed or not
static int textBox_getWrapping(FNTObj_t *font, TextBox_t *tbox, wString *string, int oldLen){
    int newLine = 0;
    switch(tbox->wrapMode){
        default:
        break;//do nothing

        /*
            Wrap text by words. If a word cannot be found,
            will fall back to letter wrapping.
        */
        case WRAP_WORD:
            if(textBox_checkWordWrap(font, tbox, string, oldLen)){
                newLine++;
                break;
            }//if no word wrap, then we can do a letter wrap (if word is > line size)
        /*
            Wrap text by letters at a time.
        */
        case WRAP_LETTER:
            newLine += textBox_checkLetterWrap(font, tbox, string);
        break;
    }
    return newLine > 0;
}
/*=================================================================================

Publically accessable functions
    -names should be self explanitory
=================================================================================*/
void BAG_Font_TextBoxSetLimits(TextBox_t *box, int x1, int y1, int x2, int y2){
    box->x1 = x1;
    box->x2 = x2;
    box->y1 = y1;
    box->y2 = y2;
    BAG_Font_TextBoxSetCharLim(box, TBOX_WHOLESTR);
}

void BAG_Font_TextBoxResetPrintX(TextBox_t *box){
    box->printX = box->x1;
}

void BAG_Font_TextBoxResetPrintY(TextBox_t *box){
    box->printY = box->y1;
    box->curLine = 0;
    box->numLines = 0;
}

void BAG_Font_TextBoxResetPrintXY(TextBox_t *box){
    BAG_Font_TextBoxResetPrintX(box);
    BAG_Font_TextBoxResetPrintY(box);
}

void BAG_Font_TextBoxSetOutEx(TextBox_t *box, unsigned short *dest, int destWd, int destHt){
    box->dest = dest;
    box->destWd = destWd;
    box->destHt = destHt;
}

void BAG_Font_TextBoxSetOut(TextBox_t *box, GFXObj_t *object){
    BAG_Font_TextBoxSetOutEx(box,
                             BAG_Display_GetGfxBuf(object),
                             *BAG_Display_GetGfxWidth(object),
                             *BAG_Display_GetGfxHeight(object));
}

void BAG_Font_TextBoxAlign(TextBox_t *box, int alignment){
    box->alignment = alignment;
}

void BAG_Font_TextBoxWrap(TextBox_t *box, int wrap){
    box->wrapMode = wrap;
}

void BAG_Font_TextBoxSetCharLim(TextBox_t *box, int charLim){
    box->charCount = charLim;
}

void BAG_FontTextBoxMove(TextBox_t *box, int newX, int newY){
    int xDiff = newX - box->x1,
        yDiff = newY - box->y1;

    //now add difference to all positions
    box->x1 += xDiff;
    box->x2 += xDiff;
    box->printX += xDiff;

    box->y1 += yDiff;
    box->y2 += yDiff;
    box->printY += yDiff;
}

//insert new line into the text box
void BAG_Font_TextBoxNewLine(TextBox_t *box, FNTObj_t *font){
    fntFile_newLine(box, font);
}


void BAG_Font_TextBoxSetMaxLines(TextBox_t *box, int lines){
    box->maxLines = lines;
}


void BAG_Font_Unload(FNTObj_t *font){
    //unload header information
    if(font->Header.faceName)
        free(font->Header.faceName);
    font->Header.faceName = NULL;

    if(font->Header.pageID){
        for(int i = 0; i < font->Header.pages; i++){
            if(font->Header.pageID[i])
                free(font->Header.pageID[i]);
            font->Header.pageID[i] = NULL;
        }

        free(font->Header.pageID);
    }
    font->Header.pageID = NULL;

    //unload graphics
    if(font->image){
        for(int i = 0; i < font->Header.pages; i++)
            BAG_Display_DeleteObj(&font->image[i]);
        free(font->image);
    }
    font->image = NULL;
    #if defined(___SYS_DS2_)
        __dcache_writeback_all();
    #endif
    //memset(font, 0, sizeof(FNTObj_t));
}


int BAG_Font_Load(const char *filePath, FNTObj_t *font){

    BAG_Font_Unload(font);
    #ifdef dbgOut
        printf("loading: %s\n", filePath);
    #endif

    //first read the fnt file for informatioj
    FILE *tempFile = fopen(filePath, "rb");
    if(!tempFile){
        BAG_DBG_LibMsg(OPEN_FAIL_MSG, filePath);
        return ERR_END;
    }

    #ifdef dbgOut
        printf("file opened\n");
    #endif

    int err = ERR_NONE;
    if((err = fntFile_readHeader(tempFile, font)) == ERR_NONE){
        if((err = fntFile_getImages(filePath, font)) == ERR_NONE)
            err = fntFile_readCharacters(tempFile, font);
    }
    fclose(tempFile);

    if(err != ERR_NONE){
        BAG_Font_Unload(font);
        BAG_DBG_LibMsg("BAG:Failed to open %s\n", filePath);
    }

    return err;
}


inline void BAG_Font_ScaleFont(FNTObj_t *font, int percent){
    font->Header.scale = percent;
}

int BAG_Font_GetCharWd(FNTObj_t *font, int character){
    fntChar *letter = fntFile_getChar(font, character);

    if(!letter) return 0;

    return fntFile_scale(letter->xadvance, font->Header.scale);
}


int BAG_Font_GetCharHt(FNTObj_t *font){
    return fntFile_scale(font->Header.lineHeight, font->Header.scale);
}

static int processColorCode(WSTRING_TYPE *buf, FNTObj_t *font, int *skipped){
    int pos = 0;
    unsigned short newCol = 0;
    if(buf[pos] == '{' && buf[pos+1] == '^' && buf[pos+2] == 'C'){
        //skip past these
        pos += 3;

        char colBuf[8];
        memset(colBuf, 0, 8);
        char *col = &colBuf[0];
        while(buf[pos] != '}'){
            *col++ = (char)buf[pos++];
        }
        pos++;
        newCol = atoi(colBuf);
    }
    (*skipped) += pos;
    //return original color
    return newCol;
}


int BAG_Font_GetStrSizeEx(FNTObj_t *font, wString *string, int len){
    int size = 0, oldSize = 0;
    WSTRING_TYPE *buf = string->string(string);
    for(int i = 0; i < len; i++){

        int skipped = 0;
        if(processColorCode(&buf[i], font, &skipped))
            i+=skipped;

        int character = buf[i];

        //store the longest line
        if(character == '\n'){
            if(size > oldSize)
                oldSize = size;
        }

        size += BAG_Font_GetCharWd(font, character);
        // + fntFile_scale(font->Character[idNum].xoffset, font->Header.scale));
    }
    if(oldSize < size)
        oldSize = size;

    return oldSize;
}

int BAG_Font_GetStrSize(FNTObj_t *font, wString *string){
    return BAG_Font_GetStrSizeEx(font, string, string->length(string));
}


static int _estimateLineCount(wString *string, FNTObj_t *font, TextBox_t *tbox){
    int curLines = 0, curWd = 0;

    WSTRING_TYPE *buf = string->string(string);
    for(int i = 0; i < string->length(string); i++){
        if(buf[i] == '\n'){
            curLines++;
            curWd = 0;
        }
        else{
            curWd += BAG_Font_GetCharWd(font, buf[i]);
            if(tbox->x1 + curWd >= tbox->x2){
                curLines++;
                curWd = 0;
            }
        }
    }
    return curLines;
}




void BAG_Font_PrintCharInfo(int character, FNTObj_t *font){
    fntChar *letter = fntFile_getChar(font, character);
    if(!letter)
        return;

    printf("character->%c:\n", (char)letter->number);
    printf("x:%d y:%d\n", letter->x, letter->y);
    printf("width:%d height:%d\n", letter->width, letter->height);
    printf("xoffset:%d yoffset:%d\n", letter->xoffset, letter->yoffset);
    printf("xdvance:%d\n", letter->xadvance);
    printf("page:%d\n", letter->page);
}




int BAG_Font_PrintChar(FNTObj_t *font, int character, int x, int y, unsigned short *dest, int destWd, int destHt){
    fntChar *letter = fntFile_getChar(font, character);
    if(!letter)
        return 0;

    GFXObj_t *page = &font->image[letter->page];
    fntFile_scaleLetterGfx(font, font->Header.scale, letter);

    //locate character on bitmap
    (*BAG_Display_GetGfxOffsetX(page)) = letter->x;
    (*BAG_Display_GetGfxOffsetY(page)) = letter->y;
    BAG_Display_SetGfxFrameDim(page, letter->width, letter->height);

    BAG_Display_SetGfxBlitXY(page, x + fntFile_scale(letter->xoffset, font->Header.scale), y + fntFile_scale(letter->yoffset, font->Header.scale));
    BAG_Display_DrawObjSlowEx(page, dest, destWd, destHt);
    return fntFile_scale(letter->xadvance, font->Header.scale);
}


void BAG_Font_SetFontColor(FNTObj_t *font, unsigned short color){
    for(int i = 0; i < font->Header.pages; i++)
        BAG_Display_SetFontColor(&font->image[i], color);
}

int BAG_Font_GetFontColor(FNTObj_t *font){
    return (*BAG_Display_GetFontColor(&font->image[0]));
}

void BAG_Font_SetSmoothing(FNTObj_t *font, int smoothing){
    for(int i = 0; i < font->Header.pages; i++)
        BAG_Display_SetGfxPixSmoothing(&font->image[i], smoothing);
}


static int _fntprintOut(TextBox_t *tbox, wString *string, FNTObj_t *font){

    int len = string->length(string);
    WSTRING_TYPE *buf = string->string(string);

    for(int i = 0; i < len; i++){
        //keep track of how many characters we are drawing
        if(tbox->charCount <= 0)
            return 0;
        tbox->charCount--;


        //check for different colors
        //calculate skipped count
        int skipped = 0;
        int newCol = processColorCode(&buf[i], font, &skipped);
        if(skipped){
            tbox->charCount -= skipped;
            i += skipped;
            BAG_Font_SetFontColor(font, newCol);
        }

        WSTRING_TYPE character = buf[i];
        //new line
        if(character == '\n'){
            fntFile_newLine(tbox, font);
            return 1;
        }


        //stop printing rest of line if out of text box
        if(tbox->printX >= tbox->x2){
            //i++;
            tbox->charCount -= (len - 1);
            if(tbox->charCount <= 0)
                return 0;
            return 2;
        }

        if(tbox->printX + BAG_Font_GetCharWd(font, character) < tbox->x2)
            tbox->printX += BAG_Font_PrintChar(font, character, tbox->printX, tbox->printY,
                                                tbox->dest, tbox->destWd, tbox->destHt);
        else//string too big
            return 2;
    }
    return 1;
}

void BAG_Font_TBoxAlignment(FNTObj_t *font, TextBox_t *tbox, wString *string){
    int textPixLen = BAG_Font_GetStrSize(font, string);
    switch(tbox->alignment){
        //default left alignment
        default:break;

        case ALIGN_RIGHT:{
            int start = tbox->x1;
            //if(start < 0)start = 0;
            tbox->printX = start + (abs(tbox->x2 - start) - textPixLen);
        }break;

        case ALIGN_CENTERX:{
            int start = tbox->x1;
            //if(start < 0)start = 0;
            tbox->printX = start + ((abs(tbox->x2 - start) - textPixLen)>>1);
        }break;
        case ALIGN_CENTERY:{
        }break;
        case ALIGN_CENTER:{
            int start = tbox->x1;
            //if(start < 0)start = 0;
            tbox->printX = start + ((abs(tbox->x2 - start) - textPixLen)>>1);
        }break;
    }
}



//void BAG_Font_PrintEx(unsigned short *dest, int x, int y, wString *string, FNTObj_t *font, int destWd, int destHt, int len){
void BAG_Font_PrintEx(TextBox_t *tbox, wString *string, FNTObj_t *font){
    //check if no characters to be printed
    int oldOutputLen = tbox->charCount;
    if(tbox->charCount == 0)
        return;
    //check if whole string to be printed
    else if(tbox->charCount == -1)
        tbox->charCount = string->length(string);

    //text is completely off screen
    if(tbox->x2 < 0 || tbox->y2 < 0)
        return;

    //estimate the number of lines before printing to center y coords
    if(tbox->alignment == ALIGN_CENTER || tbox->alignment == ALIGN_CENTERY){

        if(tbox->curLine == 0 && tbox->maxLines == 0){
            tbox->numLines = _estimateLineCount(string, font, tbox);
            tbox->numLines += (tbox->numLines == 0);//make sure tbox->numLines isn't 0

            //if no lines have been printed yet, set the inital y position
            tbox->printY = tbox->y1 + ((abs(tbox->y2 - tbox->y1) - (BAG_Font_GetCharHt(font) * tbox->numLines)) >> 1);
        }
        else if(tbox->curLine == 0 && tbox->maxLines > 0)
            tbox->printY = tbox->y1 + ((abs(tbox->y2 - tbox->y1) - (BAG_Font_GetCharHt(font) * tbox->maxLines)) >> 1);

    }


    int oldStrStart = string->strtokStart;
    int oldLen = string->length(string) + string->strtokStart;
    int fullLen = oldLen;
     //   int curLine = 0;
    while(string->strtokStart < fullLen){
        int newLine = 0;
        string->len = oldLen;

        /*
            Splite the text into chunks of newlines,
            if possible.
        */
        string->len = string->strtokStart;
        while(string->array[string->len] != '\n' && string->len < oldLen){
            string->len++;
        }
        //ensure we aren't starting on a newline character
        string->len += (string->array[string->len] == '\n');

        //if there is wrapping, then a newline is needed
        newLine += textBox_getWrapping(font, tbox, string, oldLen);

        /*
            Calculate current lines x position based on
            set alignment.
        */
        BAG_Font_TBoxAlignment(font, tbox, string);

        /*
            Print out the current line of text.
        */
        int status = _fntprintOut(tbox, string, font);
        if(!status || (status == 2 && tbox->wrapMode == WRAP_NONE)){
            //no more characters to draw!
            break;
        }

        //move to next line if necessary
        if(newLine)
            fntFile_newLine(tbox, font);

        //stop printing if hit bottom of text box
        if(tbox->printY >= tbox->y2)
            break;

        if(tbox->maxLines > 0 && tbox->curLine >= tbox->maxLines){
            tbox->maxLines = 0;//reset max lines when hit?
            break;
        }

        //update number of characters output
        string->strtokStart += string->length(string);
    }

    //restore strings tokenization settings
    string->strtokStart = oldStrStart;
    string->len = oldLen;
    tbox->charCount = oldOutputLen;
}

void BAG_Font_Print(u16 *screen, TextBox_t *tbox, wString *string, FNTObj_t *font){
    u16 *dest = screen;

    BAG_Font_TextBoxSetOutEx(tbox, dest, SCREEN_WIDTH, SCREEN_HEIGHT);

    //reset tbox position
    BAG_Font_TextBoxResetPrintXY(tbox);

    BAG_Font_PrintEx(tbox, string, font);
}


void BAG_Font_Print2(unsigned short *dest, TextBox_t *tbox, wString *string, FNTObj_t *font){
    BAG_Font_TextBoxSetOutEx(tbox, dest, SCREEN_WIDTH, SCREEN_HEIGHT);

    //reset tbox position
    BAG_Font_TextBoxResetPrintXY(tbox);

    BAG_Font_PrintEx(tbox, string, font);
}


