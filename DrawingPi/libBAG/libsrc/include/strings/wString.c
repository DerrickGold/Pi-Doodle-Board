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


#include "wString.h"
#include "../debug/libBAG_Debug.h"


static const char boolTexts[3][2][8] = {
    {
        "false",
        "true"
    },
    {
        "no",
        "yes"
    },
    {
        "off",
        "on"
    },
};

#define MODE_CHECKBOX 3
#define MODE_CHECKMARK 4

typedef enum{
    UNICODE_CHECKMARK = 10003,
    UNICODE_BOX = 9744,
    UNICODE_CHECKBOX = 9745,
}SPECIAL_CHARACTERS;


/*===============================================================================
Some helpful conversion stuff
===============================================================================*/
static inline char letterToSmall(char letter){
    if((letter >= 'A')&&(letter <= 'Z'))
        return (letter-'A'+'a');

   return letter;
}

static inline char letterToBig(char letter){
    if((letter >= 'a' && letter <= 'z'))
        return (letter - 'a' + 'A');

    return letter;
}


static int numToStr(void *num, char format, WSTRING_TYPE *out){
    int value = (*(int*)num);
    int len = 0;

    //check if its zero first
    if(value == 0){
        out[len++] = 48;
        return len;
    }
    //otherwise add negative sign if necessary
    else if(value < 0){
        out[len++] = '-';
        //make value positive for conversion process
        value *= -1;
    }

    //collect all the digits
    char digit[16];
    int i = 0;
    for(; i < 16 && value > 0; i++){
        digit[i] = (value % 10);
        value /= 10;
    }
    //now convert them to letters in the right order
    i--;
    for(; i >= 0; i--)
        out[len++] = (digit[i] + 48);

    return len;
}


//converts a bool value to string of true or false
static int boolToStr(char value, WSTRING_TYPE *out, char capMode, const char text[2][8]){
    char output[8];
    if(value)
        value = 1;

    strcpy(output, text[value]);

    switch(capMode){
        default:break;
        case 1://capitolize the first letter
            output[0] = letterToBig(output[0]);
        break;
    }

    int i = 0;
    for(;i < strlen(output); i++){
        //capitolize whole word
        if(capMode == 2)
            output[i] = letterToBig(output[i]);
        out[i] = output[i];
    }

    return i;
}





static inline int wStringToU16(WSTRING_TYPE *out, const wString *input){
    int i = 0;
    for(; i < input->len; i++)
        *out++ = input->array[i];

    return i;
}



static inline int u16Tochar( const WSTRING_TYPE* src, char* dest, int srcLen)
{
    int len=0;
    while(srcLen >= 0){
        if(*src <= 0)
            src++;

        if (*src < 0x80) //1 byte
        {
            dest[len++] = *src;
        }
        else if (*src < 0x800) //2 bytes
        {
            dest[len++] = 0xC0 | (*src >> 6 & 0x1F);
            dest[len++] = 0x80 | (*src & 0x3F);
        }
        else //if(*src < 0x10000) //3 bytes
        {
            dest[len++] = 0xE0 | (*src >> 12);
            dest[len++] = 0x80 | (*src >>6 & 0x3F);
            dest[len++] = 0x80 | (*src &0x3F);
        }
        src ++;
        srcLen--;
    }
    dest[len] = 0;
    return len;
}


static inline const char* utf8decode(const char* utf8, WSTRING_TYPE *ucs){
    unsigned char c = *utf8++;
    unsigned long code;
    int tail = 0;

    if ((c <= 0x7f) || (c >= 0xc2)) {
        /* Start of new character. */
        if (c < 0x80) {        /* U-00000000 - U-0000007F, 1 byte */
            code = c;
        } else if (c < 0xe0) { /* U-00000080 - U-000007FF, 2 bytes */
            tail = 1;
            code = c & 0x1f;
        } else if (c < 0xf0) { /* U-00000800 - U-0000FFFF, 3 bytes */
            tail = 2;
            code = c & 0x0f;
        } else if (c < 0xf5) { /* U-00010000 - U-001FFFFF, 4 bytes */
            tail = 3;
            code = c & 0x07;
        } else {
            /* Invalid size. */
            code = 0;
        }

        while (tail-- && ((c = *utf8++) != 0)) {
            if ((c & 0xc0) == 0x80) {
                /* Valid continuation character. */
                code = (code << 6) | (c & 0x3f);

            } else {
                /* Invalid continuation char */
                code = 0xfffd;
                utf8--;
                break;
            }
        }
    } else {
        /* Invalid UTF-8 char */
        code = 0;
    }
    /* currently we don't support chars above U-FFFF */
    *ucs = code;
    //(code < 0x10000) ? code : 0;
    return utf8;
}


