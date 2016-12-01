//ds2_main.c

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libBAG.h>
#include <limits.h>
#include <time.h>
#include <syslog.h>
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
//shut backlight off after 120 seconds
#define SLEEP_AFTER 120


typedef enum PRGM_STATE {
  STATE_DRAWING,
  STATE_MENU,
  STATE_COLORWHEEL,
} PRGM_STATE;

//#define DO_BUFFER
static FNTObj_t ArialFont;

static GFXObj_t ToolBar = {0};
static GFXObj_t Drawing = {0};
static GFXObj_t SaveScreen = {0};
static GFXObj_t ColorWheel = {0};
static GFXObj_t placeButton = {0};
static GFXObj_t cancelButton = {0};
static GFXObj_t eraser = {0};

static char rootPath[PATH_MAX];
static char saveDir[PATH_MAX];




static char isGfxTouched(GFXObj_t *gfx) {
  int x = *BAG_Display_GetGfxBlitX(gfx),
			y = *BAG_Display_GetGfxBlitY(gfx);

  int x2 = x + *BAG_Display_GetGfxWidth(gfx),
    	y2 = y + *BAG_Display_GetGfxHeight(gfx);

  return BAG_StylusZone(x, y, x2, y2);
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
    //    BAG_Update();BAG_Update();
    break;
  case 0:
    //hide toolbar
    memcpy(BAG_GetScreen(), Drawing.buffer.gfx, SCREEN_SIZE);
    //BAG_Update();BAG_Update();
    break;
  }

  return show;
}


static void drawToolBar(unsigned short color) {
  //update color on tool bar
  BAG_Draw_Rect(ToolBar.buffer.gfx, ToolBar.data.width, ToolBar.data.height, 272, 6, 293, 26, color);
  //refresh toolbar
  BAG_Display_DrawObjFast(&ToolBar, BAG_GetScreen(), 0, SCREEN_HEIGHT - ToolBar.data.height);
  BAG_Display_DrawObj(&eraser, BAG_GetScreen(), 272 - 40, SCREEN_HEIGHT - ToolBar.data.height + 6);
}


static void clearScreen(void) {

  BAG_Update();
  BAG_ClearScreen(BGCOLOR);
  BAG_Update();
  BAG_ClearScreen(BGCOLOR);
  showToolBar(1);
}


static void saveNote(void) {
  memcpy(BAG_GetScreen(), SaveScreen.buffer.gfx, SCREEN_SIZE);
  char savename[PATH_MAX];
  snprintf(savename, PATH_MAX, "%lu", (unsigned long)time(NULL));
  BAG_Display_GfxToBitmapFile(&Drawing, saveDir, savename);
  sleep(2);
  showToolBar(0);
}


void loadGfx(GFXObj_t *gfx, char *relpath) {
  char tempPathBuf[PATH_MAX];
  snprintf(tempPathBuf, PATH_MAX, "%s/%s", rootPath, relpath);
  if( BAG_Display_LoadObj(tempPathBuf, gfx) != ERR_NONE) {
    BAG_DBG_Msg("error loading %s\n", tempPathBuf);
    BAG_Exit(0);
  }
}

void loadFnt(FNTObj_t *fnt, char *relpath) {
  char tempPathBuf[PATH_MAX];
  snprintf(tempPathBuf, PATH_MAX, "%s/%s", rootPath, relpath);
  if(BAG_Font_Load(tempPathBuf, &ArialFont) != ERR_NONE) {
    BAG_DBG_Msg("error loading %s\n", tempPathBuf);
    BAG_Exit(0);
  }
  BAG_Font_SetFontColor(&ArialFont, FONTCOLOR);
}
  

void setbacklight(int on) {
  char tempPathBuf[PATH_MAX];
  snprintf(tempPathBuf, PATH_MAX, "sudo %s/set_screen.sh \"%s\"", rootPath, (on) ? "on" : "0");
  syslog(LOG_INFO, "Command: %s\n", tempPathBuf);
  system(tempPathBuf);
  /*  FILE *f = fopen("/sys/class/backlight/soc\\:backlight/brightness", "r");
  if (!f) {
    syslog(LOG_INFO, "error opening backlight file");
    return;
  }

  if (on) fprintf(f, "1");
  else fprintf(f, "0");
  fclose(f);*/

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


  char tempPathBuf[PATH_MAX];
  BAG_Core_SetFPS(FRAME_RATE);

  BAG_DBG_Init(NULL, DBG_ENABLE | DBG_LIB);
  loadFnt(&ArialFont, "graphics/arial_uni.fnt");
  
  //load graphics
  loadGfx(&ToolBar, "graphics/toolbar.bmp");
  loadGfx(&ColorWheel, "graphics/colorwheel.bmp");
  loadGfx(&eraser, "graphics/eraser.png");

  //disable using transparent color, 32bits have an alpha value to use
  BAG_Display_UseTransparentColor(&ColorWheel, 0);
  BAG_Display_CreateObj(&Drawing, 16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);

  //generate save screen
  BAG_Display_CreateObj(&SaveScreen, 16, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
  
  wString SaveText;
  wString_new(&SaveText);
  SaveText.printf(&SaveText, "Posting doodle...");
  TextBox_t ScreenBox = {
    0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ALIGN_CENTER, WRAP_WORD, TBOX_WHOLESTR
  };
  BAG_Draw_Rect(SaveScreen.buffer.gfx, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RGB15(0,0,0));
  BAG_Font_Print2(SaveScreen.buffer.gfx, &ScreenBox, &SaveText, &ArialFont);
  
  //load place note button
  loadGfx(&placeButton, "graphics/okbutton.png");
  loadGfx(&cancelButton, "graphics/cancelbutton.png");

  clearScreen();

  int colorCounter = 0, drawSize = 2;
  unsigned short curColor = DRAWCOLOR;
  int prgmState = STATE_MENU;

  snprintf(saveDir, PATH_MAX, "%s/%s", rootPath, SAVE_DIR);

  int blankScreen = 0;
  time_t lastTouch = time(NULL);
  while(1) {
    if (!blankScreen) {
      time_t curTime = time(NULL);
      if (curTime - lastTouch > SLEEP_AFTER) {
        syslog(LOG_INFO, "SLEEPING SCREEN");
        setbacklight(0);
        blankScreen = 1;
      }
    } else if (Stylus.Newpress) {
      lastTouch = time(NULL);
      syslog(LOG_INFO, "WAKING SCREEN");
      setbacklight(1);
      blankScreen = 0;
    }

    
    if (Stylus.Newpress && BAG_StylusZone(0, 0, 20, 20)) {
      break;
    }
    else {
      switch(prgmState) {

      case STATE_DRAWING:
        StylusDraw(BAG_GetScreen(), SCREEN_WIDTH, SCREEN_HEIGHT, curColor, drawSize);
        if(Stylus.Released) {
          //copy image before it gets destroyed
          showToolBar(1);
          prgmState = STATE_MENU;
        }
        break;
      case STATE_MENU:

        drawToolBar(curColor);
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
          clearScreen();
        }
        //user wants new color
        else if (Stylus.Newpress && BAG_StylusZone(271, SCREEN_HEIGHT - ToolBar.data.height, 292, SCREEN_HEIGHT)) {
          prgmState = STATE_COLORWHEEL;
        }
        else if (Stylus.Newpress && isGfxTouched(&eraser)) {
          curColor = BGCOLOR;
          drawToolBar(curColor);
        }


        break;
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
              drawToolBar(curColor);
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
    }
    BAG_Update();
  } //while

  BAG_ClearScreen(0);
  BAG_Update();
  BAG_Exit(0);
  return 0;
}
