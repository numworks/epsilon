#include <omg/store.h>

#include <cstring>

namespace OMG {

// Public

Store::Store(char* buffer, size_t bufferSize)
    : m_buffer(buffer), m_bufferSize(bufferSize), m_numberOfElements(0) {}

void* Store::elementAtIndex(int index) const {
  assert(0 <= index && index <= numberOfElements() - 1);
  return index == numberOfElements() - 1 ? m_buffer
                                         : endOfElementAtIndex(index + 1);
}

// Private

char* Store::endOfElementAtIndex(int index) const {
  assert(0 <= index && index < numberOfElements());
  char* res = pointerArray()[index];
  /* Make sure the element pointed to is inside the buffer */
  assert(m_buffer <= res && res < m_buffer + m_bufferSize);
  return res;
}

size_t Store::spaceForNewElements(const char* currentEndOfElements) const {
  // Be careful with size_t: negative values are not handled
  return currentEndOfElements + sizeof(void*) < pointerArea()
             ? (pointerArea() - currentEndOfElements) - sizeof(void*)
             : 0;
}

void Store::deleteElementAtIndex(int index) {
  char* deletionStart = index == numberOfElements() - 1
                            ? m_buffer
                            : endOfElementAtIndex(index + 1);
  char* deletionEnd = endOfElementAtIndex(index);
  assert(deletionEnd >= deletionStart);
  size_t deletedSize = deletionEnd - deletionStart;
  assert(endOfElements() >= deletionEnd);
  size_t shiftedMemorySize = endOfElements() - deletionEnd;

  // TODO
  // Ion::CircuitBreaker::lock();
  std::memmove(deletionStart, deletionEnd, shiftedMemorySize);

  for (int i = index - 1; i >= 0; i--) {
    pointerArray()[i + 1] = pointerArray()[i] - deletedSize;
  }
  m_numberOfElements--;
  // Ion::CircuitBreaker::unlock();
}

void Store::getEmptySpace(size_t neededSize) {
  assert(neededSize < m_bufferSize);
  while (remainingBufferSize() < neededSize) {
    deleteOldestElement();
  }
}

void* Store::pushElement(const void* element, int size) {
  char* destination = endOfElements();
  assert(destination >= m_buffer &&
         destination + size + sizeof(void*) <= pointerArea());

  std::memcpy(destination, element, size);
  registerElement(destination, size);
  return destination;
}

void Store::registerElement(char* element, int size) {
  // Write the pointer to the new element at pointerArea()
  pointerArray()[-1] = element + size;

  /* Now that the element is copied, we can finally update
   * m_numberOfElements. As that is the only variable tracking the state of the
   * store, updating it only at the end of the push ensures that, should an
   * interruption occur, all the temporary states are silently discarded and no
   * ill-formed Element is stored. */
  m_numberOfElements++;
  assert(elementAtIndex(0) == element);
}

}  // namespace OMG
