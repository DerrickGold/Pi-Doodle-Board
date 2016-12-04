//ds2_main.c

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libBAG.h>
#include <limits.h>
#include <time.h>
#include <syslog.h>
#include "button.h"
#include "undo.h"
//#include <system.h>

//#define FPRINTF(stream, fmt, ...) fprintf(stream, fmt, ##__VA_ARGS__)
#define FPRINTF(stream, fmt, ...) do {} while (0)

//Assuming This is located in "<MAGIC_MIRROR_DIR>/PluginDaemon/web/Plugins/Notes/DrawingPi"
#define SAVE_DIR "./images"
#define NOTES_LIST_FMT SAVE_DIR"/%s:%d%,%d%\n"
#define FRAME_RATE 60
#define DRAWCOLOR RGB15(31,63,31)
#define BGCOLOR RGB15(0,0,0)
#define FONTCOLOR RGB15(31, 63, 31)
#define BUTTON_BG_COL RGB15(0, 0, 15)
#define BUTTON_FG_COL RGB15(31, 63, 0)
#define UNDO_LEVELS 10

//shut backlight off after 120 seconds
#define SLEEP_AFTER 120


#define CURCOL_SQR_WD 28
#define CURCOL_SQR_HT 28
#define CURCOL_X 272
#define CURCOL_Y (SCREEN_HEIGHT - 30)

#define ICON_FRAME_WD 32
#define ICON_FRAME_HT 32

#define SET_TWITTER_ENV() do {                                          \
  setenv("TWITTER_API_KEY", getenv("TWITTER_API_KEY"), 1);              \
  setenv("TWITTER_API_SECRET", getenv("TWITTER_API_SECRET"), 1);        \
  setenv("TWITTER_TOKEN", getenv("TWITTER_TOKEN"), 1);                  \
  setenv("TWITTER_TOKEN_SECRET", getenv("TWITTER_TOKEN_SECRET"), 1);    \
  } while (0)


typedef enum PRGM_STATE {
  STATE_DRAWING,
  STATE_MENU,
  STATE_COLORWHEEL,
} PRGM_STATE;

typedef enum DRAW_STATE {
  DRAW_NORMAL,
  DRAW_ERASE,
  DRAW_FLOOD,
} DRAW_STATE;


UndoBuffer undos = {0};

//#define DO_BUFFER
static FNTObj_t ArialFont;

static GFXObj_t ToolBar = {0};
static GFXObj_t Drawing = {0};
static GFXObj_t MsgScreen = {0};
static GFXObj_t ColorWheel = {0};
static GFXObj_t Eraser = {0};
static GFXObj_t CurrentColSquare = {0};
static GFXObj_t Bucket = {0};
static GFXObj_t UndoBtn = {0};

static Button_t OKButton = {.font = &ArialFont};
static Button_t CancelButton = {.font = &ArialFont};

static char rootPath[PATH_MAX];
static char saveDir[PATH_MAX];




static void loadGfx(GFXObj_t *gfx, char *relpath) {
  char tempPathBuf[PATH_MAX];
  snprintf(tempPathBuf, PATH_MAX, "%s/%s", rootPath, relpath);
  if( BAG_Display_LoadObj(tempPathBuf, gfx) != ERR_NONE) {
    BAG_DBG_Msg("error loading %s\n", tempPathBuf);
    BAG_Exit(0);
  }
}

static void loadFnt(FNTObj_t *fnt, char *relpath) {
  char tempPathBuf[PATH_MAX];
  snprintf(tempPathBuf, PATH_MAX, "%s/%s", rootPath, relpath);
  if(BAG_Font_Load(tempPathBuf, &ArialFont) != ERR_NONE) {
    BAG_DBG_Msg("error loading %s\n", tempPathBuf);
    BAG_Exit(0);
  }
  BAG_Font_SetFontColor(&ArialFont, FONTCOLOR);
}