static size_t copyCharToU16(WSTRING_TYPE *out, int outLen, const char *input){
    size_t i = 0, len = strlen(input);
    for(; i < len && i < outLen && *input != '\0'; i++)
        //*out++ = *input++;
        input = utf8decode(input, out++);

    return i;
}

/*===============================================================================
Some access functions
===============================================================================*/


static WSTRING_TYPE *wString_getArray(wString *input){
    return (WSTRING_TYPE *)&input->array[input->strtokStart];
}

static int wString_getLen(wString *input){
    return input->len - input->strtokStart;
}

static inline void wString_setLength(wString *str, int newLen){
    if(newLen <= str->len)
        str->len = newLen;
}

static inline void wString_restoreLength(wString *str){
    str->len = str->oldLen;
}

/*===============================================================================
General String initialization and deletion
===============================================================================*/


#if defined(WSTRING_ASCII)
    static inline void wString_asciiDel(wString *string){
        #ifdef WSTRING_ALLOC
            if(string->asciiLen > 0 && string->ascii)
                free(string->ascii);

            string->ascii = NULL;
        #else
            memset(string->ascii, 0, sizeof(string->ascii));
        #endif
        string->asciiLen = 0;
    }
#endif

static inline void wString_del(wString *string){
    #ifdef WSTRING_ALLOC
        if(string->len > 0 && string->array)
            free(string->array);

        string->array = NULL;
    #else
        memset(string->array, 0, sizeof(string->array));
    #endif
    string->len = 0;
    string->strtokStart = string->nextStrTok = 0;
    #if defined(WSTRING_ASCII)
        wString_asciiDel(string);
    #endif
    //memset(string, 0, sizeof(wString));
}

static inline int wString_newAlloc(wString *string, int len){
    wString_del(string);

    #ifndef WSTRING_ALLOC
        if(len >= MAX_PATH<<1)
            len = (MAX_PATH<<1)-1;
    #endif

    string->len  = string->oldLen = len;
    #ifdef WSTRING_ALLOC
        string->array = calloc(len, sizeof(WSTRING_TYPE));
    #endif
    #ifdef WSTRING_ALLOC
        return !(!string->array);
    #else
        return 1;
    #endif
}

static inline int wString_realloc(wString *string, int newLen){
    #ifdef WSTRING_ALLOC
        WSTRING_TYPE *temp = realloc(string->array, newLen);
        if(!temp)
            return 0;

        string->array = temp;
    #else
        if(newLen < string->len)
            memset(&string->array[newLen+1], 0, string->len *sizeof(WSTRING_TYPE));
        else if(newLen > (MAX_PATH<<1))
            newLen = (MAX_PATH<<1);
    #endif

    string->len = string->oldLen = newLen;
    string->strtokStart = string->nextStrTok = 0;
    return 1;
}



/*===============================================================================
Handling string ascii information
===============================================================================*/

#if defined(WSTRING_ASCII)
    static char *wString_toAscii(wString *string){
        wString_asciiDel(string);

        #ifdef WSTRING_ALLOC
            //at least allocate enough space if the whole string is unicode
            string->ascii = calloc((string->len*2) + 1, sizeof(char));
            if(!string->ascii){
                BAG_DBG_LibMsg("Error allocating ascii string!\n");
                return NULL;
            }

        #endif
        string->asciiLen = 0;
        //grab utf buffer
        WSTRING_TYPE *buf = wString_getArray(string);

        //convert it to asci
        string->asciiLen = u16Tochar(buf, string->ascii, string->len);


        #ifdef WSTRING_ALLOC
            //resize the string if not using a constant buf
            char *temp = realloc(string->ascii, string->asciiLen);
            if(!temp){
                BAG_DBG_LibMsg("Error reallocating ascii string!\n");
                return NULL;
            }
            string->ascii = temp;

            return string->ascii;
        #else
            return (char*)&string->ascii;
        #endif
    }
