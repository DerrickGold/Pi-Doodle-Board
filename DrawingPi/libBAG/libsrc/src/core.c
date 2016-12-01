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

#if defined(COMPILE_AUDIO)
    #include "Audio/Audio_Core.h"
#endif

#include "core.h"
#include "input/input.h"
#include "BAG_Math.h"

#include <signal.h>
#include <time.h>
#include <sys/times.h>



static Display *mainDisplay = NULL;
static Window root;

static s8  _BAG_UPDATE_FLAG = 0,
	Num_Asynchro = 0,
	Num_Synchro = 0;

static int libBAG_TTY_FD = -1;
static int libBAG_FB_FD = -1;

static struct fb_var_screeninfo vinfo;
static struct fb_var_screeninfo vinfo_orig;
static struct fb_fix_screeninfo finfo;

static struct timespec oldTicks;
static struct timespec newTicks;

static unsigned long ticksPerSecond;
static unsigned long ticksPerFrame;

static unsigned short *internalScreenAddr  = NULL;
unsigned short *SCREEN_ADDR = NULL;
unsigned int SCREEN_SIZE = (SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(unsigned short));

unsigned short *BAG_GetScreen(void) {
  return internalScreenAddr;
}



BAG_Core_UpdateFunction BAG_AsynchroFunc[BAG_MAX_FUNCTIONS];
BAG_Core_UpdateFunction BAG_SynchroFunc[BAG_MAX_FUNCTIONS];

/*===========================================================
Asynchronus function updates (not limited to the programs set FPS)
===========================================================*/
void BAG_Core_SetAsynchroFunction (void(*BAG_Core_UpdateFunction)(void)){
	if(Num_Asynchro < BAG_MAX_FUNCTIONS){
		BAG_AsynchroFunc[Num_Asynchro] = (*BAG_Core_UpdateFunction);
		Num_Asynchro++;
	}
}



void BAG_Core_RemoveAsynchroFunction (void(*BAG_Core_UpdateFunction)(void)){
	int i = 0, update = 0;
	//scan for function pointer and set it to null
	for(i = 0; i < Num_Asynchro; i++){
		if(BAG_AsynchroFunc[i] == (*BAG_Core_UpdateFunction)){
			BAG_AsynchroFunc[i] = NULL;
			update = i;
			i = 0;
			break;
		}
	}

	//if a function is found and removed, then shift everything in the array down
	if(update){
		for( i = update; i < Num_Asynchro - 1; i++)
			BAG_AsynchroFunc[i] = BAG_AsynchroFunc[i + 1];

		BAG_AsynchroFunc[i] = NULL;

		if(Num_Asynchro > 0) Num_Asynchro--;
	}
}

/*===========================================================
Synchronus function updates limited to the programs set FPS
===========================================================*/
void BAG_Core_SetSynchroFunction (void(*BAG_Core_UpdateFunction)(void)){
	if(Num_Synchro < BAG_MAX_FUNCTIONS){
		BAG_SynchroFunc[Num_Synchro] = (*BAG_Core_UpdateFunction);
		Num_Synchro++;
	}
}



void BAG_Core_RemoveSynchroFunction (void(*BAG_Core_UpdateFunction)(void)){
	int i = 0, update = 0;
	//scan for function pointer and set it to null
	for(i = 0; i < Num_Synchro; i++){
		if(BAG_SynchroFunc[i] == (*BAG_Core_UpdateFunction)){
			BAG_SynchroFunc[i] = NULL;
			update = i;
			i = 0;
			break;
		}
	}

	//if a function is found and removed, then shift everything in the array down
	if(update){
		for( i = update; i < Num_Synchro - 1; i++)
			BAG_SynchroFunc[i] = BAG_SynchroFunc[i + 1];

		BAG_SynchroFunc[i] = NULL;

		if(Num_Synchro > 0) Num_Synchro--;
	}
}

#if defined(___SYS_DS2_)
    void BAG_Core_SetFPS(s8 fps){
    	/*random values obtained through cross multiplication.
    		Setting _BAG_FPS_Lim to 60 resulted in 108 fps
    		so using those values and the value of fps we can
    		calculate a cailbrated value for the timer.
    	*/
    	_BAG_FPS_Lim = ((60 *  fps ) / 108);
    	//initiate timer for fps
    	stopTimer (0);
    	resetTimer (0);
    	initTimer (0, 1000000/_BAG_FPS_Lim, _BAG_Core_Update, 1);
    	runTimer (0);
    }
