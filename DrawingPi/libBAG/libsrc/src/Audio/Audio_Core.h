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
#ifndef _BAG_AUDIO_
#define _BAG_AUDIO_


#ifdef __cplusplus
extern "C" {
#endif


#include <ds2sdk.h>

#if defined(OGG_MODULE)
    #include "OGG/ogg_config.h"
#endif

#if defined(MP3_MODULE)
    #include "MP3/mp3helper.h"
#endif

#define AUDIO_BUF_ALLOC

#define MAX_AUDIO_CHANS 32
#define BAG_TARGET_FREQUENCY 44100

typedef enum{
	AUDIO_PLAY = (1<<0),
	AUDIO_PAUSE = (1<<1),
	AUDIO_STOP = (1<<2),
	AUDIO_STREAM = (1<<3),
	AUDIO_HEADCHCK = (1<<4),
	AUDIO_FORCESTEREO = (1<<5),
	AUDIO_8TO16 = (1<<6),
	AUDIO_STREAM_END = (1 << 7),
}
SND_FLAGS;

typedef enum{
	FORMAT_RAW,
	FORMAT_WAVE,
	FORMAT_OGG,
	FORMAT_MP3,
}
AUDIO_FORMAT;

typedef struct SNDObj_s{
	char FileName[MAX_PATH];
	FILE *File;

    #if defined(OGG_MODULE)
        //for ogg support
        OggVorbis_File vf;
    #endif

	#if defined(MP3_MODULE)
		MP3Container mp3_container;
	#endif

	u32 File_Size;
	//how many channels the audio has, which software channel the audio is playing on
	s8 Channels, playChan;
	int Frequency, BytesPerSample, Length,Bits, HzFactor;
	//audio buffer
	void *AudioBuf, *No_Stream_Buf, *ConvertBuf;
	int buflen, bufpos,readlen;
	int format;//audio type

	u32 Flags;

	//functions
	s8 (*Update) (struct SNDObj_s *);
	void (*Stop) (struct SNDObj_s *);
	void (*Play) (struct SNDObj_s *, s8);
	s8 (*Load) (const char* , struct SNDObj_s *);
}SNDObj_t;

struct BAG_AUDIO_SYS{
	SNDObj_t *Channel[MAX_AUDIO_CHANS];
	//address for the ds2 io layer
	s16 *Buf_Addr;
	//just a buffer for mixing audio, should be the same size as the audio buffers
	s16 *Mix_Buf_Addr;
};
extern struct BAG_AUDIO_SYS BAG_Audio_s;

#if defined(WAV_MODULE)
    #include "WAVE/wave_header.h"
    #include "WAVE/wave.h"
#endif

#if defined(OGG_MODULE)
    #include "OGG/ogg.h"
#endif

#if defined(MP3_MODULE)
    #include "MP3/mp3.h"
#endif


/** @defgroup Audio_Core Audio Core
 *  Core components for audio system
 *  @{
 */



 /*! \fn void BAG_Audio_u8tos16(s16 * source, s16 *dest)
    \brief
		 Converts an unsigned 8 bit sample of audio to signed 16 bits

	\~\param source
		Source buffer
	\~\param dest
		Destination buffer
*/
extern void BAG_Audio_u8tos16(s16 * source, s16 *dest);



 /*! \fn void BAG_Audio_ConvertFreq(s16 *source, s16 *dest, int HzFactor)
    \brief
		 Converts an frequency to target frequency as given by HzFactor.

	\~\param source
		Source buffer
	\~\param  dest
		Destination buffer
	\~\param HzFactor
		Number of times to copy the data to hit the target frequency of 44100 hz
*/
extern void BAG_Audio_ConvertFreq(s16 *source, s16 *dest, int HzFactor);



 /*! \fn void BAG_Audio_MonoToStereo(s16 * source, s16 * dest)
    \brief
		 Duplicates the data of a mono stream and interlaces it into a stereo stream.

	\~\param source
		Source buffer
	\~\param dest
		Destination buffer
*/
extern void BAG_Audio_MonoToStereo(s16 * source, s16 * dest);


 /*! \fn void BAG_Audio_UnloadSnd(struct SNDObj_t *snd)
    \brief
		 Unload data used by file specified
*/
extern void BAG_Audio_UnloadSnd(SNDObj_t *snd);



 /*! \fn void BAG_Audio_PauseSnd(SNDObj_t *snd, char pause)
    \brief
		 Pauses audio playback routine for specified sound

	\~\param snd
		sound to pause
	\~\param pause
		1 pauses sound, 0 unpauses sound

*/
extern void BAG_Audio_PauseSnd(SNDObj_t *snd, char pause);




 /*! \fn void BAG_Audio_StopSnd(SNDObj_t * snd);
    \brief
		 Stops audio playback routine for specified sound

	\~\param snd
		sound to pause
*/
extern void BAG_Audio_StopSnd(SNDObj_t * snd);




 /*! \fn void BAG_Audio_PlaySnd(SNDObj_t * snd, char channel);
    \brief
		Plays and audio file loaded to the snd handle

	\~\param snd
		sound to pause
	\~\param channel
		an audio channel between 0 and MAX_AUDIO_CHANS

*/
extern void BAG_Audio_PlaySnd(SNDObj_t * snd, char channel);




 /*! \fn int BAG_Audio_LoadSndEx( const char * filename, SNDObj_t * snd, int type)
    \brief
		Loads an audio file of specified type

	\~\param filename
		filepath of audio file
	\~\param snd
		handle for audio file
	\~\param type
		FORMAT_RAW, FORMAT_WAVE, FORMAT_OGG, or FORMAT_MP3

*/
extern int BAG_Audio_LoadSndEx( const char * filename, SNDObj_t * snd, int type);




 /*! \fn int BAG_Audio_LoadSnd( const char * filename, SNDObj_t * snd, int type)
    \brief
		Loads an audio file and auto-detects type

	\~\param filename
		filepath of audio file
	\~\param snd
		handle for audio file
	\~\param type
		FORMAT_RAW, FORMAT_WAVE, FORMAT_OGG, or FORMAT_MP3

*/
extern int BAG_Audio_LoadSnd(const char *filename, SNDObj_t *snd);




 /*! \fn int BAG_Audio_LoadSndExt( const char * filename, SNDObj_t * snd, int type)
    \brief
		Loads an audio file without an extension or type specified, auto detects the type

	\~\param filename
		filepath of audio file
	\~\param snd
		handle for audio file
	\~\param type
		FORMAT_RAW, FORMAT_WAVE, FORMAT_OGG, or FORMAT_MP3

*/
extern int BAG_Audio_LoadSndExt(const char *filename, SNDObj_t *snd);




 /*! \fn void BAG_Audio_TransferStream(void *data, char channels)
    \brief
		 Transfers audio data to a pointer to the DS2's io layer which is updated in the BAG_Audio_UpdateAudio() function

	\~\param data
		An audio buffer to sent to the DS2 io layer
	\~\param channels
		1 for mono, 2 for stereo
*/
extern void BAG_Audio_TransferStream(void *data, char channels);


 /*! \fn char BAG_Audio_InitMixer(void)
    \brief
		Allocates a buffer for mixing samples, returns 1 if successful, 0 if fails.

	\~\return
		1 for success
*/
extern char BAG_Audio_InitMixer(void);


 /*! \fn void BAG_Audio_UpdateAudio(void)
    \brief
		 Updates all audio played using core audio functions.
*/
extern void BAG_Audio_UpdateAudio(void);


 /*! \fn void BAG_Audio_UpdateAudioBuffers(void)
    \brief
		 Updates audio to all audio buffers available in the DSTwo IO layer
*/
extern void BAG_Audio_UpdateAudioBuffers(void);

extern int BAG_Audio_IsPlaying(void);

//function s16 forms
#ifdef BAG_SHORTFORM

	#define BAG_SndU8S16				BAG_Audio_u8tos16
	#define BAG_CnvtFreq				BAG_Audio_ConvertFreq
	#define BAG_MonoStereo				BAG_Audio_MonoToStereo
	#define BAG_UnloadSnd				BAG_Audio_UnloadSnd
	#define BAG_PauseSnd				BAG_Audio_PauseSnd
	#define BAG_StopSnd					BAG_Audio_StopSnd
	#define BAG_PlaySnd					BAG_Audio_PlaySnd
	#define BAG_LoadSnd					BAG_Audio_LoadSnd
	#define BAG_TAudio					BAG_Audio_TransferStream
	#define BAG_InitAudio				BAG_Audio_InitMixer
	#define BAG_UpdateAudio				BAG_Audio_UpdateAudio
	#define BAG_UpdateAudioBuffers		BAG_Audio_UpdateAudioBuffers

#endif

/** @} */ // end of Audio

#ifdef __cplusplus
}
#endif

#endif
