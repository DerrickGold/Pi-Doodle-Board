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

#ifndef __WAVE_HEADER_H__
#define __WAVE_HEADER_H__

/* Definitions for Microsoft WAVE format */

#define RIFF		0x46464952
#define WAVE		0x45564157
#define FMT			0x20746D66
#define DATA		0x61746164
#define PCM_CODE	1
#define WAVE_MONO	1
#define WAVE_STEREO	2



typedef struct _waveheader {
	unsigned int	main_chunk;		//'RIFF'
	unsigned int	length;			//filelen
	unsigned int	chunk_type;		//'WAVE'

	unsigned int	sub_chunk;		//'fmt '
	unsigned int	sc_len;			//length of sub_chunk, =16
	unsigned short	format;			//should be 1 for PCM-code
	unsigned short	modus;			//1 Mono, 2 Stereo
	unsigned int	sample_fq;		//frequence of sample
	unsigned int	byte_p_sec;
	unsigned short	byte_p_spl;		//samplesize; 1 or 2 bytes
	unsigned short	bit_p_spl;		//8, 12 or 16 bit

	unsigned int	data_chunk;		//'data'
	unsigned int	data_length;	//samplecount
} WaveHeader;

typedef struct {
    int magic;						//must be equal to SND_MAGIC
    int dataLocation;				//Offset or pointer to the raw data
    int dataSize;					//Number of bytes of data in the raw data
    int dataFormat;					//The data formate coding
    int samplingRate;				//The sampling rate
    int channelCount;				//The number of channels
} SndHeader;

#define SND_MAGIC ((long int)0x2e736e64)

#define SND_FORMAT_UNSPECIFIED          (0)
#define SND_FORMAT_MULAW_8              (1)
#define SND_FORMAT_LINEAR_8             (2)
#define SND_FORMAT_LINEAR_16            (3)
#define SND_FORMAT_LINEAR_24            (4)
#define SND_FORMAT_LINEAR_32            (5)
#define SND_FORMAT_FLOAT                (6)


#endif //__WAVE_H__

#endif //WAV_MODULE
