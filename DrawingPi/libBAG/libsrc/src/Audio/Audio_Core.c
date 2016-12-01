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
#include "Audio_Core.h"
#include "../Filesystem/filesystem.h"
#include <debug/libBAG_Debug.h>

//global audio data
struct BAG_AUDIO_SYS BAG_Audio_s;

/*====================================
	Allocates the buffer needed for mixing channels
====================================*/
char BAG_Audio_InitMixer(void){
	BAG_Audio_s.Mix_Buf_Addr = (s16*)calloc(audio_samples_per_trans, 4);
	if(BAG_Audio_s.Mix_Buf_Addr != NULL)
		return ERR_NONE;

	BAG_DBG_LibMsg(BUFFER_FAILED_MSG, audio_samples_per_trans * 4, "Audio mix");
	return ERR_END;
}


/*=============================================
	Frees the data of a sound. Be sure to call codec specific clean
	ups first.
==============================================*/
void BAG_Audio_UnloadSnd(SNDObj_t *snd){
	//clear the file name
	memset(snd->FileName, 0, sizeof(snd->FileName));

	//ensure file handle is closed
	if(snd->File)
		fclose(snd->File);

	snd->File = NULL;

	//clear the converting buffer if used
	if(GET_FLAG(snd->Flags,AUDIO_8TO16)){
		if(snd->ConvertBuf)
			free(snd->ConvertBuf);
	}
	snd->ConvertBuf = NULL;

	//clear the mono to stereo buffer
	if(GET_FLAG(snd->Flags, AUDIO_FORCESTEREO)){
		if(snd->AudioBuf)
			free(snd->AudioBuf);
	}
	snd->AudioBuf = NULL;

	//finally clear the streaming and or file buffer
	if(snd->No_Stream_Buf)
		free(snd->No_Stream_Buf);

	snd->No_Stream_Buf = NULL;

	//clear audio update pointer
	snd->Update = NULL;
	snd->Stop = NULL;
	snd->Play = NULL;
	snd->Load = NULL;

	//now to reset everything
	memset(snd, 0, sizeof(SNDObj_t));
}


/*============================================
	Pauses playback routine for audio
============================================*/
void BAG_Audio_SetType(SNDObj_t *snd, int type){
	switch(type){

		#if defined(MP3_MODULE)
			case FORMAT_MP3:
				BAG_MP3_Functions(snd);
			break;
		#endif

		#if defined(OGG_MODULE)
			case FORMAT_OGG:
				BAG_Ogg_Functions(snd);
			break;
		#endif

		#if defined(WAV_MODULE)
			case FORMAT_WAVE:
				BAG_Wave_Functions(snd, 0);
			break;
			case FORMAT_RAW:
				BAG_Wave_Functions(snd, 1);
			break;
		#endif
	}
}

void BAG_Audio_PauseSnd(SNDObj_t *snd, char pause){
	if(GET_FLAG(snd->Flags, AUDIO_PLAY) || GET_FLAG(snd->Flags, AUDIO_PAUSE)){
		if(pause){
			RESET_FLAG(snd->Flags, AUDIO_STOP);
			RESET_FLAG(snd->Flags, AUDIO_PLAY);
			SET_FLAG(snd->Flags, AUDIO_PAUSE);
		}
		else{
			SET_FLAG(snd->Flags, AUDIO_PLAY);
			RESET_FLAG(snd->Flags, AUDIO_STOP);
			RESET_FLAG(snd->Flags, AUDIO_PAUSE);
		}
	}
}


void BAG_Audio_StopSnd(SNDObj_t * snd){
	if(snd->Stop)
		snd->Stop(snd);
}


void BAG_Audio_PlaySnd(SNDObj_t * snd, char channel){
	if(snd->Play)
		snd->Play(snd, channel);
}

int BAG_Audio_LoadSndEx(const char * filename, SNDObj_t * snd, int type){
	BAG_Audio_UnloadSnd(snd);
	//get file size
	FILE * audio = fopen(filename, "rb");
	if(!audio)
		return ERR_END;

	fseek(audio, 0, SEEK_END);
	snd->File_Size = ftell(audio);
	fseek(audio, 0, SEEK_SET);
	fclose(audio);

	//load audio
	BAG_Audio_SetType(snd, type);
	if(snd->Load)
		return snd->Load(filename, snd);

	return ERR_END;
}

int BAG_Audio_LoadSnd(const char *filename, SNDObj_t *snd){
	u8 magic_number[4];
	memset(magic_number, 0, sizeof(magic_number));

	if(BAG_Filesystem_GetMagicNumbers(filename, magic_number, 4) == -1)
		return ERR_END;

	int err = ERR_NONE;
	if((magic_number[0] == 'I' && magic_number[1] == 'D'  && magic_number[2] == '3') ||
	    ((((magic_number[1] << 8) + magic_number[0]) & 0xFFFE) == 0xFFFA))
	{
		err = BAG_Audio_LoadSndEx(filename, snd, FORMAT_MP3);
	}
	else if(magic_number[0] == 'R' && magic_number[1] == 'I'  &&
		    magic_number[2] == 'F' && magic_number[3] == 'F')
	{
		err =  BAG_Audio_LoadSndEx(filename, snd, FORMAT_WAVE);
	}
	else if(magic_number[0] == 'O' && magic_number[1] == 'g'  &&
	        magic_number[2] == 'g' && magic_number[3] == 'S')
	{
		err = BAG_Audio_LoadSndEx(filename, snd, FORMAT_OGG);
	}

	if(err != ERR_NONE) {
		BAG_Audio_UnloadSnd(snd);
		BAG_DBG_LibMsg("BAG:Failed to open %s\n", filename);
	}
	return err;
}

