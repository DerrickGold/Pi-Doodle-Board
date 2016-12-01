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

#include <ds2sdk.h>
#include "Audio_Core.h"
#include <debug/libBAG_Debug.h>


void BAG_Wave_Functions(SNDObj_t *snd, char raw){
	//set up audio functions
	snd->Update = (void*)&BAG_Wave_UpdateWav;
	snd->Stop = (void*)&BAG_Wave_StopWav;
	if(raw == 0){
		snd->Play = (void*)&BAG_Wave_PlayWav;
		snd->Load = (void*)&BAG_Wave_LoadWav;
	}
	else{
		snd->Play = (void*)&BAG_Wave_PlayRaw;
		snd->Load = (void*)&BAG_Wave_LoadRaw;
	}
}


void BAG_Wave_GetWavInfo_s(SNDObj_t *snd, char *textbuf){
	sprintf(textbuf, "%s \nChannels:%d \nFrequency:%d \nBytesPerSample:%d \nBits:%d \nSize:%d",
						snd->FileName, snd->Channels, snd->Frequency, snd->BytesPerSample, snd->Bits,snd->Length);
}


void BAG_Wave_PlayWavEx(SNDObj_t *snd, s8 chan, int offset){
	//if song is already playing or paused, then do nothing
	if(GET_FLAG(snd->Flags, AUDIO_PLAY) || GET_FLAG(snd->Flags, AUDIO_PAUSE))
		goto end;

	if(!GET_FLAG(snd->Flags,AUDIO_HEADCHCK))
		goto end;

	if(chan >= MAX_AUDIO_CHANS)
		chan = MAX_AUDIO_CHANS - 1;

	snd->playChan = chan;

	//set the update function
	//if the wave is stopped, then play from the beginning
	if(GET_FLAG(snd->Flags, AUDIO_STOP) && GET_FLAG(snd->Flags, AUDIO_STREAM)){
		snd->File = fopen(snd->FileName, "r");
		fseek ( snd->File, offset, SEEK_SET );
	}

	//set flags of the snd accordingly
	SET_FLAG(snd->Flags, AUDIO_PLAY);
	RESET_FLAG(snd->Flags, AUDIO_STOP);
	RESET_FLAG(snd->Flags, AUDIO_PAUSE);
	BAG_Audio_s.Channel[chan] = snd;

	end:
		return;
}


void BAG_Wave_PlayWav(SNDObj_t * snd, s8 chan){
	BAG_Wave_PlayWavEx(snd, chan, sizeof(WaveHeader));
}


void BAG_Wave_PlayRaw(SNDObj_t * snd, s8 chan){
	BAG_Wave_PlayWavEx(snd, chan, 0);
}


void BAG_Wave_StopWav(SNDObj_t *snd){
	if(GET_FLAG(snd->Flags, AUDIO_PLAY) || GET_FLAG(snd->Flags, AUDIO_PAUSE)){
		SET_FLAG(snd->Flags, AUDIO_STOP);
		RESET_FLAG(snd->Flags, AUDIO_PLAY);
		RESET_FLAG(snd->Flags, AUDIO_PAUSE);

		//reset song position to beginning
		if(GET_FLAG(snd->Flags, AUDIO_STREAM)){
			fseek(snd->File, 0, SEEK_SET);
			fclose(snd->File);
		}
		else
			snd->bufpos = 0;

		BAG_Audio_s.Channel[snd->playChan] = NULL;
		snd->playChan = 0;
	}
}


/*============================================================
	This function will check the wave file for its existance
	and read the important data from the header.
=============================================================*/
static int Init_Wav(const char *filename, SNDObj_t *snd){
	int err = ERR_NONE;
	//allocate space for the header
	WaveHeader *Header;
	Header = (WaveHeader*)calloc(sizeof(WaveHeader), 1);
	if(Header == NULL){
		BAG_DBG_LibMsg(BUFFER_FAILED_MSG, sizeof(WaveHeader), "WAV Header");
		err = ERR_END;
		goto error;
	}

	//open file if it exists
	strcpy(snd->FileName, filename);
	snd->File = fopen(snd->FileName, "r");
	if(!snd->File){
		BAG_DBG_LibMsg(OPEN_FAIL_MSG, filename);
		err = ERR_END;
		goto error;
	}

	//read the header
	int len = fread(Header, 1, sizeof(WaveHeader), snd->File);
	if(len != sizeof(WaveHeader)){
		BAG_DBG_LibMsg("BAG: Invalid WAV header\n");
		err = ERR_END;
		fseek(snd->File, 0, SEEK_SET);
		fclose(snd->File);
		goto error;
	}

	//if the header passes all the checks
	if (Header->main_chunk == RIFF && Header->chunk_type == WAVE &&
		Header->sub_chunk == FMT && Header->data_chunk == DATA) {
		if (Header->format != PCM_CODE) {
			BAG_DBG_LibMsg("BAG: Invalid WAV header\n");
			err = ERR_END;
			goto error;
		}
		if (Header->modus > 2) {
			BAG_DBG_LibMsg("BAG: Invalid WAV header\n");
			err = ERR_END;
			goto error;
		}

		//copy data from the header then clear it
		snd->Channels = Header->modus;
		snd->Frequency = Header->sample_fq;
		snd->BytesPerSample = Header->byte_p_spl;
		snd->Length = Header->data_length;
		snd->Bits = Header->bit_p_spl;

		free(Header);
		Header = NULL;

		fseek(snd->File, 0, SEEK_SET);
		fclose( snd->File);
		SET_FLAG(snd->Flags,AUDIO_HEADCHCK);

		return ERR_NONE;
	}


	error:
		if(Header)
			free(Header);

		Header = NULL;
		return err;
}


