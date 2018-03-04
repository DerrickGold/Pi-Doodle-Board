#include "keyboard.h"

const unsigned char _qwerty[2][KB_HEIGHT][KB_WIDTH]={
    {
        {'1','2','3','4','5','6','7','8','9','0','-','='},
        {'q','w','e','r','t','y','u','i','o','p',NO_KEY,NO_KEY},
        {'a','s','d','f','g','h','j','k','l',NO_KEY,NO_KEY,NO_KEY},
        {NO_KEY,'z','x','c','v','b','n','m',',','.','/','\\'},
        {NO_KEY,';','\'',NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,'[',']',NO_KEY},
    },
    {   
        {'!','@','#','$','%','^','&','*','(',')','_','+'},
        {'Q','W','E','R','T','Y','U','I','O','P',NO_KEY,NO_KEY},
        {'A','S','D','F','G','H','J','K','L',NO_KEY,NO_KEY,NO_KEY},
        {NO_KEY,'Z','X','C','V','B','N','M','<','>','?','|'},
        {NO_KEY,':','"',NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,NO_KEY,'{','}',NO_KEY},
    },
};
const unsigned char _numpad[NUM_HEIGHT][NUM_WIDTH]={
    {'7','8','9'},
    {'4','5','6'},
    {'1','2','3'},
    {'0',NO_KEY,NO_KEY},
};


/*================================================================================================================================
Initialization
================================================================================================================================*/
static void keyboard_setCasing(keyboard_t *kb, char caps){
    caps = (caps>1) ? 1:caps;
    caps = (caps<0) ? 0:caps;
    for(int y = 0; y < KB_HEIGHT; y++){
        for(int x = 0; x < KB_WIDTH; x++){
            kb->kbMap[y][x] = (unsigned char*)&_qwerty[caps][y][x];
        }
    }
}

static void keyboard_setNumPad(keyboard_t *kb){
    for(int y = 0; y < NUM_HEIGHT; y++){
        for(int x = 0; x < NUM_WIDTH; x++){
            kb->numMap[y][x] = (unsigned char*)&_numpad[y][x];
        }
    }
}

static void prepKeysKb(keyboard_t *kb){
    //set key positions
    int y = 0, x = 0;
    for(y = 0; y < KB_HEIGHT; y++){
        for(x = 0; x < KB_WIDTH; x++){
            if(*kb->kbMap[y][x] == NO_KEY)
                continue;

            kb->Letters[y][x].y1 = kb->yPos + (kb->tileHt * y);
            kb->Letters[y][x].y2 = kb->yPos + (kb->tileHt * y) + kb->tileHt;

            switch(y){
                case 0:
                case 1:
                    kb->Letters[y][x].x1 = kb->xPos + (kb->tileWd * x);
                    kb->Letters[y][x].x2 = kb->xPos + (kb->tileWd * x) + kb->tileWd;
                break;
                case 2:
                case 3:
                    kb->Letters[y][x].x1 = kb->xPos + (kb->tileWd * x) + (kb->tileWd >> 1);
                    kb->Letters[y][x].x2 = kb->xPos + (kb->tileWd * x) + (kb->tileWd >> 1) + kb->tileWd;
                break;
                case 4:
                    kb->Letters[y][x].x1 = kb->xPos + (kb->tileWd * x) + kb->tileWd;
                    kb->Letters[y][x].x2 = kb->xPos + (kb->tileWd * x) + (kb->tileWd << 1);
                break;
            }
        }
    }

    //special key positions
    //space bar
    kb->Extra[Key_Space].x1 = kb->xPos + (kb->tileWd*4);
    kb->Extra[Key_Space].y1 = kb->yPos + (kb->tileHt*4);
    kb->Extra[Key_Space].x2 = kb->xPos + (kb->tileWd*10);
    kb->Extra[Key_Space].y2 = kb->yPos + (kb->tileHt*5);

    //enter key
    kb->Extra[Key_Enter].x1 = kb->xPos + (kb->tileWd*9) + (kb->tileWd>>1);
    kb->Extra[Key_Enter].y1 = kb->yPos + (kb->tileHt*2);
    kb->Extra[Key_Enter].x2 = kb->xPos + (kb->tileWd*12);
    kb->Extra[Key_Enter].y2 = kb->yPos + (kb->tileHt*3);

    //backspace key
    kb->Extra[Key_Back].x1 = kb->xPos + (kb->tileWd*10);
    kb->Extra[Key_Back].y1 = kb->yPos + (kb->tileHt);
    kb->Extra[Key_Back].x2 = kb->xPos + (kb->tileWd*12);
    kb->Extra[Key_Back].y2 = kb->yPos + (kb->tileHt*2);

    //tab key
    kb->Extra[Key_Tab].x1 = kb->xPos;
    kb->Extra[Key_Tab].y1 = kb->yPos + (kb->tileHt*2);
    kb->Extra[Key_Tab].x2 = kb->xPos + (kb->tileWd>>1);
    kb->Extra[Key_Tab].y2 = kb->yPos + (kb->tileHt*3);

    //shift key
    kb->Extra[Key_Shift].x1 = kb->xPos;
    kb->Extra[Key_Shift].y1 = kb->yPos + (kb->tileHt*3);
    kb->Extra[Key_Shift].x2 = kb->xPos + (kb->tileWd + (kb->tileWd>>1));
    kb->Extra[Key_Shift].y2 = kb->yPos + (kb->tileHt*4);

    //caps
    kb->Extra[Key_Caps].x1 = kb->xPos;
    kb->Extra[Key_Caps].y1 = kb->yPos + (kb->tileHt*4);
    kb->Extra[Key_Caps].x2 = kb->xPos + (kb->tileWd*2);
    kb->Extra[Key_Caps].y2 = kb->yPos + (kb->tileHt*5);
}


