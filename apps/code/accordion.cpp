#include "accordion.h"
#include <assert.h>
#include <string.h>

Accordion::Accordion(char * buffer, int bufferSize) :
  m_historySize(bufferSize),
  m_history(buffer),
  m_numberOfBuffers(0),
  m_startOfLastEditedBuffer(0)
{
  for (int i = 0; i < m_historySize; i ++) {
    m_history[i] = k_freeSpaceMarker;
  }
}

const char * Accordion::bufferAtIndex(int index) {
  assert(index >= 0 && index < numberOfBuffers());
  cleanFreeSpace();
  int startOfBuffer = startOfBufferAtIndex(index);
  return &m_history[startOfBuffer];
}

char * Accordion::editableBufferAtIndex(int index) {
  assert(index >= 0 && index < numberOfBuffers());
  cleanFreeSpace();
  moveFreeSpaceAtEndOfBufferAtIndex(index);
  int startOfBuffer = startOfBufferAtIndex(index);
  return &m_history[startOfBuffer];
}

int Accordion::sizeOfEditableBufferAtIndex(int index) {
  assert(index >= 0 && index < numberOfBuffers());
  cleanFreeSpace();
  moveFreeSpaceAtEndOfBufferAtIndex(index);
  int length = 0;
  for (int i = startOfBufferAtIndex(index); i < m_historySize-1; i++) {
    if (m_history[i] == k_freeSpaceMarker && m_history[i+1] != k_freeSpaceMarker) {
      break;
    }
    length++;
    // We do not count one Free Space Marker, in order to always have at a Free
    // Space of size at least one.
  }
  return length;
}

bool Accordion::appendBuffer(const char * buffer) {
  cleanFreeSpace();
  moveFreeSpaceAtEndOfHistory();
  int len = strlen(buffer);
  if (len + 1 > freeSpaceSize() - 1) { // We keep at keast one Free char.
    return false;
  }
  m_startOfLastEditedBuffer = startOfFreeSpace();
  memcpy(&m_history[m_startOfLastEditedBuffer], buffer, len+1);
  m_numberOfBuffers++;
  return true;
}

bool Accordion::replaceBufferAtIndex(int index, const char * buffer) {
  assert(index >= 0 && index < numberOfBuffers());
  cleanFreeSpace();
  int len = strlen(buffer);
  if (len < sizeOfEditableBufferAtIndex(index)) {
    int startOfOldBuffer = startOfBufferAtIndex(index);
    memcpy(&m_history[startOfOldBuffer], buffer, len+1);
    m_startOfLastEditedBuffer = startOfOldBuffer;
    return true;
  }
  return false;
}

void Accordion::deleteBufferAtIndex(int index) {
  assert(index >= 0 && index < numberOfBuffers());
  cleanFreeSpace();
  moveFreeSpaceAtEndOfBufferAtIndex(index);
  int i = startOfBufferAtIndex(index);
  while (i < m_historySize && m_history[i] != k_freeSpaceMarker) {
    m_history[i] = k_freeSpaceMarker;
    i++;
  }
  m_numberOfBuffers--;
}

void Accordion::deleteLastBuffer() {
  cleanFreeSpace();
  if (m_numberOfBuffers > 0) {
    deleteBufferAtIndex(m_numberOfBuffers-1);
  }
}

void Accordion::deleteAll() {
  cleanFreeSpace();
  for (int i = 0; i < m_historySize; i++){
    m_history[i] = k_freeSpaceMarker;
  }
  m_numberOfBuffers = 0;
}

int Accordion::freeSpaceSize() {
  cleanFreeSpace();
  int sizeOfFreeSpace = 0;
  int freeSpaceStart = startOfFreeSpace();
  for (int i = freeSpaceStart; i < m_historySize; i++) {
    if (m_history[i] == k_freeSpaceMarker) {
      sizeOfFreeSpace++;
    } else {
      return sizeOfFreeSpace;
    }
  }
  return sizeOfFreeSpace;
}