static void createMessage(char *message) {
  if (!MsgScreen.buffer.gfx)
    BAG_Display_CreateObj(&MsgScreen, 16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
  
  wString msgText;
  wString_new(&msgText);
  msgText.printf(&msgText, message);
  TextBox_t ScreenBox = {
    0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ALIGN_CENTER, WRAP_WORD, TBOX_WHOLESTR
  };
  BAG_Draw_Rect(MsgScreen.buffer.gfx, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RGB15(0,0,0));
  BAG_Font_Print2(MsgScreen.buffer.gfx, &ScreenBox, &msgText, &ArialFont);
  msgText.del(&msgText);
}

static void drawMessageScreen(void) {
  memcpy(BAG_GetScreen(), MsgScreen.buffer.gfx, SCREEN_SIZE);
}

static char loadResources(void) {
  loadFnt(&ArialFont, "graphics/arial_uni.fnt");
  //load graphics
  loadGfx(&ToolBar, "graphics/toolbar.bmp");
  loadGfx(&ColorWheel, "graphics/colorwheel.bmp");
  loadGfx(&Eraser, "graphics/eraser.png");
  BAG_Display_SetGfxFrameDim(&Eraser, ICON_FRAME_WD, ICON_FRAME_HT);
  loadGfx(&Bucket, "graphics/bucket.png");
  BAG_Display_SetGfxFrameDim(&Bucket, ICON_FRAME_WD, ICON_FRAME_HT);
  loadGfx(&UndoBtn, "graphics/undo.png");
  
  //disable using transparent color, 32bits have an alpha value to use
  BAG_Display_UseTransparentColor(&ColorWheel, 0);
  BAG_Display_CreateObj(&Drawing, 16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);

  //set up the currently selected color box
  BAG_Display_CreateObj(&CurrentColSquare, 16, CURCOL_SQR_WD, CURCOL_SQR_HT, CURCOL_SQR_WD, CURCOL_SQR_HT);
  BAG_Display_UseTransparentColor(&CurrentColSquare, 0);
  
  //make some buttons
  Button_Create(SCREEN_HEIGHT>>3, 40, "Yes", &OKButton);
  Button_Create(SCREEN_HEIGHT>>3, 40, "No", &CancelButton);
}



static char isGfxTouched(GFXObj_t *gfx) {
  int x = *BAG_Display_GetGfxBlitX(gfx),
			y = *BAG_Display_GetGfxBlitY(gfx);

  int x2 = x + *BAG_Display_GetGfxWidth(gfx),
    	y2 = y + *BAG_Display_GetGfxHeight(gfx);

  return Stylus.Newpress && BAG_StylusZone(x, y, x2, y2);
}

void StylusDraw(unsigned short *dest, int destWd, int destHt, unsigned short color, int draw_size) {
  static int oldX = 0, oldY = 0;
  static short oldDownTime = 0;

  if(Stylus.Newpress) {
    short low = (draw_size >> 1) - draw_size + 1;
    short high = (draw_size >> 1) + 1;

    int i = 0, j = 0;
    for(i = low; i < high; i++) {
      for(j=low; j < high; j++) {
	if((Stylus.X + i >= 0) && (Stylus.Y + j >= 0) && (Stylus.X + i < destWd) && (Stylus.Y + j < destHt))
	  BAG_Draw_BlitPixel(dest, destWd, destHt, Stylus.X + i, Stylus.Y + j, color);
      }
    }
  } else if (Stylus.Held) {
    if(oldDownTime != (Stylus.Downtime-1)) {
      oldX = Stylus.X;
      oldY = Stylus.Y;
    }

    BAG_Draw_BlitLineEx(dest, destWd, destHt, Stylus.X, Stylus.Y, oldX, oldY, color, draw_size);

  }
  oldX = Stylus.X;
  oldY = Stylus.Y;
  oldDownTime = Stylus.Downtime;
}

static char showToolBar(char show) {
  switch(show) {
  case 1:
    //show the tool bar
    memcpy(Drawing.buffer.gfx, BAG_GetScreen(), SCREEN_SIZE);
    break;
  case 0:
    //hide toolbar
    memcpy(BAG_GetScreen(), Drawing.buffer.gfx, SCREEN_SIZE);
    break;
  }
  return show;
}

static void drawCurColSqr(unsigned short color) {
  int wd = *BAG_Display_GetGfxWidth(&CurrentColSquare);
  int ht = *BAG_Display_GetGfxHeight(&CurrentColSquare);
  BAG_Draw_Rect(CurrentColSquare.buffer.gfx, wd, ht, 0, 0, wd, ht, RGB15(0,0,0));
  BAG_Draw_Rect(CurrentColSquare.buffer.gfx, wd, ht, 2, 2, wd - 2, ht - 2, color);
  BAG_Display_DrawObjFast(&CurrentColSquare, BAG_GetScreen(), CURCOL_X, CURCOL_Y);  
}

static void drawToolBar(DRAW_STATE curTool, unsigned short color) {
  //refresh toolbar
  BAG_Display_DrawObjFast(&ToolBar, BAG_GetScreen(), 0, SCREEN_HEIGHT - ToolBar.data.height);
  drawCurColSqr(color);

  int eraserX = *BAG_Display_GetGfxBlitX(&CurrentColSquare) - *BAG_Display_GetGfxFrameWd(&Eraser);
  BAG_Display_SetObjFrame(&Eraser, FRAME_VERT, curTool == DRAW_ERASE);
  BAG_Display_DrawObj(&Eraser, BAG_GetScreen(), eraserX, SCREEN_HEIGHT - ToolBar.data.height);

  int bucketX = *BAG_Display_GetGfxBlitX(&Eraser) - *BAG_Display_GetGfxFrameWd(&Bucket);
  BAG_Display_SetObjFrame(&Bucket, FRAME_VERT, curTool == DRAW_FLOOD);
  BAG_Display_DrawObj(&Bucket, BAG_GetScreen(), bucketX, SCREEN_HEIGHT - ToolBar.data.height);

  int undoX = *BAG_Display_GetGfxBlitX(&Bucket) - *BAG_Display_GetGfxFrameWd(&Bucket);
  BAG_Display_DrawObj(&UndoBtn, BAG_GetScreen(), undoX, SCREEN_HEIGHT - ToolBar.data.height);
}


static void clearScreen(void) {
  BAG_Update();
  BAG_ClearScreen(BGCOLOR);
  BAG_Update();
  BAG_ClearScreen(BGCOLOR);
  Undo_Add(&undos, BAG_GetScreen());
  showToolBar(1);
}

static int confirmationMenu(char *msg) {
  createMessage(msg);
  drawMessageScreen();
  Button_Draw(&OKButton, SCREEN_WIDTH>>2, SCREEN_HEIGHT/2 + (SCREEN_HEIGHT/4),
              BUTTON_BG_COL, BUTTON_FG_COL);
  Button_Draw(&CancelButton, SCREEN_WIDTH/2 + SCREEN_WIDTH/4, SCREEN_HEIGHT/2 + (SCREEN_HEIGHT/4),
              BUTTON_BG_COL, BUTTON_FG_COL);

  int status = 1;
  while (status == 1) {

    if (Button_isTouched(&OKButton))
      status = 0;
    else if (Button_isTouched(&CancelButton))
      status = -1;

    BAG_Update();
  }

  //make sure input is cleared
  BAG_Update();
  return status;
}

static void tweetNote(char *filename) {
  if (confirmationMenu("Would you like @PiDoodleBot to tweet this doodle?")) {
    showToolBar(0);
    return;
  }

  createMessage("Tweeting Doodle...");
  drawMessageScreen();
  
  char tempPathBuf[PATH_MAX];
  snprintf(tempPathBuf, PATH_MAX, "/usr/bin/python3 %s/tweet.py \"%s/%s.bmp\"", rootPath, saveDir, filename);
  syslog(LOG_INFO, "Command: %s\n", tempPathBuf);
  system(tempPathBuf);
  showToolBar(0);
}

static void saveNote(void) {
  //check with user first before posting
  if (confirmationMenu("Are you ready to share this doodle?")) {
    showToolBar(0);
    return;
  }
  
  createMessage("Posting Doodle...");
  drawMessageScreen();

  char savename[PATH_MAX];
  snprintf(savename, PATH_MAX, "%lu", (unsigned long)time(NULL));
  BAG_Display_GfxToBitmapFile(&Drawing, saveDir, savename);
  sleep(2);
  tweetNote(savename);
}

void setbacklight(int on) {
  char tempPathBuf[PATH_MAX];
  snprintf(tempPathBuf, PATH_MAX, "sudo %s/set_screen.sh \"%s\"", rootPath, (on) ? "on" : "0");
  syslog(LOG_INFO, "Command: %s\n", tempPathBuf);
  system(tempPathBuf);
}

static void floodfill(unsigned short *dest, int wd, int ht, int x, int y, unsigned short oldcol, unsigned short newcol) {

  if (oldcol == newcol || BAG_Draw_GetPixel(dest, wd, ht, x, y) != oldcol
      || x < 0 || x >= wd || y < 0 || y >= ht) 
    return;

  //dest[x + (y * SCREEN_WIDTH)] = newcol;
  BAG_Draw_BlitPixel(dest, wd, ht, x, y, newcol);
  floodfill(dest, wd, ht, x - 1, y, oldcol, newcol);
  floodfill(dest, wd, ht, x + 1, y, oldcol, newcol);
  floodfill(dest, wd, ht, x, y - 1, oldcol, newcol);
  floodfill(dest, wd, ht, x, y + 1, oldcol, newcol);
}


int main(int argc, char *argv[]){
  //get directory that this binary is running in
  readlink("/proc/self/exe", rootPath, PATH_MAX);
  char *dirEnd = strrchr(rootPath, '/');
  if (dirEnd) *dirEnd = '\0';

  if(!BAG_Init(1)){
    printf("Init error...\n");
    BAG_Exit(0);
  }
  SET_TWITTER_ENV();
  
  BAG_Core_SetFPS(FRAME_RATE);
  BAG_DBG_Init(NULL, DBG_ENABLE | DBG_LIB);

  
  Undo_Init(&undos, UNDO_LEVELS, (SCREEN_WIDTH * SCREEN_HEIGHT));
  
  loadResources();
  clearScreen();

  int colorCounter = 0, drawSize = 2;
  unsigned short curColor = DRAWCOLOR;
  int prgmState = STATE_MENU;
  DRAW_STATE drawTool = DRAW_NORMAL, lastTool = DRAW_NORMAL;
  int blankScreen = 0;
  time_t lastTouch = time(NULL);
  snprintf(saveDir, PATH_MAX, "%s/%s", rootPath, SAVE_DIR);
  
  while(1) {
    if (Stylus.Newpress || Stylus.Held) lastTouch = time(NULL);
    else if (!blankScreen && time(NULL) - lastTouch > SLEEP_AFTER) {
      syslog(LOG_INFO, "SLEEPING SCREEN");
      setbacklight(0);
      blankScreen = 1;
    } else if (blankScreen && time(NULL) - lastTouch < SLEEP_AFTER) {
      syslog(LOG_INFO, "WAKING SCREEN");
      setbacklight(1);
      blankScreen = 0;
    }

    switch(prgmState) {
      /*User is Currently Drawing...*/
    case STATE_DRAWING:

      switch(drawTool) {
      case DRAW_NORMAL:
        StylusDraw(BAG_GetScreen(), SCREEN_WIDTH, SCREEN_HEIGHT, curColor, drawSize);
        break;
      case DRAW_ERASE:
        StylusDraw(BAG_GetScreen(), SCREEN_WIDTH, SCREEN_HEIGHT, BGCOLOR, drawSize);
        break;
      case DRAW_FLOOD:{
          unsigned short *buf = BAG_GetScreen();
          unsigned short oldCol = buf[Stylus.X + (Stylus.Y * SCREEN_WIDTH)];
          floodfill(buf, SCREEN_WIDTH, SCREEN_HEIGHT, Stylus.X, Stylus.Y, oldCol, curColor);
      }break;
      }
      
      if(Stylus.Released) {
        Undo_Add(&undos, BAG_GetScreen());
        //copy image before it gets destroyed
        showToolBar(1);
        prgmState = STATE_MENU;
      }
      break;
      
      /*Stylus is off the screen and or re-entered in toolbar area*/
    case STATE_MENU:
      drawToolBar(drawTool, curColor);
      //user starts drawing again, hide toolbar
      if(Stylus.Newpress && !BAG_StylusZone(0, SCREEN_HEIGHT - ToolBar.data.height, SCREEN_WIDTH, SCREEN_HEIGHT)){
        showToolBar(0);
        prgmState = STATE_DRAWING;
      }
      //user hits save
      else if (Stylus.Newpress &&
               BAG_StylusZone(SCREEN_WIDTH - 74, SCREEN_HEIGHT - ToolBar.data.height, SCREEN_WIDTH, SCREEN_HEIGHT))
        {
          saveNote();
        }

      //user hits clear
      else if (Stylus.Newpress && BAG_StylusZone(0, SCREEN_HEIGHT - ToolBar.data.height, 74, SCREEN_HEIGHT)) {
        if (confirmationMenu("Clear the screen?\nThis action cannot be undone."))
          showToolBar(0);
        else
          clearScreen();
      }
      //user wants new color
      else if (isGfxTouched(&CurrentColSquare)) {
        prgmState = STATE_COLORWHEEL;
      }
      else if (isGfxTouched(&Eraser)) {
        drawTool = (drawTool == DRAW_ERASE) ? DRAW_NORMAL: DRAW_ERASE;
      }
      else if (isGfxTouched(&Bucket)) {
        drawTool = (drawTool == DRAW_FLOOD) ? DRAW_NORMAL: DRAW_FLOOD;
      }
      else if (isGfxTouched(&UndoBtn)) {
        Undo_Revert(BAG_GetScreen(), &undos);
        showToolBar(1);
      }
      break;
      /* User is selecting a color from the color wheel*/
    case STATE_COLORWHEEL:
      if(!colorCounter){
        BAG_Display_DrawObj(&ColorWheel, BAG_GetScreen(), (SCREEN_WIDTH - ColorWheel.data.width)>>1, 0);
        colorCounter++;
      }

      if ((Stylus.Newpress || Stylus.Held) &&
          BAG_StylusZone(ColorWheel.blitX, ColorWheel.blitY, ColorWheel.blitX +
                         ColorWheel.data.width, ColorWheel.blitY + ColorWheel.data.height))
        {
          int posX = Stylus.X - ColorWheel.blitX,
            posY = Stylus.Y - ColorWheel.blitY;

          unsigned short tempCol = *BAG_Display_GetGfxPixelCol(&ColorWheel, posX, posY);
          unsigned short alpha = *BAG_Display_GetGfxAlphaPix(&ColorWheel, posX, posY);
          if(alpha > 0) {
            curColor = tempCol;
            drawToolBar(drawTool, curColor);
          }
        }

      else if (Stylus.Newpress && BAG_StylusZone(271, SCREEN_HEIGHT - ToolBar.data.height, 292, SCREEN_HEIGHT)) {
        prgmState = STATE_MENU;
        colorCounter = 0;
        showToolBar(0);
        showToolBar(1);
      }
      break;
      
    }
    BAG_Update();
  } //while

  BAG_ClearScreen(0);
  BAG_Update();
  BAG_Exit(0);
  return 0;
}
