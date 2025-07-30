#include <omg/store.h>
#include <string.h>

#include <type_traits>

namespace OMG {

template <auto L, auto U>
Store<L, U>::Store(char* buffer, size_t bufferSize)
    : m_buffer(buffer), m_bufferSize(bufferSize), m_numberOfElements(0) {
  static_assert(std::is_same_v<offset_t, uint16_t>);
  assert(bufferSize < UINT16_MAX);
}

template <auto L, auto U>
void* Store<L, U>::elementAtIndex(int index) const {
  assert(0 <= index && index <= numberOfElements() - 1);
  return index == numberOfElements() - 1 ? m_buffer
                                         : endOfElementAtIndex(index + 1);
}

template <auto L, auto U>
void Store<L, U>::makeRoomForElement(size_t neededSize) {
  assert(neededSize < m_bufferSize);
  while (remainingSize() < neededSize) {
    deleteOldestElement();
  }
}

template <auto L, auto U>
char* Store<L, U>::endOfElementAtIndex(int index) const {
  assert(0 <= index && index < numberOfElements());
  char* res = m_buffer + offsetArray()[index];
  /* Make sure the element pointed to is inside the buffer */
  assert(m_buffer <= res && res < m_buffer + m_bufferSize);
  return res;
}

template <auto L, auto U>
size_t Store<L, U>::spaceForNewElements(
    const char* currentEndOfElements) const {
  // Be careful with size_t: negative values are not handled
  return currentEndOfElements + sizeof(offset_t) < offsetArea()
             ? (offsetArea() - currentEndOfElements) - sizeof(offset_t)
             : 0;
}

template <auto Lock, auto Unlock>
void Store<Lock, Unlock>::deleteElementAtIndex(int index) {
  char* deletionStart = index == numberOfElements() - 1
                            ? m_buffer
                            : endOfElementAtIndex(index + 1);
  char* deletionEnd = endOfElementAtIndex(index);
  assert(deletionEnd >= deletionStart);
  size_t deletedSize = deletionEnd - deletionStart;
  assert(endOfElements() >= deletionEnd);
  size_t shiftedMemorySize = endOfElements() - deletionEnd;

  Lock();
  memmove(deletionStart, deletionEnd, shiftedMemorySize);

  for (int i = index - 1; i >= 0; i--) {
    offsetArray()[i + 1] = offsetArray()[i] - deletedSize;
  }
  m_numberOfElements--;
  Unlock();
}

template <auto L, auto U>
void* Store<L, U>::pushElement(const void* element, int size) {
  char* destination = endOfElements();
  assert(destination >= m_buffer &&
         destination + size + sizeof(offset_t) <= offsetArea());

  memcpy(destination, element, size);
  registerElement(destination, size);
  return destination;
}

template <auto L, auto U>
void Store<L, U>::registerElement(char* element, int size) {
  assert(m_buffer <= element && element + size <= m_buffer + m_bufferSize);
  // Write the pointer to the new element at pointerArea()
  offsetArray()[-1] = element + size - m_buffer;

  /* Now that the element is copied, we can finally update
   * m_numberOfElements. As that is the only variable tracking the state of the
   * store, updating it only at the end of the push ensures that, should an
   * interruption occur, all the temporary states are silently discarded and no
   * ill-formed Element is stored. */
  m_numberOfElements++;
  assert(elementAtIndex(0) == element);
}

}  // namespace OMG