/*============================================================
	Create the buffers necessary for decoding, streaming and converting
	sound files as needed.

============================================================*/

static s8 Create_Decoding_Bufs( SNDObj_t *snd, bool Stream){
	int err = ERR_NONE;

	//force the mono audio to play in stereo for mixing purposes
	if(snd->Channels == WAVE_MONO){
		SET_FLAG(snd->Flags, AUDIO_FORCESTEREO);
		//an extra buffer is needed for converting mono to stereo
		snd->AudioBuf = (u16*)calloc(snd->buflen, 1);
		if(NULL == snd->AudioBuf) {
			BAG_DBG_LibMsg(BUFFER_FAILED_MSG, snd->buflen, "Audio");
			RESET_FLAG(snd->Flags, AUDIO_PLAY);
			err = ERR_END;
			goto end;
		}
	}

	/*
		If streaming is enabled, only enough memory for one transfer
		of the dstwo io layer is allocated
	*/
	if(Stream){
		SET_FLAG(snd->Flags, AUDIO_STREAM);

		//allocate the normal buffer
		snd->No_Stream_Buf =(u16*) calloc(snd->buflen, 1);// final buffer
		if(snd->No_Stream_Buf == NULL){
			BAG_DBG_LibMsg(BUFFER_FAILED_MSG, snd->buflen, "WAV stream");
			err = ERR_END;
			RESET_FLAG(snd->Flags, AUDIO_PLAY);
			goto end;
		}
		return ERR_NONE;
	}
	/*
		If streaming is disabled, then enough memory
		to hold the entire snd file is allocated
	*/
	else{
		/* If wave file is 8 bit, then an extra buffer is needed for up converting to 16 bit
			to prevent overwrite of original data. Streaming doesn't need this extra buffer
			as it is already handled.
		*/
		if(snd->Bits == 8){
			snd->ConvertBuf =(s16*) calloc(snd->buflen,1);
			if(snd->ConvertBuf == NULL){
				BAG_DBG_LibMsg(BUFFER_FAILED_MSG, snd->buflen, "WAV conversion");
				err = ERR_END;
				goto end;
			}
			SET_FLAG(snd->Flags,AUDIO_8TO16);
		}


		snd->No_Stream_Buf =(s16*) calloc(snd->Length,1);
		if(NULL == snd->No_Stream_Buf){
			BAG_DBG_LibMsg(BUFFER_FAILED_MSG, snd->Length, "WAV stream");
			err = ERR_END;
			RESET_FLAG(snd->Flags, AUDIO_PLAY);
			goto end;
		}
		else{
			snd->File = fopen(snd->FileName, "r");
			if(snd->File){
				fread(snd->No_Stream_Buf, 1, snd->Length, snd->File);
				fclose(snd->File);
				return 1;
			}
			else{
				BAG_DBG_LibMsg(OPEN_FAIL_MSG, snd->FileName);
				err = ERR_END;
			}
		}
	}
	end:
		return err;
}


/*==============================================
	Load a Raw file, basically a wave file but with no header
==============================================*/
s8 BAG_Wave_LoadRaw(const char *FileName, SNDObj_t *snd, s8 channels, int freq, s8 bits, bool Stream){
	int err = ERR_NONE;
	snd->Flags = 0;

	//open file if it exists
	strcpy(snd->FileName, FileName);
	snd->File = fopen(snd->FileName, "r");
	if(!snd->File){
		BAG_DBG_LibMsg(OPEN_FAIL_MSG, snd->FileName);
		err= ERR_END;
		goto end;
	}

	fseek(snd->File, 0, SEEK_END);
	snd->Length = ftell(snd->File);
	fseek(snd->File, 0, SEEK_SET);
	fclose(snd->File);

	snd->Channels = channels;
	snd->Frequency = freq;
	snd->BytesPerSample = (bits == 16) ? 4 : 2;
	snd->Bits = bits;

	SET_FLAG(snd->Flags, AUDIO_STOP);
    RESET_FLAG(snd->Flags, AUDIO_STREAM_END);

	//Audio buffer length should always be for 16 bit snd for the DS2IO layer
	snd ->buflen= 4 * audio_samples_per_trans;
	//how much data to read from the file
	snd ->readlen= snd->BytesPerSample * audio_samples_per_trans;
	//calculate the new read length based on the waves frequency
	snd-> HzFactor = (BAG_TARGET_FREQUENCY<<8) / (snd->Frequency);
	snd->readlen = snd->readlen / (snd->HzFactor>>8);

	SET_FLAG(snd->Flags,AUDIO_HEADCHCK);

	err = Create_Decoding_Bufs(snd, Stream);

	BAG_Wave_Functions(snd, 1);
	//error
	end:
		return err;
}



