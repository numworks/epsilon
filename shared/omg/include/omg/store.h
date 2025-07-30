#pragma once

#include <omg/expiring_pointer.h>
#include <stddef.h>
#include <stdint.h>

namespace OMG {

/* Data structure to store a stack of variable-sized elements in one big buffer.
 * The elements are stored one after another while offsets to reach the end of
 * each element are stored at the end of the buffer, in the opposite direction.
 * By doing so, we can memoize every element entered while not limiting the
 * number of element stored in the buffer.
 *
 * If the remaining space is too small for storing a new element, we delete the
 * oldest one.
 */

/*  Memory layout :
                                                 <- Available space
                                                 for new elements ->
+------------------------------------------------------------------------------+
|           |           |           |           |                  |  |  |  |  |
| Element 3 | Element 2 | Element 1 | Element 0 |                  |o0|o1|o2|o3|
|   Oldest  |           |           |           |                  |  |  |  |  |
+------------------------------------------------------------------------------+
^           ^           ^           ^           ^                              ^
m_buffer    p3          p2          p1          p0                  offsetArea()

with p_i = m_buffer + o_i

*/

// Lock and Unlock are callbacks to disable interruptions during critical parts.

template <auto Lock = []() {}, auto Unlock = []() {}>
class Store {
 public:
  Store(char* buffer, size_t bufferSize);

  int numberOfElements() const { return m_numberOfElements; }
  void* elementAtIndex(int index) const;

  size_t maximumSize() const { return m_bufferSize - sizeof(Offset); }
  size_t remainingSize() const { return spaceForNewElements(endOfElements()); }

  void* pushElement(const void* element, int size);

  /* Make space by clearing some older elements if needed.
   * neededSize must be smaller than maximumSize */
  void makeRoomForElement(size_t neededSize);

  void deleteElementAtIndex(int index);
  void deleteAll() { m_numberOfElements = 0; }

 protected:
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

  // Track an element that has been already copied/constructed in the buffer
  void registerElement(char* element, int size);

 private:
  using Offset = uint16_t;
  char* offsetArea() const {
    return m_buffer + m_bufferSize - m_numberOfElements * sizeof(Offset);
  }
  Offset* offsetArray() const {
    return reinterpret_cast<Offset*>(offsetArea());
  }

  char* const m_buffer;
  const size_t m_bufferSize;
  int m_numberOfElements;
};

}  // namespace OMG

#include <omg/src/store_impl.inc>