int BAG_Audio_LoadSndExt(const char *filename, SNDObj_t *snd){
	char newFile[MAX_PATH];
	char exts[3][5] = {
		".mp3\0",
		".ogg\0",
		".wav\0",
	};

	char *extlist[3];
	int i = 0;
	for(; i < 3; i++)
		extlist[i] = (char*)&exts[i];

	BAG_Filesystem_FindFileExt(filename, extlist, 3, newFile, MAX_PATH);
	return BAG_Audio_LoadSnd(newFile, snd);
}


/*============================================
	Converts a signed 8 bit audio buffer to signed 16 bit
	for playback.
============================================*/
inline void BAG_Audio_u8tos16(s16 * source, s16 *dest){
	register int n=0;
	while(n++ < audio_samples_per_trans){
		*dest = (*(source)-128)<<8;dest++;
		*dest= (*(source++)-128)<<8;dest++;
	}
}


/*==========================================================
	Expands audio data from lower frequencies to the target frequency of
	the ds2io layer
==========================================================*/
inline void BAG_Audio_ConvertFreq(s16 *source, s16 *dest, int HzFactor){
	register int n=0, k = 0, newFactor = HzFactor>>8;
	while(n++ < audio_samples_per_trans){
		k=0;
		while(k++ < newFactor)
			*dest++ = *source;
		source++;
	}
}


/*========================================
Convert a mono data stream to stereo
	Basically copies the mono data to both
	left and right outputs
=========================================*/
inline void BAG_Audio_MonoToStereo(s16 * source, s16 * dest){
	int n=0;
	while(n++ < audio_samples_per_trans){
		//we just want to copy the same data to both streams
		*dest++ = *source;
		*dest++ = *source++;
	}
}


/*==========================================
Copies stereo and or mono data to the
	ds two io layer
===========================================*/
inline void BAG_Audio_TransferStream(void *data, char channels){
	if(!BAG_Audio_s.Buf_Addr || !data)
		return;

	s16 *src = (s16*)data;
	s16 *dst0 = BAG_Audio_s.Buf_Addr;
	s16 *dst1 = BAG_Audio_s.Buf_Addr + audio_samples_per_trans;

	int n=0;
	while(n++ < audio_samples_per_trans){
		switch(channels){
			case 2://stereo
				*dst0++ = *src++;
				*dst1++ = *src++;
			break;
			case 1://mono
				*dst0++ = *src++;
			break;
		}
	}
}


/*
Mixes to snd files together.
	mixes must be stereo and 16 bit
	updateing the snd file will auto matically
	convert mono samples to stereo for mixing
*/

static void _BAG_Audio_Mix(s16* source[MAX_AUDIO_CHANS], s16 *dest, int srcCount){

	if(dest == NULL || srcCount == 0) //No sounds are playing so do not mix anything
		return;

	s16 n = 0, NewSrc[srcCount];
	int SrcCheck = 0, i = 0;

	while(n++ < audio_samples_per_trans<<1){
		SrcCheck = 0;
		for(i = 0; i< srcCount; i++){
			//reduce volume by half
			NewSrc[i] = *source[i];
			SrcCheck += NewSrc[i];
			source[i]++;
		}

		//check if audio in range
		if (SrcCheck >= -32768 && SrcCheck <= 32767)
			*dest = SrcCheck;

		dest++;
	}
}


/*===================================================
	Updates all the audio buffers and handles the audio mixing,
	then sends the data through the ds2io layer for sound output.

	This function is necessary for audio output.
====================================================*/

inline void BAG_Audio_UpdateAudio(void){
	int tmp = ds2_checkAudiobuff();
	if(tmp >= 0 && tmp < AUDIO_BUFFER_COUNT){
		int i=0, play=0;
		s16 *Source[MAX_AUDIO_CHANS];
		BAG_Audio_s.Buf_Addr = (s16*)ds2_getAudiobuff();
		//update all channels
		for(i = 0; i< MAX_AUDIO_CHANS; i++){
			if( BAG_Audio_s.Channel[i] != NULL && BAG_Audio_s.Channel[i]->Update != NULL && GET_FLAG(BAG_Audio_s.Channel[i]->Flags,AUDIO_PLAY)){
				BAG_Audio_s.Channel[i]->Update(BAG_Audio_s.Channel[i]);
				Source[play] =  (s16*)BAG_Audio_s.Channel[i]->AudioBuf;
				play++;
			}
		}
		if(play > 0){
			_BAG_Audio_Mix(Source, BAG_Audio_s.Mix_Buf_Addr, play);//mix all audio sources
			BAG_Audio_TransferStream(BAG_Audio_s.Mix_Buf_Addr,2);//transfer stream to DS2 audio layer in stereo
			ds2_updateAudio();//send stream to DS
		}
	}
}

int BAG_Audio_IsPlaying(void){
	register int i = 0;
	for(i = 0; i < MAX_AUDIO_CHANS; i++){
		if(BAG_Audio_s.Channel[i] != NULL && GET_FLAG(BAG_Audio_s.Channel[i]->Flags, AUDIO_PLAY))
			return ERR_NONE;
	}
	return ERR_END;
}

inline void BAG_Audio_UpdateAudioBuffers(void){
	int i = 0;
	while( i++ < AUDIO_BUFFER_COUNT)
		BAG_Audio_UpdateAudio();
}


