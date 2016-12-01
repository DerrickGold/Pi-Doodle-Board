

CC = gcc
AR = ar


LIBS := -lc -lm -lgcc -lrt -lX11
EXTLIBS := 

#cflags
CFLAGS := -std=gnu99 -Wall -Wno-char-subscripts -O3


#some modules to enable for the library
#some audio modules
AUDIO_OGG := NO
AUDIO_MP3 := NO
AUDIO_WAV := NO
#or you can disable audio all together
ENABLE_AUDIO := NO

#now for graphic modules
FILE_JPG := YES
FILE_BMP := YES
FILE_PNG := YES

TOUCHSCREEN := YES


ifeq ($(AUDIO_OGG), NO)
	ifeq ($(AUDIO_MP3), NO)
		ifeq ($(AUDIO_WAV), NO)
			ENABLE_AUDIO = NO
		endif
	endif
endif

#folders with sources
SOURCES = include \
		include/debug \
		include/strings \
		src \
		src/Filesystem \
		src/Display \

#now for the modules
#SOURCES += src/inpaut
CFLAGS +=

ifeq ($(TOUCHSCREEN), YES)
	SOURCES += src/input
	CFLAGS += -DTOUCHSCREEN
endif

#no audio support for sdl compiles yet
ifeq ($(ENABLE_AUDIO), YES)
	SOURCES += 	src/Audio
	CFLAGS += -DCOMPILE_AUDIO

	ifeq ($(AUDIO_WAV), YES)
		SOURCES += src/Audio/WAVE
		CFLAGS += -DWAV_MODULE
	endif

	ifeq ($(AUDIO_OGG), YES)
		SOURCES +=	src/Audio/OGG \
					src/Audio/OGG/tremor
		CFLAGS += -DOGG_MODULE
	endif

	ifeq ($(AUDIO_MP3), YES)
		SOURCES += src/Audio/MP3 \
					src/Audio/MP3/libmad
		CFLAGS +=-DMP3_MODULE
	endif
endif#end audio check


#graphic formats
ifeq ($(FILE_JPG), YES)
	SOURCES += 	src/Display/Jpeg
	CFLAGS += -DJPG_MODULE
endif

ifeq ($(FILE_BMP), YES)
	SOURCES += 	src/Display/Bmp
	CFLAGS += -DBMP_MODULE
endif

ifeq ($(FILE_PNG), YES)
	SOURCES += 	src/Display/Png
	CFLAGS += -DPNG_MODULE
endif



INCLUDES = include include/debug


INC := -I$(CORE_DIR)\
			$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(SOURCES),-I$(CURDIR)/$(dir)) \

CSRC := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))



COBJS	:= $(CSRC:.c=.o)
LIBNAME := libbag.a

all : depend $(LIBNAME)

$(LIBNAME) : $(COBJS) $(SOBJS)
	$(AR) -rcsv $@ $(COBJS) $(SOBJS)
	cp $@ ../lib

.c.o:
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

clean :
	rm -rf $(COBJS) $(SOBJS) *.a
	rm -f depend

depend:	Makefile
	$(CC) -MM $(CFLAGS) $(INC) $(SSRC) $(CSRC) > $@

sinclude depend

