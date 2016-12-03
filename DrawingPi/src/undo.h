#ifndef __UNDO_BUF_H__
#define __UNDO_BUF_H__

typedef struct UndoBuffer {
  int levels;
  int curIndex, startIndex, lastIndex;
  unsigned int pixelCount;
  unsigned short **buf;
} UndoBuffer;

void Undo_Init(UndoBuffer *buffer, int undoLevels, unsigned int pixelCount);
void Undo_Add(UndoBuffer *buffer, unsigned short *source);
void Undo_Revert(unsigned short *dest, UndoBuffer *buffer);

#endif //__UNDO_BUF_H__
