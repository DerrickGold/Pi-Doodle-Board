#ifndef __BAG_KEYBOARD_H__
#define __BAG_KEYBOARD_H__

#include <libBAG.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KB_HEIGHT 5
#define KB_WIDTH 12
#define NUM_HEIGHT 4
#define NUM_WIDTH 3

typedef enum{
    KB_BIG = 20,
    KB_NORM = 16,
    KB_SMALL = 12,
}KEYBOARD_SIZES;

typedef enum{
    SPACE, BACKSPACE, CAPS, TAB, SHIFT, NO_KEY = '~', ENTER = '\n',
}KEYVALUES;

typedef enum{
    Key_Shift, Key_Enter, Key_Space, Key_Back, Key_Tab, Key_Caps, Key_Max,
}_BAG_Extra_Keys;

struct _key{
    short x1, y1, x2, y2;
};

struct _colors{
    unsigned short key, letter, keyp, letterp;
};


typedef struct keyboard_s{
    FNTObj_t *font;
    short tileWd, tileHt;
    unsigned char *kbMap[KB_HEIGHT][KB_WIDTH];//key mappings
    unsigned char *numMap[NUM_HEIGHT][NUM_WIDTH];

    unsigned char pressed, mode, capslock, shift;//letter that was pressed

    short xPos, yPos;
    struct _key Letters[KB_HEIGHT][KB_WIDTH];
    struct _key Extra[Key_Max];//extra keys like space, tab, enter etc.
    struct _colors Colors;

    void (*size) (struct keyboard_s *, int);
    void (*setXY)(struct keyboard_s*, int, int);
    void (*draw)(unsigned short *, int, int, struct keyboard_s *);
    char (*check)(struct keyboard_s *);

} keyboard_t;

void Keyboard_SetXY(keyboard_t *kb, int x, int y);
void Keyboard_Draw(unsigned short *dest, int destwd, int destht, keyboard_t *kb);
void Keyboard_Init(keyboard_t *kb, FNTObj_t *fnt, unsigned short key, unsigned short letter, unsigned short keyp, unsigned short letterp);
char Keyboard_Check(keyboard_t * kb);
void Keyboard_TileSize(keyboard_t *kb, int size);

#ifdef __cplusplus
}
#endif


#endif
