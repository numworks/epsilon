#ifndef ESCHER_ACCORDION_H
#define ESCHER_ACCORDION_H

/* Accordion sequentially stores null-terminated char buffers. It moves the free
 * space at the end of a buffer if it will be edited. */

class Accordion {
public:
  Accordion(char * buffer, int bufferSize);
  int numberOfBuffers();
  const char * bufferAtIndex(int index);
  char * editableBufferAtIndex(int index);
  int sizeOfEditableBufferAtIndex(int index);
  /* sizeOfEditableBufferAtIndex appends the free space at the end of the buffer
   * and returns the length of the buffer plus the free space, minus one free
   * space char that we never edit in order to keep track of the position of the
   * last edited buffer. */
  bool appendBuffer(const char * buffer);
  bool replaceBufferAtIndex(int index, const char * buffer);
  void deleteBufferAtIndex(int index);
  void deleteLastBuffer();
  void deleteAll();
  int freeSpaceSize();
private:
  static constexpr char k_freeSpaceMarker = 0x01;
  int startOfBufferAtIndex(int index);
  int endOfBufferAtIndex(int index);
  int startOfFreeSpace();
  void cleanFreeSpace();
  /* When a buffer is edited, there is garbage after the first null char of the
   * buffer. cleanFreeSpace() declares the space after this null char as free,
   * by marking it with the FreeSpaceMarker. Because we always keep at least one
   * Free Space char out of the editable zone, cleanFreeSpace() just needs to
   * put FreeSpaceMarkers after the first null char of the last edited Buffer,
   * until the first FreeSpaceMarker char.
   * WARNING: We have to call cleanFreeSpace() before any operation on m_history,
   * otherwise m_history might contain garbage chars. */
  void moveFreeSpaceAtPosition(int i);
  void moveFreeSpaceAtEndOfBufferAtIndex(int index);
  void moveFreeSpaceAtEndOfHistory();
  int m_historySize;
  char * m_history;
  /* The m_history variable sequentially stores null-terminated char buffers.
   * It also contains free space, which is marked with the FreeSpaceMarker. By
   * construction, there is always at least one free byte, and the free space is
   * always continuous. */
  int m_numberOfBuffers;
  int m_startOfLastEditedBuffer;
};

#endif
