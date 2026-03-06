#include <omg/memory.h>
#include <omg/unreachable.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/pool.h>
#include <poincare/pool_checkpoint.h>
#include <poincare/pool_handle.h>
#include <stdint.h>
#include <string.h>

namespace Poincare {

#if ASSERTIONS
bool Pool::s_treePoolLocked = false;
#endif

OMG::TrackedGlobalBox<Pool> Pool::sharedPool;

void Pool::freeIdentifier(uint16_t identifier) {
  if (PoolObject::IsValidIdentifier(identifier) &&
      identifier < MaxNumberOfObjects) {
    m_objectOffset[identifier] = UINT16_MAX;
  }
}

// Find the first identifier available in the object offset table
uint16_t Pool::generateIdentifier() {
  for (uint16_t i = 0; i < MaxNumberOfObjects; i++) {
    if (m_objectOffset[i] == UINT16_MAX) {
      return i;
    }
  }
  /* No identifier is available. This should not happen since the number of
   * objects in the pool should be limited by the number of identifiers. */
  OMG_UNREACHABLE;
}

void Pool::move(PoolObject* destination, PoolObject* source) {
  moveObjects(destination, source, AlignedSize(source->size()));
}

PoolObject* Pool::copyObjectFromAddress(const void* address, size_t size) {
  void* ptr = alloc(size);
  memcpy(ptr, address, size);
  PoolObject* copy = reinterpret_cast<PoolObject*>(ptr);
  renameObject(copy, false);
  return copy;
}

void Pool::moveObjects(PoolObject* destination, PoolObject* source,
                       size_t moveSize) {
  assert(destination->isAfterTopmostCheckpoint());
  assert(source->isAfterTopmostCheckpoint());
  assert(moveSize % 4 == 0);
  assert((((uintptr_t)source) % 4) == 0);
  assert((((uintptr_t)destination) % 4) == 0);

#if ASSERTIONS
  assert(!s_treePoolLocked);
#endif

  uint32_t* src = reinterpret_cast<uint32_t*>(source);
  uint32_t* dst = reinterpret_cast<uint32_t*>(destination);
  size_t len = moveSize / 4;

  if (OMG::Memory::Rotate(dst, src, len)) {
    updateObjectOffsetsFromObject(dst < src ? destination : source);
  }
}

#if POINCARE_TREE_LOG
void Pool::flatLog(std::ostream& stream) {
  size_t size = static_cast<char*>(m_cursor) - static_cast<char*>(buffer());
  stream << "<Pool format=\"flat\" size=\"" << size << "\">";
  for (PoolObject* object : allObjects()) {
    object->log(stream);
  }
  stream << "</Pool>";
  stream << std::endl;
}

void Pool::treeLog(std::ostream& stream, bool verbose) {
  stream << "<Pool format=\"tree\" size=\"" << (int)(m_cursor - buffer())
         << "\">";
  for (PoolObject* object : allObjects()) {
    object->log(stream, 1, verbose);
  }
  stream << std::endl;
  stream << "</Pool>";
  stream << std::endl;
}

#endif

int Pool::numberOfObjects() const {
  int count = 0;
  PoolObject* firstObject = first();
  PoolObject* lastObject = last();
  while (firstObject != lastObject) {
    count++;
    firstObject = NextObject(firstObject);
  }
  return count;
}

void* Pool::alloc(size_t size) {
  assert(last()->isAfterTopmostCheckpoint());
#if ASSERTIONS
  assert(!s_treePoolLocked);
#endif

  size = AlignedSize(size);
  if (m_cursor + size > buffer() + BufferSize) {
    ExceptionCheckpoint::Raise();
  }
  void* result = m_cursor;
  m_cursor += size;
  return result;
}

void Pool::dealloc(PoolObject* object, size_t size) {
  assert(object->isAfterTopmostCheckpoint());
#if ASSERTIONS
  assert(!s_treePoolLocked);
#endif

  size = AlignedSize(size);
  char* ptr = reinterpret_cast<char*>(object);
  assert(ptr >= buffer() && ptr < m_cursor);

  // Step 1 - Compact the pool
  memmove(ptr, ptr + size, m_cursor - (ptr + size));
  m_cursor -= size;

  // Step 2: Update m_objectOffset for all objects downstream
  updateObjectOffsetsFromObject(object);
}

void Pool::discardPoolObject(PoolObject* object) {
  uint16_t objectIdentifier = object->identifier();
  size_t size = object->size();
  object->~PoolObject();
  dealloc(object, size);
  freeIdentifier(objectIdentifier);
}

void Pool::registerObject(PoolObject* object) {
  uint16_t objectID = object->identifier();
  assert(objectID < MaxNumberOfObjects);
  const int objectOffset =
      (((char*)object) - (char*)m_alignedBuffer) / ByteAlignment;
  // Check that the offset can be stored in a uint16_t
  assert(objectOffset < k_maxObjectOffset);
  m_objectOffset[objectID] = objectOffset;
}

void Pool::updateObjectOffsetsFromObject(PoolObject* object) {
  for (PoolObject* n : Objects(object)) {
    registerObject(n);
  }
}

void Pool::resetObjectOffsets() {
  for (uint16_t i = 0; i < MaxNumberOfObjects; i++) {
    m_objectOffset[i] = UINT16_MAX;
  }
}

// Discard all objects after firstObjectToDiscard
void Pool::freePoolFromObject(PoolObject* firstObjectToDiscard) {
  assert(firstObjectToDiscard != nullptr);
  assert(firstObjectToDiscard >= first());
  assert(firstObjectToDiscard <= last());

  // Free all identifiers
  resetObjectOffsets();
  PoolObject* currentObject = first();
  while (currentObject < firstObjectToDiscard) {
    registerObject(currentObject);
    currentObject = NextObject(currentObject);
  }
  assert(currentObject == firstObjectToDiscard);
  m_cursor = reinterpret_cast<char*>(currentObject);
  // TODO: Assert that no tree continues into the discarded pool zone
}

}  // namespace Poincare
