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

#if defined(WAV_MODULE)

#ifndef __WAVE_H__
#define __WAVE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "ds2sdk.h"

/** @defgroup Wave Wave Audio
 *  Wave and raw playback functions.
 *  @{
 */



 /*! \fn void BAG_Wave_PlayWavEx(SNDObj_t *snd, s8 chan, int offset)
    \brief
		 Plays a wave or a raw audio file.

	\~\param snd
		audio handle
	\~\param chan
		Channel to play sound on, ( 0 to MAX_AUDIO_CHANS -1 )
	\~\param offset
		Offset to start playing from file. This is mainly used to skip rereading the header of the file and to skip it in raw files.
*/
extern void BAG_Wave_PlayWavEx(SNDObj_t *snd, s8 chan, int offset);




 /*! \fn void BAG_Wave_PlayWav(SNDObj_t * snd, s8 chan)
    \brief
		 Plays a wave audio file

	\~\param snd
		audio handle
	\~\param chan
		Channel to play sound on, ( 0 to MAX_AUDIO_CHANS -1 )
*/
extern void BAG_Wave_PlayWav(SNDObj_t * snd, s8 chan);




 /*! \fn void BAG_Wave_PlayRaw(SNDObj_t * snd, s8 chan)
    \brief
		 Plays a raw audio file

	\~\param snd
		audio handle
	\~\param chan
		Channel to play sound on, ( 0 to MAX_AUDIO_CHANS -1 )
*/
extern void BAG_Wave_PlayRaw(SNDObj_t * snd, s8 chan);





 /*! \fn void BAG_Wave_StopWav(SNDObj_t *snd)
    \brief
		 Stops a wave sound

	\~\param snd
		audio handle
*/
extern void BAG_Wave_StopWav(SNDObj_t *snd);




 /*! \def BAG_Wave_StopRaw( snd )
    \brief
		 Stops a raw sound

	\~\param snd
		audio handle
*/
#define BAG_Wave_StopRaw( snd ) BAG_Wave_StopWav( snd)




 /*! \fn s8 BAG_Wave_LoadWav(const char *FileName, SNDObj_t *snd, bool Stream)
    \brief
		 Loads and prepares a wave audio file for playback

	\~\param FileName
		filepath in filesystem
	\~\param snd
		audio handle
	\~\param Stream
		1 - enables streaming from filesystem, 0 - loads entire file for playback from ram.

	\~\return
		ERR_NONE is successful
*/
extern s8 BAG_Wave_LoadWav(const char *FileName, SNDObj_t *snd, bool Stream);




 /*! \fn char BAG_Wave_LoadRaw(const char *FileName, SNDObj_t *snd, s8 channels, int freq, s8 bits, bool Stream)
    \brief
		 Loads and prepares a raw audio file for playback

	\~\param FileName
		filepath in filesystem
	\~\param snd
		audio handle
	\~\param Stream
		1 - enables streaming from filesystem, 0 - loads entire file for playback from ram.

	\~\return
		ERR_NONE is successful
*/
extern s8 BAG_Wave_LoadRaw(const char *FileName, SNDObj_t *snd, s8 channels, int freq, s8 bits, bool Stream);



 /*! \fn void BAG_Wave_UpdateWav(SNDObj_t *snd)
    \brief
		 Updates a wave or raw audio file

	\~\param snd
		audio handle
*/
extern s8 BAG_Wave_UpdateWav(SNDObj_t *snd);



 /*! \fn void BAG_Wave_GetWavInfo_s(SNDObj_t *snd, char *textbuf)
    \brief
		 Formats wave audio information into a string

	\~\param snd
		audio handle
	\~\param textbuf
		A buffer to hold information
*/
extern void BAG_Wave_GetWavInfo_s(SNDObj_t *snd, char *textbuf);




 /*! \def void BAG_Wave_GetRawInfo_s( snd , textbuf )
    \brief
		 Formats raw audio information into a string
*/
#define BAG_Wave_GetRawInfo_s( snd , textbuf ) BAG_Wave_GetWavInfo_s( snd, textbuf)


extern void BAG_Wave_Functions(SNDObj_t * snd, char raw);
//function short forms
#ifdef BAG_SHORTFORM

	#define BAG_PlayWavEx 			BAG_Wave_PlayWavEx
	#define BAG_PlayWav					BAG_Wave_PlayWav
	#define BAG_PlayRaw					BAG_Wave_PlayRaw
	#define BAG_StopWav				BAG_Wave_StopWav
	#define BAG_StopRaw				BAG_Wave_StopRaw
	#define BAG_LoadWav				BAG_Wave_LoadWav
	#define BAG_LoadRaw				BAG_Wave_LoadRaw
	#define BAG_UpdateWav			BAG_Wave_UpdateWav
	#define BAG_GetWavInfo			BAG_Wave_GetWavInfo_s
	#define BAG_GetRawInfo			BAG_Wave_GetRawInfo_s

#endif

/** @} */ // end of Wave

#ifdef __cplusplus
}
#endif

#endif

#endif//WAV_MODULE