/*===================================================================
	Prepares and enables the audio for playback.
	Stream determines whether or not to load the entire
	wave file into the ram. This is great for small files however
	larger files should be streamed to use the least amount of
	memory.
====================================================================*/

s8 BAG_Wave_LoadWav(const char *FileName, SNDObj_t *snd, bool Stream){
	int err = ERR_NONE;
	//clear flags
	snd->Flags = 0;
	//ensure the wave file is legit first
	err = Init_Wav(FileName,snd);
	if(err != ERR_NONE)
		goto end;

	if(!GET_FLAG(snd->Flags,AUDIO_HEADCHCK)){
		err = ERR_END;
		goto end;
	}

	// et the wave state to stopped
	SET_FLAG(snd->Flags, AUDIO_STOP);
    RESET_FLAG(snd->Flags, AUDIO_STREAM_END);

	//Audio buffer length should always be for 16 bit snd for the DS2IO layer
	snd ->buflen= 4 * audio_samples_per_trans;
	//how much data to read from the file
	snd ->readlen= snd->BytesPerSample * audio_samples_per_trans;
	//calculate the new read length based on the waves frequency
	snd-> HzFactor = (BAG_TARGET_FREQUENCY<<8) / (snd->Frequency);
	snd->readlen = snd->readlen / (snd->HzFactor>>8);

	err = Create_Decoding_Bufs((SNDObj_t*)snd, Stream);

	BAG_Wave_Functions(snd, 0);
	end:
		return err;
}




/*=====================================================
	Update the buffers for specified wave file, this handles both streaming
	and non-streaming. Audio is also converted here for playback
=====================================================*/
static s8 Update_Wave_Stream ( SNDObj_t *snd){
	int bytes = fread(snd->No_Stream_Buf, 1, snd->readlen, snd->File);

	//check if the end of the file has been reached
	if(0 == bytes) {
		SET_FLAG(snd->Flags, AUDIO_STOP);
		RESET_FLAG(snd->Flags, AUDIO_PLAY);
		RESET_FLAG(snd->Flags, AUDIO_PAUSE);

		fseek(snd->File, 0, SEEK_SET);
		fclose(snd->File);
		return ERR_END;
	}


	s16 *AudioSource = snd->No_Stream_Buf;

	//convert 8 bits to 16 bits
	if(snd->Bits == 8)
		BAG_Audio_u8tos16(snd->No_Stream_Buf,snd->No_Stream_Buf);

	if(snd->Frequency < BAG_TARGET_FREQUENCY)
		BAG_Audio_ConvertFreq(snd->No_Stream_Buf, snd->No_Stream_Buf,snd->HzFactor);

	//Check if and convert mono to stereo
	if(snd->Channels == 1 && GET_FLAG(snd->Flags, AUDIO_FORCESTEREO))
		BAG_Audio_MonoToStereo(AudioSource,snd->AudioBuf);
	else
		snd->AudioBuf = AudioSource;

	return ERR_NONE;
}


static s8 Update_Wave_NoStream ( SNDObj_t *snd){

	//check if the end of the file has been reached
	if(snd->bufpos >= snd->Length){
		SET_FLAG(snd->Flags, AUDIO_STOP);
		RESET_FLAG(snd->Flags, AUDIO_PLAY);
        SET_FLAG(snd->Flags, AUDIO_STREAM_END);
		return ERR_END;
	}


	s16 *AudioSource = snd->No_Stream_Buf + snd->bufpos;

	//convert 8 bits to 16 bits
	if(snd->Bits == 8){
		BAG_Audio_u8tos16(AudioSource,snd->ConvertBuf);
		AudioSource = snd->ConvertBuf;
	}

	//convert mono to stereo if needed
	if(snd->Channels == 1 && GET_FLAG(snd->Flags, AUDIO_FORCESTEREO))
		BAG_Audio_MonoToStereo(AudioSource, snd->AudioBuf);

	else
		snd->AudioBuf = (s16*)AudioSource;

	//update audio buffer position
	snd->bufpos += (snd->buflen>>(1-GET_FLAG(snd->Flags, AUDIO_FORCESTEREO)));
	return ERR_NONE;
}



s8 BAG_Wave_UpdateWav(SNDObj_t *snd){

	if(GET_FLAG(snd->Flags, AUDIO_PLAY)){
		if(NULL == BAG_Audio_s.Buf_Addr)
			goto end;

		//handle streaming
		if(GET_FLAG(snd->Flags, AUDIO_STREAM))
			return Update_Wave_Stream (snd);
		//No streaming
		else
			return Update_Wave_NoStream (snd);
	}
	end:
		return ERR_NONE;
}

#endif //WAV_MODULE