#else

void BAG_Core_SetFPS(s8 fps) {
  ticksPerFrame = ticksPerSecond / (unsigned long)fps;
}
#endif


static int setFrameBufferMode(void) {
  libBAG_TTY_FD = open("/dev/tty1", O_RDWR);
  if(libBAG_TTY_FD > -1 )
  	ioctl(libBAG_TTY_FD, KDSETMODE, KD_GRAPHICS);

  return (libBAG_TTY_FD > -1);
}

static void revertFrameBufferMode(void) {
  if(libBAG_TTY_FD < 0) return;

  ioctl(libBAG_TTY_FD, KDSETMODE, KD_TEXT);
  close(libBAG_TTY_FD);
}


static void closeFrameBuffer(void) {
  if(internalScreenAddr && (int)internalScreenAddr != -1){
    munmap(internalScreenAddr, SCREEN_SIZE);
  }
  internalScreenAddr = NULL;
  SCREEN_ADDR = NULL;
  if(libBAG_FB_FD > -1) {
    close(libBAG_FB_FD);
  }
  libBAG_FB_FD = -1;
}

void BAG_CloseFrameBuffer(void) {
  closeFrameBuffer();
}


static int initFrameBuffer(const char *framebuffer) {
  char *fb = "/dev/fb1";
  if (framebuffer) fb = (char*)framebuffer;

    libBAG_FB_FD = open(fb, O_RDWR);
    if(libBAG_FB_FD < 0) {
      printf("Failed to open %s\n", fb);
      return 0;
    }


    if (ioctl(libBAG_FB_FD, FBIOGET_VSCREENINFO, &vinfo)) {
      printf("Failed to get screen info?\n");
      closeFrameBuffer();
      return 0;
    }

    memcpy(&vinfo_orig, &vinfo, sizeof(struct fb_var_screeninfo));

    vinfo.bits_per_pixel = 16;
    vinfo.xres = 480;
    vinfo.yres = 320;
    vinfo.xres_virtual = vinfo.xres;
    vinfo.yres_virtual = 640;
    vinfo.yoffset = 0;
    vinfo.xoffset = 0;


    if(ioctl(libBAG_FB_FD, FBIOPUT_VSCREENINFO, &vinfo)) {
      closeFrameBuffer();
      return 0;
    }

    if (ioctl(libBAG_FB_FD, FBIOGET_FSCREENINFO, &finfo)) {
      closeFrameBuffer();
      return 0;
    }

    SCREEN_SIZE = finfo.smem_len;

    internalScreenAddr = (unsigned short *) mmap(0, SCREEN_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, libBAG_FB_FD, 0);
    if ((int) internalScreenAddr < 0) {
      closeFrameBuffer();
      return 0;
    }

    SCREEN_ADDR = internalScreenAddr;
    return 1;
}

void BAG_InitFrameBuffer(void) {
  initFrameBuffer(NULL);
}

void BAG_ClearScreen(unsigned short color) {
  unsigned short *start = BAG_GetScreen();
  unsigned short *end = start + (SCREEN_SIZE>>1);

  while(start < end) {
    *start = color;
    start++;
  }
}


static void waitForVBL(void) {

  clock_gettime(CLOCK_REALTIME, &newTicks);
  unsigned long delta = newTicks.tv_nsec - oldTicks.tv_nsec;

  if (delta < ticksPerFrame) {

    struct timespec sleepTs = {
      .tv_sec=0,
      .tv_nsec=(ticksPerFrame-delta)
    };

    nanosleep(&sleepTs, NULL);
  }

  clock_gettime(CLOCK_REALTIME, &oldTicks);
  _BAG_UPDATE_FLAG = 0;


}

static int initXDisplay(void) {
  mainDisplay = XOpenDisplay(NULL);
  if(!mainDisplay) return 0;
  root = XDefaultRootWindow(mainDisplay);

  return 1;
}

static void cleanXDisplay(void) {
  if(!mainDisplay) return;

  XCloseDisplay(mainDisplay);
  mainDisplay = NULL;
}


void BAG_Exit(int status) {
  BAG_Input_Clean();
  closeFrameBuffer();
  revertFrameBufferMode();
  cleanXDisplay();
  exit(status);
}

