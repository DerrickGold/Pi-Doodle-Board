#ifndef _BAG_JPEG_
#define _BAG_JPEG_

#include "../Display.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int BAG_Display_LoadJpg(const char *filename, GFXObj_t *object);
extern void BAG_Display_ReadJpgSize(const char *filename, int *width, int *height);
extern int BAG_Display_LoadJpgFromBuf(u8 *jpgData, unsigned int size, GFXObj_t *object);
extern void BAG_Display_GetJpgSize(unsigned char *jpgData, unsigned int size, int *width, int *height);
#ifdef __cplusplus
}
#endif


#endif
