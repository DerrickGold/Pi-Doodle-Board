#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "undo.h"


void Undo_Init(UndoBuffer *buffer, int undoLevels, unsigned int pixelCount) {
  buffer->levels = undoLevels;
  buffer->curIndex = 0;
  buffer->startIndex = 0;
  buffer->lastIndex = buffer->levels - 1;
  buffer->pixelCount = pixelCount;

  buffer->buf = calloc(buffer->levels, sizeof(unsigned short *));
  if (!buffer->buf) {
    fprintf(stderr, "Error allocating undo buffers\n");
    exit(-1);
  }

  for (int i = 0; i < buffer->levels; i++) {
    buffer->buf[i] = calloc(buffer->pixelCount, sizeof(unsigned short));
    if (!buffer->buf[i]) {
      fprintf(stderr, "Error allocating undo buffer level: %d\n", i);
      exit(-1);
    }
  }

}

void Undo_Add(UndoBuffer *buffer, unsigned short *source) {
  buffer->curIndex++;
  if (buffer->curIndex > buffer->lastIndex) buffer->curIndex = 0;
  if (buffer->curIndex == buffer->startIndex)
    buffer->startIndex = (buffer->startIndex + 1 > buffer->lastIndex) ? 0 : buffer->startIndex + 1;

  memcpy(buffer->buf[buffer->curIndex], source, buffer->pixelCount * sizeof(unsigned short));
}

void Undo_Revert(unsigned short *dest, UndoBuffer *buffer) {
  if (buffer->curIndex == buffer->startIndex) return;
  buffer->curIndex--;
  if (buffer->curIndex < 0) buffer->curIndex = buffer->lastIndex;
  memcpy(dest, buffer->buf[buffer->curIndex], buffer->pixelCount * sizeof(unsigned short));
}
