#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <libBAG.h>

typedef struct Button_s {
  FNTObj_t *font;
  wString name;
  TextBox_t tb;
  int status;
} Button_t;

void Button_Create(int width, int height, const char *name, Button_t *button);
void Button_Draw(Button_t *button, int xpos, int ypos, unsigned short bgcolor, unsigned short fntcolor);
int Button_isTouched(Button_t *button);

#endif //__BUTTON_H_