int Accordion::startOfBufferAtIndex(int index) {
  assert(index >= 0 && index < numberOfBuffers());
  cleanFreeSpace();
  int bufferCount = 0;
  int startOfBuffer = 0;
  while (m_history[startOfBuffer] == k_freeSpaceMarker && startOfBuffer < m_historySize) {
    startOfBuffer++;
  }
  for (int i = startOfBuffer; i < m_historySize; i++) {
    if (bufferCount == index) {
      while (m_history[i] == k_freeSpaceMarker && i < m_historySize) {
        i++;
      }
      return i;
    }
    if (m_history[i] == 0) {
      bufferCount++;
    }
  }
  assert(false);
  return 0;
}

int Accordion::endOfBufferAtIndex(int index) {
  assert(index >= 0 && index < numberOfBuffers());
  cleanFreeSpace();
  int startOfBuffer = startOfBufferAtIndex(index);
  for (int i = startOfBuffer; i < m_historySize; i++) {
    if (m_history[i] == 0) {
      return i;
    }
  }
  assert(false);
  return 0;
}

int Accordion::startOfFreeSpace() {
  cleanFreeSpace();
  for (int i = 0; i < m_historySize; i++) {
    if (m_history[i] == k_freeSpaceMarker) {
      return i;
    }
  }
  assert(false);
  return 0;
}

void Accordion::cleanFreeSpace() {
  if (m_history[m_startOfLastEditedBuffer] == k_freeSpaceMarker) {
    return;
  }
  int indexOfCharToChangeIntoFreeSpaceMarker = m_startOfLastEditedBuffer
    + strlen(&m_history[m_startOfLastEditedBuffer]) + 1;
  while (m_history[indexOfCharToChangeIntoFreeSpaceMarker] != k_freeSpaceMarker
      && indexOfCharToChangeIntoFreeSpaceMarker < m_historySize)
  {
    m_history[indexOfCharToChangeIntoFreeSpaceMarker] = k_freeSpaceMarker;
    indexOfCharToChangeIntoFreeSpaceMarker++;
  }
}

void Accordion::moveFreeSpaceAtPosition(int i) {
  assert(i >= 0 && i <= m_historySize);
  cleanFreeSpace();
  int freeSpaceStart = startOfFreeSpace();
  int newFreeSpaceStart = freeSpaceStart;
  if (freeSpaceStart != i){

    // First, move the chars that would be overriden by the free space.
    // The indexes depend on the relative positions of the free space and the
    // new destination.
    int sizeFreeSpace = freeSpaceSize();
    int len = 0;
    int src = 0;
    int dst = 0;
    if (freeSpaceStart > i) {
      len = freeSpaceStart - i;
      src = i;
      dst = i + sizeFreeSpace;
      newFreeSpaceStart = i;
    } else {
      src = freeSpaceStart + sizeFreeSpace;
      len = i - src;
      dst = freeSpaceStart;
      newFreeSpaceStart = i-sizeFreeSpace;
    }
    memmove(&m_history[dst], &m_history[src], len);

    // Then move the free space.
    for (int j = newFreeSpaceStart ; j < newFreeSpaceStart+sizeFreeSpace; j++) {
      m_history[j] = k_freeSpaceMarker;
    }
  }

  m_startOfLastEditedBuffer = newFreeSpaceStart-1;
  while (m_startOfLastEditedBuffer > 0 && m_history[m_startOfLastEditedBuffer-1] != 0 ) {
    m_startOfLastEditedBuffer--;
  }
}

void Accordion::moveFreeSpaceAtEndOfBufferAtIndex(int index) {
  assert(index >= 0 && index < numberOfBuffers());
  cleanFreeSpace();
  int endOfBuffer = endOfBufferAtIndex(index);
  moveFreeSpaceAtPosition(endOfBuffer+1);
}

void Accordion::moveFreeSpaceAtEndOfHistory() {
  cleanFreeSpace();
  if (m_numberOfBuffers > 0) {
    moveFreeSpaceAtEndOfBufferAtIndex(m_numberOfBuffers-1);
  }
}

