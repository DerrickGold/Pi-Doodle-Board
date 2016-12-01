#ifndef MP3_HELPER_H
#define MP3_HELPER_H

#include <ds2sdk.h>
#include "libmad/mad.h"
#include <stdarg.h>



/*
 *    Size of buffer for storing compressed mp3 data.
 *    A larger size may stutter more as more samples will have to be decoded each time MP3Loop is called.
 */
#define MP3_INPUT_BUFFER_SIZE 8192

typedef struct _MP3OutputSettings{
	// 16 or 8 bit output.
	int mOutputBits;
	
	// Stereo: 1 = yes, 0 = no.
	int mIsStereo;
	
	// For stereo, are channels laid one after other, or interleaved?
	int mIsSequential;
	
} MP3OutputSettings;


typedef struct _MP3Container{
	struct mad_stream        mStream;
	struct mad_frame         mFrame;
	struct mad_synth         mSynth;
	struct mad_header        mHeader;
	u8            mInputBuffer[ MP3_INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
	MP3OutputSettings        mOutputSettings;
	u32 FilePos;
	u32 BitRatePerByteValue, BitRatePerByteDivCount;
} MP3Container;


struct audio_stats {
  unsigned long clipped_samples;
  mad_fixed_t peak_clipping;
  mad_fixed_t peak_sample;
};

struct audio_dither {
  mad_fixed_t error[3];
  mad_fixed_t random;
};

/*
 *   Returns 0 until samplerate is found.
 */
int  MP3GetSamplerate ( MP3Container *aContainer );

extern void MP3Reset ( MP3Container *aContainer );

extern int OutputMP3 ( MP3Container *aContainer, short *aOutputBuffer);

extern int MP3GetBitrate( MP3Container *aContainer);

extern u32 MP3GetCurTime(MP3Container * aContainer);
#endif
