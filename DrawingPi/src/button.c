#include "button.h"

void Button_Create(int width, int height, const char *name, Button_t *button) {

  wString_new(&button->name);
  button->name.printf(&button->name, "%s", name);
  button->tb  = (TextBox_t) {
          0, 0,//x1, y1
          width, height,//x2, y2
          ALIGN_CENTER,//text alignment (ALIGN_LEFT, ALIGN_RIGHT, ALI
          WRAP_WORD,//text wrapping mode (WRAP_NONE, WRAP_LETTER, WRAP_WORD)
          TBOX_WHOLESTR,//print whole string, otherwise specify a numerical number of characters to print
  };
  button->status = 0;
}


void Button_Draw(Button_t *button, int xpos, int ypos, unsigned short bgcolor, unsigned short fntcolor) {

  int drawX2 = xpos + (button->tb.x2 - button->tb.x1),
          drawY2 = ypos + (button->tb.y2 - button->tb.y1);

  button->tb = (TextBox_t) {
          xpos, ypos,//x1, y1
          drawX2, drawY2,//x2, y2
          ALIGN_CENTER,//text alignment (ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
          WRAP_WORD,//text wrapping mode (WRAP_NONE, WRAP_LETTER, WRAP_WORD)
          TBOX_WHOLESTR,//print whole string, otherwise specify a numerical number of characters to print
  };

  BAG_Draw_Rect(BAG_GetScreen(), SCREEN_WIDTH, SCREEN_HEIGHT, xpos, ypos, drawX2, drawY2, bgcolor);
  
  unsigned short oldCol = (*BAG_Display_GetFontColor(button->font->image));
  BAG_Font_SetFontColor(button->font, fntcolor);
  BAG_Font_Print2(BAG_GetScreen(), &button->tb, &button->name, button->font);
  BAG_Font_SetFontColor(button->font, oldCol);
}

int Button_isTouched(Button_t *button) {
  return Stylus.Newpress && BAG_StylusZone(button->tb.x1, button->tb.y1, button->tb.x2, button->tb.y2);
}