void BAG_LaunchProgram(const char *command) {
  BAG_Input_Clean();
  closeFrameBuffer();
  revertFrameBufferMode();
  cleanXDisplay();

  system(command);


  if(!initXDisplay()) {
    return;
  }
  //set terminal mode to graphics
  if(!setFrameBufferMode()) {
    return;
  }

  if (!initFrameBuffer(NULL)) {
    return;
  }

  if(!BAG_Input_Init(mainDisplay, &root)) {
    fprintf(stderr, "error init input\n");
    return;
  }

}


s8 BAG_Core_InitEx(s8 console_screen, s8 console, s8 enable_update){
  //1 second = 1000000000 nanoseconds
  ticksPerSecond = 1000000000;

  if(!initXDisplay()) {
    printf("failed to init display\n");
    return 0;
  }


  //set terminal mode to graphics
  if(!setFrameBufferMode()) {
    printf("failed to set buffer mode\n");
    return 0;
  }

  if (!initFrameBuffer(NULL)) {
    printf("failed to init framebuffer\n");
    return 0;
  }



    #if defined(COMPILE_AUDIO)
        //initialize audio mixer in libBAG
        if(!BAG_Audio_InitMixer())
            return 0;

        //set update function
        BAG_Core_SetAsynchroFunction ((void*)&BAG_Audio_UpdateAudio);
    #endif





    BAG_Core_SetFPS(BAG_DEFAULT_FPS);

#if defined(TOUCHSCREEN)
    if(!BAG_Input_Init(mainDisplay, &root)) {
      fprintf(stderr, "error init input\n");
      return 0;
    }
    BAG_Core_SetSynchroFunction ((void*)&BAG_Input_UpdateIN);
#endif

    //set default double click time
    //BAG_Input_SetDblClickTime(BAG_DEFAULT_DBLCLICK);
    //set update function
    /* BAG_Core_SetSynchroFunction ((void*)&BAG_Input_UpdateIN);

    //set display pointer update
    BAG_Core_SetSynchroFunction((void*)&_BAG_Screen_Update);

    */
    return 1;
}


s8 BAG_Core_Init(s8 console){
    return BAG_Core_InitEx(0, console, 1);
}



s8 BAG_Core_UpdateAll(void){
  	int i = 0;
	//update the synchronous functions
	do{
		if(BAG_SynchroFunc[i])
			BAG_SynchroFunc[i]();
	}while( ++i < Num_Synchro);


	//limit the number of cycles to execute synchronous functions
	i = 0;
	do{
	  if(BAG_AsynchroFunc[i])
	    BAG_AsynchroFunc[i]();
	}while( ++i < Num_Asynchro);


	waitForVBL();
	return 1;
}


/*
Built in qsort algorithm
*/
static inline void swap_internal(char *a, char *b, size_t size){
    if (a == b)
        return;

    char t;
    while (size--){
        t = *a;
        *a++ = *b;
        *b++ = t;
    }
}

static void qsort_internal(char *begin, char *end, size_t size, int(*compar)(const void *, const void *)){
    if (end <= begin)
        return;

    char *pivot = begin;
    char *l = begin + size, *r = end;

    while (l < r){
        int result = compar(l, pivot);

        if (result > 0)
            l += size;
        else if(result < 0){
            r -= size;
            swap_internal(l, r, size);
        }
        else
            break;
    }
    l -= size;
    swap_internal(begin, l, size);
    qsort_internal(begin, l, size, compar);
    qsort_internal(r, end, size, compar);

}

void BAG_Qsort(void *data, size_t dataCount, size_t dataSize, int(*compar)(const void *, const void *)){
    qsort_internal((char *)data, (char *)data+dataCount*dataSize, dataSize, compar);
}



int BAG_binSearch(const void *data,  size_t dataCount, size_t dataSize, void *searchVal, int(*compare)(const void *, const void*)){
    register int min = 0, max = dataCount - 1, mid = 0, cmp = 0;
    if(max < 0)
        return -2;
    do{
        mid = (min + max) >> 1;
        cmp = compare((void*)data + (mid * dataSize), searchVal);

        if(cmp == 0)
            return mid;
        else if(cmp < 0)
            min = mid + 1;
        else if(cmp > 0)
            max = mid - 1;

    }while(min <= max);
    return -1;
}

