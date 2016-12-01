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
#include "mp3.h"
#include "mp3helper.h"
#include <debug/libBAG_Debug.h>


void BAG_MP3_Functions(SNDObj_t * snd){
	//setup audio functions
	snd->Update = (void*)&BAG_MP3_UpdateMp3;
	snd->Stop = (void*)&BAG_MP3_StopMp3;
	snd->Play = (void*)&BAG_MP3_PlayMp3;
	snd->Load = (void*)&BAG_MP3_LoadMp3;
}

unsigned int BAG_MP3_getTime(SNDObj_t *snd){
	return MP3GetCurTime(&snd->mp3_container);//get time in seconds
}


void BAG_MP3_StopMp3(SNDObj_t *snd){
	if(GET_FLAG(snd->Flags, AUDIO_PLAY) || GET_FLAG(snd->Flags, AUDIO_PAUSE)){
		SET_FLAG(snd->Flags, AUDIO_STOP);
		RESET_FLAG(snd->Flags, AUDIO_PLAY);
		RESET_FLAG(snd->Flags, AUDIO_PAUSE);

		//reset song position to beginning
		if(GET_FLAG(snd->Flags, AUDIO_STREAM)){
			fclose(snd->File);
			MP3Container * file = &snd->mp3_container;

			mad_header_finish ( &file->mHeader );
			mad_synth_finish ( &file->mSynth );
			mad_frame_finish ( &file->mFrame );
			mad_stream_finish ( &file->mStream );
		}
		BAG_Audio_s.Channel[snd->playChan] = NULL;
		snd->playChan = 0;
	}
}


void BAG_MP3_PlayMp3(SNDObj_t *snd, s8 chan){
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
	if(GET_FLAG(snd->Flags, AUDIO_STOP)){
		MP3Reset (&snd->mp3_container);
		snd->File = fopen (snd->FileName, "rb" );
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


s8 BAG_MP3_LoadMp3(const char *file, SNDObj_t * snd){
	int err = ERR_NONE;
	strcpy(snd->FileName, file);
	snd->File = fopen(file, "rb");


	if(!snd->File){
		BAG_DBG_LibMsg(OPEN_FAIL_MSG, file);
		err = ERR_END;
		return err;
	}
	fclose(snd->File);

	//read header for important info
	snd->Channels = 2;
	snd->Frequency = 44100;
	snd->Bits = 16;//set audio core to output in 16 bit sound
	snd->mp3_container.mOutputSettings.mOutputBits = 16;
	snd->mp3_container.mOutputSettings.mIsStereo = 1;
	snd->mp3_container.mOutputSettings.mIsSequential = 0;
	snd->BytesPerSample = 4;
	snd->readlen = snd->buflen = snd->BytesPerSample * audio_samples_per_trans;
	snd->mp3_container.FilePos = 0;
	snd->format = FORMAT_MP3;

	snd->AudioBuf = (s16*)calloc(snd->buflen,1);
	if(!snd->AudioBuf){
		BAG_DBG_LibMsg(BUFFER_FAILED_MSG, snd->buflen, "Audio");
		err = ERR_END;
	}
	SET_FLAG(snd->Flags,AUDIO_HEADCHCK);
	SET_FLAG(snd->Flags, AUDIO_STOP);
	SET_FLAG(snd->Flags, AUDIO_STREAM);



	BAG_MP3_Functions(snd);
	return err;
}



static s8 _Update_Mp3Buffer(SNDObj_t *snd){
	MP3Container * file = &snd->mp3_container;

	if(file->FilePos >= snd->File_Size)
		return ERR_END;//end of file reached

	if (file->mStream.buffer == NULL ||
		file->mStream.error == MAD_ERROR_BUFLEN) {
		int read_size = 0;
		int remaining = 0;
		unsigned char * read_start = NULL;

		if (file->mStream.next_frame != NULL) {
			remaining = file->mStream.bufend - file->mStream.next_frame;
			memmove(file->mInputBuffer, file->mStream.next_frame, remaining);
			read_start = file->mInputBuffer + remaining;
			read_size = MP3_INPUT_BUFFER_SIZE - remaining;

		} else {
			read_size = MP3_INPUT_BUFFER_SIZE;
			read_start = file->mInputBuffer;
			remaining = 0;
		}

		read_size = fread(read_start, 1, read_size, snd->File);
		file->FilePos += read_size;

		mad_stream_buffer(&file->mStream, file->mInputBuffer, read_size + remaining);
		file->mStream.error = MAD_ERROR_NONE;
	}
	return ERR_NONE;
}


s8 BAG_MP3_UpdateMp3(SNDObj_t *snd){
	if(!GET_FLAG(snd->Flags, AUDIO_PLAY))
		return ERR_NONE;

	if(NULL == BAG_Audio_s.Buf_Addr)
		return ERR_NONE;

	MP3Container * file = &snd->mp3_container;
	do{
		if(_Update_Mp3Buffer(snd) == 1){

			if (mad_frame_decode(&file->mFrame, &file->mStream)) {
				if (MAD_RECOVERABLE(file->mStream.error)) {
					//return 0;
				}
				else if (file->mStream.error == MAD_ERROR_BUFLEN) {
					//return 0;
				}
				else {
				}
			}
			else
				break;
		}
		else{
			SET_FLAG(snd->Flags, AUDIO_STOP);
			RESET_FLAG(snd->Flags, AUDIO_PLAY);
			RESET_FLAG(snd->Flags, AUDIO_PAUSE);

			fclose(snd->File);
			SET_FLAG(snd->Flags, AUDIO_STREAM_END);

			mad_header_finish ( &file->mHeader );
			mad_synth_finish ( &file->mSynth );
			mad_frame_finish ( &file->mFrame );
			mad_stream_finish ( &file->mStream );
			return ERR_END;
		}
	}while(1);

	mad_synth_frame ( &file->mSynth, &file->mFrame );

	s16* dest = snd->AudioBuf;
	OutputMP3 ( file, dest);
	int freq = MP3GetSamplerate (file);
	if( freq != 44100)
		BAG_Audio_ConvertFreq(snd->AudioBuf, snd->AudioBuf,  (BAG_TARGET_FREQUENCY<<8) / freq);

	return ERR_NONE;
}
