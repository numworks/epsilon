#include <omg/store.h>
#include <string.h>

#include <type_traits>

namespace OMG {

template <auto L, auto U>
Store<L, U>::Store(char* buffer, size_t bufferSize)
    : m_buffer(buffer), m_bufferSize(bufferSize), m_numberOfElements(0) {
  static_assert(std::is_same_v<Offset, uint16_t>);
  assert(bufferSize < UINT16_MAX);
}

template <auto L, auto U>
void* Store<L, U>::elementAtIndex(int index) const {
  assert(0 <= index && index <= numberOfElements() - 1);
  return index == 0 ? m_buffer : endOfElementAtIndex(index - 1);
}

template <auto L, auto U>
void Store<L, U>::makeRoomForElement(size_t neededSize) {
  assert(neededSize < maximumSize());
  while (remainingSize() < neededSize) {
    deleteOldestElement();
  }
}

template <auto L, auto U>
char* Store<L, U>::endOfElementAtIndex(int index) const {
  assert(0 <= index && index < numberOfElements());
  char* res = m_buffer + offset(index);
  /* Make sure the element pointed to is inside the buffer */
  assert(m_buffer <= res && res < m_buffer + m_bufferSize);
  return res;
}

template <auto L, auto U>
size_t Store<L, U>::spaceForNewElements(
    const char* currentEndOfElements) const {
  // Be careful with size_t: negative values are not handled
  return currentEndOfElements + sizeof(Offset) < offsetArea()
             ? (offsetArea() - currentEndOfElements) - sizeof(Offset)
             : 0;
}

template <auto Lock, auto Unlock>
void Store<Lock, Unlock>::deleteElementAtIndex(int index) {
  char* deletionStart = static_cast<char*>(elementAtIndex(index));
  char* deletionEnd = endOfElementAtIndex(index);
  assert(deletionEnd > deletionStart);
  size_t deletedSize = deletionEnd - deletionStart;
  assert(endOfElements() >= deletionEnd);
  size_t shiftedMemorySize = endOfElements() - deletionEnd;

  Lock();
  memmove(deletionStart, deletionEnd, shiftedMemorySize);

  for (int i = index; i < m_numberOfElements - 1; i++) {
    offset(i) = offset(i + 1) - deletedSize;
  }
  m_numberOfElements--;
  Unlock();
}

template <auto Lock, auto Unlock>
void Store<Lock, Unlock>::makeElementLast(int index, char* workBuffer) {
  assert(workBuffer);

  char* element = static_cast<char*>(elementAtIndex(index));
  size_t size = endOfElementAtIndex(index) - element;
  memcpy(workBuffer, element, size);
  deleteElementAtIndex(index);
  pushElement(workBuffer, size);
}

template <auto Lock, auto Unlock>
void Store<Lock, Unlock>::updateElementAtIndex(int index, const void* element,
                                               int size) {
  char* deletionStart = static_cast<char*>(elementAtIndex(index));
  char* deletionEnd = endOfElementAtIndex(index);
  assert(deletionEnd >= deletionStart);
  size_t deletedSize = deletionEnd - deletionStart;
  assert(endOfElements() >= deletionEnd);
  size_t shiftedMemorySize = endOfElements() - deletionEnd;

  Lock();
  memmove(deletionStart + size, deletionEnd, shiftedMemorySize);
  memcpy(deletionStart, element, size);

  for (int i = index; i < m_numberOfElements; i++) {
    offset(i) = offset(i) - deletedSize + size;
  }
  Unlock();
}

template <auto L, auto U>
void* Store<L, U>::pushElement(const void* element, int size) {
  char* destination = endOfElements();
  assert(destination >= m_buffer &&
         destination + size + sizeof(Offset) <= offsetArea());

  memcpy(destination, element, size);
  registerElement(destination, size);
  return destination;
}

template <auto L, auto U>
void Store<L, U>::registerElement(char* element, int size) {
  assert(m_buffer <= element && element + size <= m_buffer + m_bufferSize);
  // Write the pointer to the new element at pointerArea()
  offset(m_numberOfElements) = element + size - m_buffer;

  /* Now that the element is copied, we can finally update
   * m_numberOfElements. As that is the only variable tracking the state of the
   * store, updating it only at the end of the push ensures that, should an
   * interruption occur, all the temporary states are silently discarded and no
   * ill-formed Element is stored. */
  m_numberOfElements++;
  assert(elementAtIndex(numberOfElements() - 1) == element);
}

}  // namespace OMG