void Keyboard_SetXY(keyboard_t *kb, int x, int y){
    kb->xPos = x;
    kb->yPos = y;
    if(!kb->mode)//is keyboard
        prepKeysKb(kb);
}

void Keyboard_TileSize(keyboard_t *kb, int size){
    kb->tileWd = kb->tileHt = size;
}

/*================================================================================================================================
key drawing functions
================================================================================================================================*/
static void keyboard_getKeyCol(keyboard_t *kb, char letter, unsigned short *keyCol, unsigned short *letterCol, char force){
    if(kb->pressed == letter || force){
        *keyCol = kb->Colors.keyp;
        *letterCol = kb->Colors.letterp;
        return;
    }

    *keyCol = kb->Colors.key;
    *letterCol = kb->Colors.letter;   
}


static void keyboard_drawKey(unsigned short *dest, int destwd, int destht, keyboard_t *kb, struct _key *input, char letter){
    short wd = kb->tileWd;
    //draw key box
    if(letter == '\\' || letter == '|'){
        wd=(wd>>1);
    }

    unsigned short tempKeyCol = 0, tempLetterCol = 0;
    keyboard_getKeyCol(kb, letter, &tempKeyCol, &tempLetterCol, 0);

    
    //horizontal lines
    BAG_Draw_BlitLine(dest, destwd, destht, input->x1, input->y1, input->x1 + wd, input->y1, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, input->x1, input->y1 + kb->tileHt, input->x1 + wd, input->y1 + kb->tileHt, tempKeyCol);

    //vertical lines
    BAG_Draw_BlitLine(dest, destwd, destht, input->x1, input->y1, input->x1, input->y1 + kb->tileHt, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, input->x1 + wd, input->y1, input->x1 + wd, input->y1 + kb->tileHt, tempKeyCol);

    int x = input->x1 + ((wd - BAG_Font_GetCharWd(kb->font, letter))>>1),
        y = input->y1 + ((abs(input->y1-input->y2) - BAG_Font_GetCharHt(kb->font))>>1);

    //BAG_Font_DrawCharacter(dest, destwd, destht, kb->font, &letter, x, y, input->x2, input->y2, tempLetterCol);    
    BAG_Font_SetFontColor(kb->font, tempLetterCol);
    BAG_Font_PrintChar(kb->font, letter, x, y, dest, destwd, destht);
}