#endif


static inline int _isDeliminator(WSTRING_TYPE letter, const WSTRING_TYPE *delimiters, int dimLen){
    for(int z = 0; z < dimLen; z++){
        if(letter == delimiters[z]){
            return 1;
        }
    }
    return 0;
}

static void wString_resetStrTok(wString *input){
    input->strtokStart = input->nextStrTok = 0;
    wString_restoreLength(input);
}



static int wString_strtok(wString *input, const WSTRING_TYPE *delimiters, int dimLen){
    input->strtokStart = input->nextStrTok;
    wString_restoreLength(input);

    //if next token starts on one of the delimiters, bump it up
    int character = input->array[input->strtokStart];
    while(_isDeliminator(character, delimiters, dimLen)){
        if(input->strtokStart < input->len)
            input->strtokStart++;
        else{
            wString_resetStrTok(input);
            return 0;
        }
        character = input->array[input->strtokStart];
    }


    //allocate enough memory to hold entire input string if necessary at first
    for(int i = input->strtokStart; i < input->len; i++){
        character = input->array[i];

        //exit when hit one of the delimiters
        if(_isDeliminator(character, delimiters, dimLen)){
            input->nextStrTok = (i+1);
            wString_setLength(input, i);
            return 1;
        }
    }
    wString_setLength(input, input->len);
    input->nextStrTok = 0;
    return 0;
}


static void wString_vsprintf(wString *string, const char *text, va_list *vl){
    int trueLen = 0;
    int stringLen = strlen(text);
    //wString_getLen(string);


    //printf("len: %d:%d\n", strlen(text), stringLen);
    for(int i = 0; i < stringLen; i++){
        if(text[i] == '%'){
            //safety check
            if(i + 1 >= stringLen){
                string->array[trueLen] = text[i];
                break;
            }

            //get the mode for output
            int mode = 0;
            if(text[i+1] >= '0' && text[i+1] <= '9'){
                i++;
                mode = text[i] - 48;
            }

            //found a formatted integer
            if(text[i+1] == 'd'){
                i++;
                int number = va_arg(*vl, int);
                trueLen += numToStr(&number, 0, &string->array[trueLen]);
                continue;
            }
            //found a boolean value
            else if(text[i+1] == 'b' || text[i+1] == 'B'){
                i++;
                char number = va_arg(*vl, int);

                switch(mode){
                    default:
                        //true/false, yes/no, on/off
                        trueLen += boolToStr(number, &string->array[trueLen], (text[i] == 'B'), boolTexts[mode]);
                    break;
                    //check mark with boxes
                    case MODE_CHECKBOX:
                        if(number)
                            string->array[trueLen++] = UNICODE_CHECKBOX;
                        else
                            string->array[trueLen++] = UNICODE_BOX;
                    break;
                    //just a checkmark
                    case MODE_CHECKMARK:
                        if(number)
                            string->array[trueLen++] = UNICODE_CHECKMARK;
                    break;
                }
                continue;
            }
            //found an ascii string!
            else if(text[i+1] == 's'){
                i++;
                char *input  = va_arg(*vl, char*);
                trueLen += copyCharToU16(&string->array[trueLen], wString_getLen(string) - trueLen, input);
                continue;
            }
            //found a wide string
            else if(text[i+1] == 'w'){
                i++;
                wString *input = va_arg(*vl, wString*);
                trueLen += wStringToU16(&string->array[trueLen], input);
                continue;
            }
            //convert input of bytes to other units
            else if(text[i+1] == '#'){
                i++;

                unsigned long long bytes = 0;
                unsigned int lowByte = va_arg(*vl, unsigned int);
                if(text[i+1] == '#'){//passing a long long value
                    i++;
                    unsigned int highByte = va_arg(*vl, unsigned int);
                    bytes = (unsigned long long) highByte << 32 | lowByte;
                }
                else
                    bytes = lowByte;


                double newSize = bytes;
                char unit[8];
                memset(unit, 0, 8);

                //get 2 decimal places
                if(bytes > SIZE_GB){
                    strcpy(unit, " GB\0");
                    newSize /= SIZE_GB;
                }
                if (bytes > SIZE_MB){
                    strcpy(unit, " MB\0");
                    newSize /= SIZE_MB;
                }
                else if (bytes > SIZE_KB){
                    strcpy(unit, " KB\0");
                    newSize /= SIZE_KB;
                }
                else{
                    strcpy(unit, " Bytes\0");
                    newSize = bytes;
                }

                char tempDigStr[64];
                memset(tempDigStr, 0, 64);
                sprintf(tempDigStr, "%0.2f %s", newSize, unit);
                trueLen += copyCharToU16(&string->array[trueLen], wString_getLen(string) - trueLen, tempDigStr);
                continue;
            }
            //convert to the right format for text color
            else if(text[i+1] == '*'){
                i++;
                int number = va_arg(*vl, int);
                char tempDigStr[16];
                memset(tempDigStr, 0, 16);
                sprintf(tempDigStr, "{^C%d}", number);
                trueLen += copyCharToU16(&string->array[trueLen], wString_getLen(string) - trueLen, tempDigStr);
                continue;
            }
        }
        //new line character
        else if(text[i] == '\\' && text[i+1] == 'n'){
            /*i++;
            string->array[trueLen] = '\n';
            continue;*/
            char tempStr[4];
            strcpy(tempStr,"\n\0");
            trueLen += copyCharToU16(&string->array[trueLen], wString_getLen(string) - trueLen, tempStr);
            i++;
            continue;//makes sure we haven't hit end of string
        }
        //regular characters
        //string->array[trueLen] = text[i];
        utf8decode(&text[i], &string->array[trueLen]);

        trueLen++;
    }


    //resize the final string
    if(!wString_realloc(string, trueLen + 1)){
        return;
    }

}

