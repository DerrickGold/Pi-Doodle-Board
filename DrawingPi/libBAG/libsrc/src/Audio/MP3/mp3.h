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

#ifndef __MP3_H__
#define __MP3_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <ds2sdk.h>
#include "mp3helper.h"

/** @defgroup MP3 Mp3 Audio
 *  Mp3 audio functions
 *  @{
 */



  /*! \fn s8 BAG_MP3_LoadMp3 (const char *file, SNDObj_t * snd)
    \brief
		 Loads and prepares an ogg file for playback

	\~\param FileName
		filepath in filesystem
	\~\param snd
		audio handle


	\~\return
		ERR_NONE if successful
*/
extern s8 BAG_MP3_LoadMp3 (const char *file, SNDObj_t * snd);





 /*! \fn s8 BAG_MP3_UpdateMp3 (SNDObj_t *snd)
    \brief
		 Updates an ogg file in the audio system

	\~\param snd
		audio handle
*/
extern s8 BAG_MP3_UpdateMp3 (SNDObj_t *snd);





 /*! \fn void BAG_MP3_StopMp3 (SNDObj_t *snd)
    \brief
		 Stops a ogg audio file

	\~\param snd
		audio handle
*/
extern void BAG_MP3_StopMp3 (SNDObj_t *snd);





 /*! \fn void BAG_MP3_PlayMp3 (SNDObj_t *snd, s8 chan)
    \brief
		 Plays an ogg file that has been loaded

	\~\param snd
		audio handle
	\~\param chan
		Channel to play sound on, ( 0 to MAX_AUDIO_CHANS -1 )
*/
extern void BAG_MP3_PlayMp3 (SNDObj_t *snd, s8 chan);


extern void BAG_MP3_Functions(SNDObj_t * snd);
//function short forms
#ifdef BAG_SHORTFORM

	#define BAG_PlayMp3 				BAG_MP3_PlayMp3
	#define BAG_StopMp3				BAG_MP3_StopMp3
	#define BAG_LoadMp3				BAG_MP3_LoadMp3
	#define BAG_UpdateMp3			BAG_MP3_UpdateMp3

#endif


#ifdef __cplusplus
}
#endif
/** @} */ // end of Mp3

#endif