static void keyboard_drawShift(unsigned short *dest, int destwd, int destht, keyboard_t *kb, struct _key *input, char letter){
    int x1 = input->x1, x2 = input->x2, y1 = input->y1, y2 = input->y2;

    unsigned short tempKeyCol = 0, tempLetterCol = 0;
    keyboard_getKeyCol(kb, letter, &tempKeyCol, &tempLetterCol, (kb->shift>1));


    //hor lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x2, y1, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y2, x2, y2, tempKeyCol);

    //vert lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1, y2, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x2, y1, x2, y2, tempKeyCol);

    
    int arrowY= y1 + (kb->tileHt>>2);
    int arrowX= x1 + (kb->tileWd>>2)+4;
    //draw shift arrow
    BAG_Draw_BlitLine(dest, destwd, destht, 
                        arrowX, arrowY + (kb->tileHt>>1), 
                        arrowX + (kb->tileWd>>1), arrowY + (kb->tileHt>>1), tempLetterCol);//bottom

     BAG_Draw_BlitLine(dest, destwd, destht, 
                        arrowX, arrowY + (kb->tileHt>>1), 
                        arrowX, arrowY + (kb->tileHt>>2), tempLetterCol);//left line   

    BAG_Draw_BlitLine(dest, destwd, destht, 
                        arrowX + (kb->tileWd>>1), arrowY + (kb->tileHt>>1), 
                        arrowX + (kb->tileWd>>1), arrowY + (kb->tileHt>>2), tempLetterCol);//right line

    BAG_Draw_BlitLine(dest, destwd, destht, 
                        arrowX-2, arrowY + (kb->tileHt>>2), 
                        arrowX + (kb->tileWd>>2), arrowY + (kb->tileHt>>2)-3, tempLetterCol);//left slant

    BAG_Draw_BlitLine(dest, destwd, destht, 
                        arrowX + (kb->tileWd>>1)+2, arrowY + (kb->tileHt>>2), 
                        arrowX + (kb->tileWd>>2), arrowY + (kb->tileHt>>2)-3, tempLetterCol);//bottom
}


static void keyboard_drawEnter(unsigned short *dest, int destwd, int destht, keyboard_t *kb, struct _key *input, char letter){
    int x1 = input->x1, /*x2 = input->x2,*/ y1 = input->y1/*, y2 = input->y2*/;

    unsigned short tempKeyCol = 0, tempLetterCol = 0;
    keyboard_getKeyCol(kb, letter, &tempKeyCol, &tempLetterCol, 0);


    //hor lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1 + (kb->tileWd<<1) + (kb->tileWd>>1), y1, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1 + kb->tileHt, x1 + (kb->tileWd<<1) + (kb->tileWd>>1), y1 +kb->tileHt, tempKeyCol);

    //vert lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1, y1 + kb->tileHt, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1 + (kb->tileWd<<1) + (kb->tileWd>>1), y1, 
                                            x1 + (kb->tileWd<<1) + (kb->tileWd>>1), y1 + kb->tileHt, tempKeyCol);

    //draw return arrow
    int Ax= x1 + (kb->tileWd<<1)-(kb->tileWd>>1);
    //down line
    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax, y1 + (kb->tileHt>>1) - 4,
                        Ax, y1 + (kb->tileHt>>1)-2, tempLetterCol);
    //horizontal line
    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax, y1 + (kb->tileHt>>1) - 1,
                        Ax - (kb->tileWd>>1), y1 + (kb->tileHt>>1)-1, tempLetterCol);

    //arrow tip
    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax - (kb->tileWd>>1)-(kb->tileWd>>2), y1 + (kb->tileHt>>1),
                        Ax - (kb->tileWd>>1), y1 + (kb->tileHt>>1)-3, tempLetterCol);

    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax - (kb->tileWd>>1)-(kb->tileWd>>2), y1 + (kb->tileHt>>1) - 1,
                        Ax - (kb->tileWd>>1), y1 + (kb->tileHt>>1)+2, tempLetterCol);
}