static void wString_sprintf(wString *string, const char *text, ...){
    //first lets make an approximate allocation of space for a workin buffer, it'll shrink after
    if(!wString_newAlloc(string, MAX_PATH<<1)){
        BAG_DBG_LibMsg("Error allocating wString!\n");
        return;
    }

    va_list vl;
    va_start(vl, text);
    wString_vsprintf(string, text, &vl);

    va_end(vl);

}

/*===============================================================================
String operations
===============================================================================*/
static inline int wString_atoiN(wString *string, int len){
    WSTRING_TYPE *buf = wString_getArray(string);

    WSTRING_TYPE *data = buf;

    int multiplier = 1;

    //check for negative input
    if(data[0] == '-'){
        multiplier = -1;
        data++;
        len--;
    }

    //get first number
    int total = 0;
    int curNumber = data[0] - 48;
    if(curNumber < 0 || curNumber > 9)
        return total;
    total += curNumber;
    //now that total has a value, we can applie if its negative or not
    total *= multiplier;

    //continue with the rest of the string
    for(int i = 1; i < len; i++){
        int curNumber = data[i] - 48;
        if(curNumber < 0 || curNumber > 9)
            return total;
        //number is legit
        total*=10;
        total += curNumber;
    }

    return total;
}

static inline int wString_atoi(wString *string){
    return wString_atoiN(string, wString_getLen(string));
}


static inline void wString_concat(wString *dest, wString *source){
    //need to resize the dest string to accomadate the new stuff
    int newLen = dest->len + wString_getLen(source) + 1;
    int start = dest->len;

    if(!wString_realloc(dest, newLen)){
        BAG_DBG_LibMsg("Error reallocating wString!\n");
        return;
    }

    //now to copy the data
    int sourceLen = wString_getLen(source);
    WSTRING_TYPE *srcBuf = wString_getArray(source);
    for(int i = 0; i < sourceLen; i++)
        dest->array[start + i] = srcBuf[i];

}

