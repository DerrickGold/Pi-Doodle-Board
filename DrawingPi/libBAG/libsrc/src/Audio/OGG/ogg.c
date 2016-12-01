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

#if defined(OGG_MODULE)

#include "Audio_Core.h"
#include "ogg.h"
#include <debug/libBAG_Debug.h>


//ogg call back setup
static int _fseek64_wrap1(FILE *f,ogg_int64_t off,int whence){
  if(f==NULL){
  	BAG_DBG_LibMsg("BAG: NULL file pointer\n");
  	return ERR_END;
  }
  return fseek(f,off,whence);
}

ov_callbacks oggcallbacks = {
    (size_t (*)(void *, size_t, size_t, void *))fread,
    (int (*)(void *, ogg_int64_t, int))_fseek64_wrap1,
    (int (*)(void *))fclose,
    (long (*)(void *))ftell
};


void BAG_Ogg_Functions(SNDObj_t * snd){
	snd->Update = (void*)&BAG_OGG_UpdateOgg;
	snd->Stop = (void*)&BAG_OGG_StopOgg;
	snd->Play = (void*)&BAG_OGG_PlayOgg;
	snd->Load = (void*)&BAG_OGG_LoadOgg;
}

/*
Get current audio play time, divide by 1000 for seconds
*/
int BAG_OGG_getTime(SNDObj_t *snd){//get time in seconds
	OggVorbis_File *vf = &snd->vf;
	return ov_time_tell(vf);
}

/*
Set audio time, multiply by time 1000 for seconds
*/
void BAG_OGG_setTime(SNDObj_t *snd, int time){
	OggVorbis_File *vf = &snd->vf;
	ov_time_seek(vf, time);
}


void BAG_OGG_StopOgg(SNDObj_t *snd){
	if(GET_FLAG(snd->Flags, AUDIO_PLAY) || GET_FLAG(snd->Flags, AUDIO_PAUSE)){
		SET_FLAG(snd->Flags, AUDIO_STOP);
		RESET_FLAG(snd->Flags, AUDIO_PLAY);
		RESET_FLAG(snd->Flags, AUDIO_PAUSE);

		//reset song position to beginning
		if(GET_FLAG(snd->Flags, AUDIO_STREAM)){
			ov_clear(&snd->vf);
			fclose(snd->File);
		}
		else
			snd->bufpos = 0;
		//memset(&snd->AudioBuf, 0, sizeof(snd->AudioBuf));
		BAG_Audio_s.Channel[snd->playChan] = NULL;
		snd->playChan = 0;
	}
}


void BAG_OGG_PlayOgg(SNDObj_t *snd, s8 chan){
	//if song is already playing or paused, then do nothing
	if(GET_FLAG(snd->Flags, AUDIO_PLAY) || GET_FLAG(snd->Flags, AUDIO_PAUSE))
		goto end;

	if(!GET_FLAG(snd->Flags,AUDIO_HEADCHCK))
		goto end;

	if(chan >= MAX_AUDIO_CHANS)
		chan = MAX_AUDIO_CHANS - 1;

	snd->playChan = chan;

	//if the wave is stopped, then play from the beginning
	if(GET_FLAG(snd->Flags, AUDIO_STOP) && GET_FLAG(snd->Flags, AUDIO_STREAM)){
		snd->File = fopen(snd->FileName, "rb");
		int oggtest = ov_open_callbacks(snd->File, &snd->vf,NULL,0, oggcallbacks);
		if(oggtest != 0){
			BAG_DBG_LibMsg("BAG:Failed to open OGG callbacks\n");
			fclose(snd->File);
			return;
		}
	}

	//set flags of the snd accordingly
	SET_FLAG(snd->Flags, AUDIO_PLAY);
	RESET_FLAG(snd->Flags, AUDIO_STOP);
	RESET_FLAG(snd->Flags, AUDIO_PAUSE);
    RESET_FLAG(snd->Flags, AUDIO_STREAM_END);

	BAG_Audio_s.Channel[chan] = snd;
	end:
		return;
}


s8 BAG_OGG_LoadOgg(const char *file, SNDObj_t * snd){
	int err = ERR_NONE;
	strcpy(snd->FileName, file);
	snd->File = fopen(file, "rb");
	//Open Ogg file
	if(!snd->File){
		err = ERR_END;
		BAG_DBG_LibMsg(OPEN_FAIL_MSG, file);
		return err;
	}
	int oggtest = ov_open_callbacks(snd->File, &snd->vf,NULL,0, oggcallbacks);
	if(oggtest != 0){
		fclose(snd->File);
		err = oggtest;
		BAG_DBG_LibMsg("BAG:Failed to open OGG callbacks\n");
		return err;
	}
	//read header for important info
	vorbis_info *vi=ov_info(&snd->vf,-1);
	snd->Channels = vi -> channels;
	snd->Frequency = vi ->rate;
	snd->Bits = 16;//set audio core to output in 16 bit sound
	snd->BytesPerSample = 4;
	snd->readlen = snd->buflen = snd->BytesPerSample * audio_samples_per_trans;

	snd->format = FORMAT_OGG;

	snd->AudioBuf = (s16*)calloc(snd->buflen,1);
	if(!snd->AudioBuf){
        BAG_DBG_LibMsg(BUFFER_FAILED_MSG, snd->buflen, "Audio");
		err = ERR_END;
	}

	SET_FLAG(snd->Flags,AUDIO_HEADCHCK);
	SET_FLAG(snd->Flags, AUDIO_STOP);
	SET_FLAG(snd->Flags, AUDIO_STREAM);

	ov_clear(&snd->vf);
	fclose(snd->File);

	BAG_Ogg_Functions(snd);
	return err;
}


inline s8 BAG_OGG_UpdateOgg ( SNDObj_t *snd){
	if(GET_FLAG(snd->Flags, AUDIO_PLAY)){
		if(NULL == BAG_Audio_s.Buf_Addr)
			goto end;

		int current_section;
		int bytes = 0;
		int length = snd->readlen;
		s16 * dest = snd->AudioBuf;
		do{

			bytes = ov_read(&snd->vf,dest, length,&current_section);
			length -= bytes;
			dest += bytes>>1;
			//check if the end of the file has been reached
			if(0 == bytes) {
				SET_FLAG(snd->Flags, AUDIO_STOP);
				RESET_FLAG(snd->Flags, AUDIO_PLAY);
				RESET_FLAG(snd->Flags, AUDIO_PAUSE);

				fclose(snd->File);
                SET_FLAG(snd->Flags, AUDIO_STREAM_END);
				return ERR_END;
			}
		}while(length > 0);
	}
	end:
		return ERR_NONE;
}

#endif//OGG_MODULE