static void keyboard_drawSpace(unsigned short *dest, int destwd, int destht, keyboard_t *kb, struct _key *input, char letter){
    int x1 = input->x1,/* x2 = input->x2,*/ y1 = input->y1/*, y2 = input->y2*/;

    unsigned short tempKeyCol = 0, tempLetterCol = 0;
    keyboard_getKeyCol(kb, letter, &tempKeyCol, &tempLetterCol, 0);


    //hor lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1 + (kb->tileWd*6), y1, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1 + kb->tileHt, x1 + (kb->tileWd*6), y1 + kb->tileHt, tempKeyCol);

    //vert lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1, y1 + kb->tileHt, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1 + (kb->tileWd*6), y1, x1 + (kb->tileWd*6), y1 + kb->tileHt, tempKeyCol);
}

static void keyboard_drawBackSpace(unsigned short *dest, int destwd, int destht, keyboard_t *kb, struct _key *input, char letter){
    int x1 = input->x1,/* x2 = input->x2,*/ y1 = input->y1/*, y2 = input->y2*/;

    unsigned short tempKeyCol = 0, tempLetterCol = 0;
    keyboard_getKeyCol(kb, letter, &tempKeyCol, &tempLetterCol, 0);


    int wd= kb->tileWd<<1;

    //key outline
    //horizontal lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1 + wd, y1, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1 + kb->tileHt, x1 + wd, y1 + kb->tileHt, tempKeyCol);

    //vertical lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1, y1 + kb->tileHt, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1 + wd, y1, x1 + wd, y1 + kb->tileHt, tempKeyCol);

    //backspace arrow
    int Ax = x1 + (kb->tileWd)+(kb->tileWd>>2);
    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax + (kb->tileWd>>2), y1 + (kb->tileHt>>1)-1, 
                        Ax - (kb->tileWd>>2), y1 + (kb->tileHt>>1)-1, tempLetterCol);//mid line

    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax - (kb->tileWd>>1), y1 + (kb->tileHt>>1), 
                        Ax - (kb->tileWd>>2), y1 + (kb->tileHt>>1)-3, tempLetterCol);

    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax - (kb->tileWd>>1), y1 + (kb->tileHt>>1)-1, 
                        Ax - (kb->tileWd>>2), y1 + (kb->tileHt>>1)+2, tempLetterCol);
}

static void keyboard_drawTab(unsigned short *dest, int destwd, int destht, keyboard_t *kb, struct _key *input, char letter){
    int x1 = input->x1,/* x2 = input->x2,*/ y1 = input->y1/*, y2 = input->y2*/;

    unsigned short tempKeyCol = 0, tempLetterCol = 0;
    keyboard_getKeyCol(kb, letter, &tempKeyCol, &tempLetterCol, 0);


    int wd= kb->tileWd>>1;
    //key outline
    //horizontal lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1 + wd, y1, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1 + kb->tileHt, x1 + wd, y1 + kb->tileHt, tempKeyCol);

    //vertical lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1, y1 + kb->tileHt, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1 + wd, y1, x1 + wd, y1 + kb->tileHt, tempKeyCol);

    //backspace arrow
    int Ax= x1 + 1;
    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax, y1 + (kb->tileHt>>1)-1, 
                        Ax + (kb->tileWd>>2), y1 + (kb->tileHt>>1)-1, tempLetterCol);//mid line

    BAG_Draw_BlitLine(dest, destwd, destht, 
                    Ax + (kb->tileWd>>3), y1 + (kb->tileHt>>1)-3, 
                    Ax + (kb->tileWd>>2), y1 + (kb->tileHt>>1), tempLetterCol);

    BAG_Draw_BlitLine(dest, destwd, destht, 
                        Ax + (kb->tileWd>>3), y1 + (kb->tileHt>>1)+2, 
                        Ax + (kb->tileWd>>2), y1 + (kb->tileHt>>1)-1, tempLetterCol); 
}


