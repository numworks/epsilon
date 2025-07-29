#pragma once

#include <omg/expiring_pointer.h>

#include <cstddef>
#include <cstdint>

namespace OMG {

/* Data structure to store a stack of variable-sized elements in one big buffer.
 * The elements are stored one after another while pointers to the end of each
 * element are stored at the end of the buffer, in the opposite direction.  By
 * doing so, we can memoize every element entered while not limiting the number
 * of element stored in the buffer.
 *
 * If the remaining space is too small for storing a new element, we delete the
 * oldest one.
 */

/*  Memory layout :
                                                 <- Available space
                                                 for new elements ->
+------------------------------------------------------------------------------+
|           |           |           |           |                  |  |  |  |  |
| Element 3 | Element 2 | Element 1 | Element 0 |                  |p0|p1|p2|p3|
|   Oldest  |           |           |           |                  |  |  |  |  |
+------------------------------------------------------------------------------+
^           ^           ^           ^           ^                              ^
m_buffer    p3          p2          p1          p0                 pointerArea()

*/

class Store {
 public:
  Store(char* buffer, size_t bufferSize);

  int numberOfElements() const { return m_numberOfElements; }
  void* elementAtIndex(int index) const;

  size_t bufferSize() const { return m_bufferSize; }
  size_t remainingBufferSize() const {
    return spaceForNewElements(endOfElements()) + sizeof(void*);
  }

  void* pushElement(const void* element, int size);
  void deleteElementAtIndex(int index);
  void deleteAll() { m_numberOfElements = 0; }

 protected:
  char* pointerArea() const {
    return m_buffer + m_bufferSize - m_numberOfElements * sizeof(void*);
  }
  char** pointerArray() const {
    return reinterpret_cast<char**>(pointerArea());
  }
  char* endOfElements() const {
    return numberOfElements() == 0 ? m_buffer : endOfElementAtIndex(0);
  }
  char* endOfElementAtIndex(int index) const;

  /* Account for the size of an additional pointer at the end of the buffer. */
  size_t spaceForNewElements(const char* currentEndOfElements) const;

  void deleteOldestElement() {
    assert(numberOfElements() > 0);
    deleteElementAtIndex(numberOfElements() - 1);
  }

  /* Make space by clearing some older elements if needed.
   * neededSize must be smaller than m_bufferSize */
  void getEmptySpace(size_t neededSize);

  char* const m_buffer;
  const size_t m_bufferSize;
  int m_numberOfElements;
};

}  // namespace OMG
