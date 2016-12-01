#include "mp3helper.h"


void MP3Reset ( MP3Container *aContainer )
{
	mad_stream_init ( &aContainer->mStream );
	mad_frame_init ( &aContainer->mFrame );
	mad_synth_init ( &aContainer->mSynth );
	mad_header_init ( &aContainer->mHeader );
}


struct audio_stats Audio_stats;
struct audio_dither Audio_dither;

/*
 * NAME:	prng()
 * DESCRIPTION:	32-bit pseudo-random number generator
 */
static unsigned long prng(unsigned long state)
{
  return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

/*
 * NAME:	audio_linear_dither()
 * DESCRIPTION:	generic linear sample quantize and dither routine
 */
static signed long audio_linear_dither(unsigned int bits, mad_fixed_t sample,
				struct audio_dither *dither,
				struct audio_stats *stats)
{
  unsigned int scalebits = 0;
  mad_fixed_t output = 0, mask = 0, random = 0;

  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

  /* noise shape */
  sample += dither->error[0] - dither->error[1] + dither->error[2];

  dither->error[2] = dither->error[1];
  dither->error[1] = dither->error[0] / 2;

  /* bias */
  output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));

  scalebits = MAD_F_FRACBITS + 1 - bits;
  mask = (1L << scalebits) - 1;

  /* dither */
  random  = prng(dither->random);
  output += (random & mask) - (dither->random & mask);

  dither->random = random;

  /* clip */
  if (output >= stats->peak_sample) {
    if (output > MAX) {
      ++stats->clipped_samples;
      if (output - MAX > stats->peak_clipping)
	stats->peak_clipping = output - MAX;

      output = MAX;

      if (sample > MAX)
	sample = MAX;
    }
    stats->peak_sample = output;
  }
  else if (output < -stats->peak_sample) {
    if (output < MIN) {
      ++stats->clipped_samples;
      if (MIN - output > stats->peak_clipping)
	stats->peak_clipping = MIN - output;

      output = MIN;

      if (sample < MIN)
	sample = MIN;
    }
    stats->peak_sample = -output;
  }

  /* quantize */
  output &= ~mask;

  /* error feedback */
  dither->error[0] = sample - output;

  /* scale */
  return output >> scalebits;
}


int OutputMP3 ( MP3Container *aContainer, s16 *aOutputBuffer){
	struct mad_pcm *pcm = &aContainer->mSynth.pcm;
	
	
	int channels = pcm->channels;
	int nsamples  = pcm->length;
	int numOut = 0;
	int i = 0;
	
	mad_fixed_t const *left_ch   = pcm->samples[ 0 ];
	mad_fixed_t const *right_ch  = pcm->samples[ 1 ];
	
	memset(&Audio_stats,0,sizeof(Audio_stats));
	memset(&Audio_dither,0,sizeof(Audio_dither));
			
	switch(channels)
	{
		default://default is stereo
			for ( i = 0; i < nsamples; i++ )
			{
				aOutputBuffer[ numOut++ ] = audio_linear_dither(16, (*left_ch++),&Audio_dither,&Audio_stats);
				aOutputBuffer[ numOut++ ] = audio_linear_dither(16, (*right_ch++),&Audio_dither,&Audio_stats);
			}
		break;
		case 1:
			while ( nsamples-- )
			{
				s16 Sample = audio_linear_dither(16, (*left_ch++),&Audio_dither,&Audio_stats);
				aOutputBuffer[ numOut++ ] = Sample;
				aOutputBuffer[ numOut++ ] = Sample;
			}
			
		break;
	}
	return pcm->length;
}

int  MP3GetSamplerate ( MP3Container *aContainer ){
	return aContainer->mFrame.header.samplerate;
}

int MP3GetBitrate( MP3Container *aContainer){
	return aContainer->mFrame.header.bitrate;
}

unsigned int MP3GetFilePos(MP3Container *aContainer){
	return aContainer->FilePos;
}

unsigned int MP3BitratePerByte( MP3Container * aContainer){
	return	MP3GetBitrate(aContainer)/8;
}

unsigned int MP3GetBitratePerByte(MP3Container *aContainer){
    if(aContainer->BitRatePerByteDivCount != 0){
      unsigned int BitRatePerByte = MP3BitratePerByte(aContainer);
      aContainer->BitRatePerByteValue+=BitRatePerByte;
      aContainer->BitRatePerByteDivCount++;
      // _consolePrintf("%d,%d kbyteps\n",PlugMP3_BitRatePerByteValue/PlugMP3_BitRatePerByteDivCount,BitRatePerByte);
      return(aContainer->BitRatePerByteValue/aContainer->BitRatePerByteDivCount);
    }
	return 0;
}

u32 MP3GetCurTime(MP3Container * aContainer){
	unsigned int  BitRatePerByte = MP3GetBitratePerByte(aContainer);
  if(BitRatePerByte==0) 
    return(0);
  unsigned int pos=MP3GetFilePos(aContainer);
  unsigned int sec=pos/BitRatePerByte;
  return(sec);
}