static void keyboard_drawCapsLock(unsigned short *dest, int destwd, int destht, keyboard_t *kb, struct _key *input, char letter){
    int x1 = input->x1,/* x2 = input->x2,*/ y1 = input->y1/*, y2 = input->y2*/;

    unsigned short tempKeyCol = 0, tempLetterCol = 0;
    keyboard_getKeyCol(kb, letter, &tempKeyCol, &tempLetterCol, kb->capslock);

    int wd = kb->tileWd<<1;

    //key outline
    //horizontal lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1 + wd, y1, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1 + kb->tileHt, x1 + wd, y1 + kb->tileHt, tempKeyCol);

    //vertical lines
    BAG_Draw_BlitLine(dest, destwd, destht, x1, y1, x1, y1 + kb->tileHt, tempKeyCol);
    BAG_Draw_BlitLine(dest, destwd, destht, x1 + wd, y1, x1 + wd, y1 + kb->tileHt, tempKeyCol);

    int len = BAG_Font_GetCharWd(kb->font, 'c') + BAG_Font_GetCharWd(kb->font, 'a') + 
              BAG_Font_GetCharWd(kb->font, 'p') + BAG_Font_GetCharWd(kb->font, 's');

    int x = input->x1 + ((wd - len)>>1),
        y = input->y1 + ((abs(y1-(y1 + kb->tileHt)) - BAG_Font_GetCharHt(kb->font))>>1);

    //BAG_Font_DrawStringEx(dest, destwd, destht, kb->font, "caps", x, y, 256, 192, -1, tempLetterCol);
}

void Keyboard_Draw(unsigned short *dest, int destwd, int destht, keyboard_t *kb){
    for(int y = 0; y < KB_HEIGHT; y++){
        for(int x = 0; x < KB_WIDTH; x++){
            if(*kb->kbMap[y][x] == NO_KEY)
                continue;
            keyboard_drawKey(dest, destwd, destht, kb, &kb->Letters[y][x], (char)*kb->kbMap[y][x]);
        }
    }
    keyboard_drawEnter(dest, destwd, destht, kb, &kb->Extra[Key_Enter], ENTER);
    keyboard_drawSpace(dest, destwd, destht, kb, &kb->Extra[Key_Space], SPACE);
    keyboard_drawBackSpace(dest, destwd, destht, kb, &kb->Extra[Key_Back], BACKSPACE);
    keyboard_drawTab(dest, destwd, destht, kb, &kb->Extra[Key_Tab], TAB);
    keyboard_drawCapsLock(dest, destwd, destht, kb, &kb->Extra[Key_Caps], CAPS);
    keyboard_drawShift(dest, destwd, destht, kb, &kb->Extra[Key_Shift], SHIFT);

    //redraw character key for full key to be drawn uninterrupted
    switch(kb->pressed){
        case ENTER: keyboard_drawEnter(dest, destwd, destht, kb, &kb->Extra[Key_Enter], ENTER); break;
        case SPACE: keyboard_drawSpace(dest, destwd, destht, kb, &kb->Extra[Key_Space], SPACE); break;
        case BACKSPACE: keyboard_drawBackSpace(dest, destwd, destht, kb, &kb->Extra[Key_Back], BACKSPACE); break;
        case TAB: keyboard_drawTab(dest, destwd, destht, kb, &kb->Extra[Key_Tab], TAB); break;
        default:
            if(kb->shift > 1)
                keyboard_drawShift(dest, destwd, destht, kb, &kb->Extra[Key_Shift], SHIFT);

            if(kb->capslock)
                keyboard_drawCapsLock(dest, destwd, destht, kb, &kb->Extra[Key_Caps], CAPS);

            for(int y = 0; y < KB_HEIGHT; y++){
                for(int x = 0; x < KB_WIDTH; x++){
                    if(*kb->kbMap[y][x] == NO_KEY || kb->pressed != *kb->kbMap[y][x])
                        continue;
                    keyboard_drawKey(dest, destwd, destht, kb, &kb->Letters[y][x], (char)*kb->kbMap[y][x]);
                }
            }  
        break;
    }
    keyboard_setCasing(kb, kb->shift);
}