static inline void wString_charCat(wString *dest, WSTRING_TYPE letter){
    int oldLen = dest->len;
    if(!wString_realloc(dest, oldLen + 1)){
        BAG_DBG_LibMsg("Error reallocating wString!\n");
        return;
    }
    dest->array[oldLen-1] = letter;
}

static inline void wString_Ncopy(wString *dest, wString *source, int len){
    //reset and alloc the dest string
    if(!wString_newAlloc(dest, len)){
        BAG_DBG_LibMsg("Error copying wString!\n");
        return;
    }

    WSTRING_TYPE *srcBuf = wString_getArray(source);
    int tempLen = wString_getLen(source);
    //copy data from source to dest
    for(int i = 0; i < len && i < tempLen; i++)
        dest->array[i] = srcBuf[i];

}

static inline void wString_copy(wString *dest, wString *source){
    wString_Ncopy(dest, source, wString_getLen(source));
}

static inline int wString_Ncmp(wString *str1, wString *str2, int len){
    int i = 0;
    WSTRING_TYPE *buf1 = wString_getArray(str1);
    WSTRING_TYPE *buf2 = wString_getArray(str2);
    int len1 = wString_getLen(str1);
    int len2 = wString_getLen(str2);

    while((buf1[i] == buf2[i]) && i < len && i < len1 && i < len2)
        i++;

    if(i >= len)
        return 0;

    return buf1[i] - buf2[i];
}

static inline int wString_cmp(wString *str1, wString *str2){
    return wString_Ncmp(str1, str2, wString_getLen(str2));
}

static inline int wString_getWord(wString *str, int offset, wString *out){
    int i = offset, len = 0;
    int strLen = wString_getLen(str);
    WSTRING_TYPE *buf = wString_getArray(str);
    for(; i < strLen; i++, len++){
        if(buf[i] == ' ')break;
    }

    if(len > 0){
        wString_newAlloc(out, len);
        memcpy(out->array, &str->array[offset], len);
    }else
        wString_del(out);

    return len;
}

static inline void wString_toUpper(wString *str){
    int len = wString_getLen(str);
    WSTRING_TYPE *buf = wString_getArray(str);

    for(int i = 0; i < len; i++){
        if(buf[i] < 128)//only convert the compatable letters
            buf[i] = letterToBig(buf[i]);
    }
}

static inline void wString_toLower(wString *str){
    int len = wString_getLen(str);
    WSTRING_TYPE *buf = wString_getArray(str);

    for(int i = 0; i < len; i++){
        if(buf[i] < 128)//only convert the compatable letters
            buf[i] = letterToSmall(buf[i]);
    }
}



/*===============================================================================
Main Initializer
===============================================================================*/
//initializer
void wString_new(wString *string){
    memset(string, 0, sizeof(wString));
    string->len = 0;
    //initialize string to empty
    wString_del(string);

    string->string = (void*)&wString_getArray;
    string->length = (void*)&wString_getLen;

    string->del = (void*)&wString_del;
    string->new = (void*)&wString_newAlloc;

    #if defined(WSTRING_ASCII)
        string->delAscii = (void*)&wString_asciiDel;
        string->toAscii = (void*)&wString_toAscii;
    #endif

    string->concat = (void*)&wString_concat;
    string->charCat = (void*)&wString_charCat;
    string->Ncopy = (void*)&wString_Ncopy;
    string->copy = (void*)&wString_copy;
    string->Ncmp = (void*)&wString_Ncmp;
    string->cmp = (void*)&wString_cmp;
    string->getWord = (void*)&wString_getWord;
    string->atoiN = (void*)&wString_atoiN;
    string->atoi = (void*)&wString_atoi;

    string->toUpper = (void*)&wString_toUpper;
    string->toLower = (void*)&wString_toLower;

    string->resize = (void*)&wString_realloc;

    string->setLength = (void*)&wString_setLength;
    string->restoreLength = (void*)&wString_restoreLength;
    string->printf = (void*)&wString_sprintf;
    string->vprintf = (void*)&wString_vsprintf;
    string->strtok = (void*)&wString_strtok;
    string->resetTok = (void*)&wString_resetStrTok;

}