/*================================================================================================================================
keyboard updating stuff
================================================================================================================================*/
char Keyboard_Check(keyboard_t * kb){
    //normal keys
    for(int y=0;y<KB_HEIGHT;y++){
        for(int x=0;x<KB_WIDTH;x++){
            if(BAG_Input_StylusZone(kb->Letters[y][x].x1, kb->Letters[y][x].y1, kb->Letters[y][x].x2, kb->Letters[y][x].y2)){
                keyboard_setCasing(kb, kb->shift);
                kb->pressed = *kb->kbMap[y][x];

                if(!kb->capslock && kb->shift) kb->shift = 0;
                else if(kb->capslock && !kb->shift)kb->shift = 1;
                return kb->pressed;
            }
        }
    }

    //shift key
    if(BAG_Input_StylusZone(kb->Extra[Key_Shift].x1, kb->Extra[Key_Shift].y1, kb->Extra[Key_Shift].x2, kb->Extra[Key_Shift].y2)){
        kb->pressed = SHIFT;
        kb->shift = (kb->shift > 0) ? 0: 2;
        keyboard_setCasing(kb, kb->shift);
        return SHIFT;
    }
    
    //enter key
    if(BAG_Input_StylusZone(kb->Extra[Key_Enter].x1, kb->Extra[Key_Enter].y1, kb->Extra[Key_Enter].x2, kb->Extra[Key_Enter].y2)){
        kb->pressed = ENTER;
        return ENTER;
    }
    //space key
    if(BAG_Input_StylusZone(kb->Extra[Key_Space].x1, kb->Extra[Key_Space].y1, kb->Extra[Key_Space].x2, kb->Extra[Key_Space].y2)){
        kb->pressed = SPACE;
        return ' ';
    }   
    //backspace key
    if(BAG_Input_StylusZone(kb->Extra[Key_Back].x1, kb->Extra[Key_Back].y1, kb->Extra[Key_Back].x2, kb->Extra[Key_Back].y2)){
        kb->pressed = BACKSPACE;
        return BACKSPACE;
    }
    //tab key
    if(BAG_Input_StylusZone(kb->Extra[Key_Tab].x1, kb->Extra[Key_Tab].y1, kb->Extra[Key_Tab].x2, kb->Extra[Key_Tab].y2)){
        kb->pressed = TAB;
        return TAB;
    }
    //caps key
    if(BAG_Input_StylusZone(kb->Extra[Key_Caps].x1, kb->Extra[Key_Caps].y1, kb->Extra[Key_Caps].x2, kb->Extra[Key_Caps].y2)){
        kb->capslock++;
        kb->shift++;
        if(kb->shift > 1){
            kb->shift = 0;
            kb->capslock = 0;
        }
        keyboard_setCasing(kb, kb->shift);
        kb->pressed = CAPS;
        return CAPS;
    }

    kb->pressed = -1;
    return 0;
}

void Keyboard_Init(keyboard_t *kb, FNTObj_t *fnt, unsigned short key, unsigned short letter, unsigned short keyp, unsigned short letterp){
    //set up the font for the keyboard
    kb->font = fnt;
    kb->tileWd = 16;
    kb->tileHt = 16;

    //set up keyboard mappings
    keyboard_setCasing(kb, 0);
    keyboard_setNumPad(kb);
    kb->mode = 0;

    //set kb colors
    kb->Colors.key = key;
    kb->Colors.letter = letter;
    kb->Colors.keyp = keyp;
    kb->Colors.letterp = letterp;

    kb->pressed = -1;
    prepKeysKb(kb);

    /*    kb->setXY = (void*)&keyboard_setXY;
    kb->draw = (void*)*keyboard_draw;
    kb->check = (void*)&keyboard_check;
    kb->size = (void*)&keyboard_tileSize;
    */
}


