#ifndef POINCARE_TREE_POOL_H
#define POINCARE_TREE_POOL_H

#include <omg/memory.h>
#include <stddef.h>
#include <string.h>

#include "pool_object.h"
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

#if __EMSCRIPTEN__
/* Emscripten memory representation assumes loads and stores are aligned.
 * Because the Pool buffer is going to store double values, Object addresses
 * have to be aligned on 8 bytes (provided that emscripten addresses are 8 bytes
 * long which ensures that v-tables are also aligned). */
typedef uint64_t AlignedObjectBuffer;
#else
/* Memory copies are done quicker on 4 bytes aligned data. We force the Pool
 * to allocate 4-byte aligned range to leverage this. */
typedef uint32_t AlignedObjectBuffer;
#endif
constexpr static int ByteAlignment = sizeof(AlignedObjectBuffer);

class PoolHandle;

class Pool final {
  friend class PoolObject;
  friend class PoolHandle;
  friend class PoolCheckpoint;

 public:
  static OMG::GlobalBox<Pool> sharedPool
#if PLATFORM_DEVICE
      __attribute__((section(".bss.$poincare_pool")))
#endif
      ;
  static void Lock() {
#if ASSERTIONS
    s_treePoolLocked = true;
#endif
  }
  static void Unlock() {
#if ASSERTIONS
    s_treePoolLocked = false;
#endif
  }

  Pool() : m_cursor(buffer()) {}

  PoolObject* cursor() const { return reinterpret_cast<PoolObject*>(m_cursor); }

  // Object
  PoolObject* object(uint16_t identifier) const {
    assert(PoolObject::IsValidIdentifier(identifier) &&
           identifier < MaxNumberOfObjects);
    if (m_objectForIdentifierOffset[identifier] != UINT16_MAX) {
      return const_cast<PoolObject*>(reinterpret_cast<const PoolObject*>(
          m_alignedBuffer + m_objectForIdentifierOffset[identifier]));
    }
    return nullptr;
  }

  // Pool memory
  void* alloc(size_t size);
  void move(PoolObject* destination, PoolObject* source);
  PoolObject* copyTreeFromAddress(const void* address, size_t size);

#if POINCARE_TREE_LOG
  void flatLog(std::ostream& stream);
  void treeLog(std::ostream& stream, bool verbose = true);
  __attribute__((__used__)) void log() { treeLog(std::cout, false); }
  __attribute__((__used__)) void verboseLog() { treeLog(std::cout, true); }
#endif
  int numberOfObjects() const;

 private:
#ifdef SMALL_POINCARE_POOL
  constexpr static int BufferSize = 32768;
#else
  /* 32kb (previous size) + 8kb (newly available size)
   * This can still be increased if needed */
  constexpr static int BufferSize = 32768 + 8192;
#endif
  constexpr static int MaxNumberOfObjects = BufferSize / sizeof(PoolObject);
  constexpr static int k_maxObjectOffset = BufferSize / ByteAlignment;
#if ASSERTIONS
  static bool s_treePoolLocked;
#endif
  // Object size, taking the pool alignement into account
  static size_t AlignedSize(size_t size) {
    return OMG::Memory::AlignedSize(size, ByteAlignment);
  }

  // PoolObject
  void discardPoolObject(PoolObject* object);
  void registerObject(PoolObject* object);
  void unregisterObject(PoolObject* object) {
    freeIdentifier(object->identifier());
  }
  void updateObjectForIdentifierFromObject(PoolObject* object);
  void renameObject(PoolObject* object,
                    bool unregisterPreviousIdentifier = true) {
    assert(object->isAfterTopmostCheckpoint());
    object->rename(generateIdentifier(), unregisterPreviousIdentifier);
  }
  // Return next object
  static PoolObject* NextObject(PoolObject* object) {
    /* Simple version would be "return this + 1;", with pointer arithmetics
     * taken care of by the compiler. Unfortunately, we want PoolObject to have
     * a VARIABLE size */
    return reinterpret_cast<PoolObject*>(reinterpret_cast<char*>(object) +
                                         AlignedSize(object->size()));
  }

  // Iterators
  PoolObject* first() const {
    return reinterpret_cast<PoolObject*>(const_cast<char*>(constBuffer()));
  }
  PoolObject* last() const {
    return reinterpret_cast<PoolObject*>(const_cast<char*>(m_cursor));
  }
  class Iterator {
   public:
    Iterator(const PoolObject* object)
        : m_object(const_cast<PoolObject*>(object)) {}
    PoolObject* operator*() { return m_object; }
    Iterator& operator++() {
      m_object = NextObject(m_object);
      return *this;
    }
    bool operator!=(const Iterator& it) const {
      return (m_object != it.m_object);
    }

   protected:
    PoolObject* m_object;
  };

  class Objects final {
   public:
    Objects(PoolObject* object) : m_object(object) {}
    Iterator begin() const { return Iterator(m_object); }
    Iterator end() const { return Iterator(Pool::sharedPool->last()); }

   private:
    PoolObject* m_object;
  };
  Objects allObjects() { return Objects(first()); }

  // Pool memory
  void dealloc(PoolObject* ptr, size_t size);
  void moveObjects(PoolObject* destination, PoolObject* source,
                   size_t moveLength);

  // Identifiers
  uint16_t generateIdentifier() { return m_identifiers.pop(); }
  void freeIdentifier(uint16_t identifier);

  class IdentifierStack final {
   public:
    IdentifierStack() { reset(); }
    void reset();
    void push(uint16_t i);
    uint16_t pop();
    void remove(uint16_t j);
    void resetObjectForIdentifierOffsets(
        uint16_t* objectForIdentifierOffset) const;

   private:
    uint16_t m_currentIndex;
    uint16_t m_availableIdentifiers[MaxNumberOfObjects];
    static_assert(MaxNumberOfObjects < INT16_MAX &&
                      sizeof(m_availableIdentifiers[0]) == sizeof(uint16_t),
                  "Tree object identifiers do not have the right data size.");
  };

  void freePoolFromObject(PoolObject* firstObjectToDiscard);

  char* buffer() { return reinterpret_cast<char*>(m_alignedBuffer); }
  const char* constBuffer() const {
    return reinterpret_cast<const char*>(m_alignedBuffer);
  }
  AlignedObjectBuffer m_alignedBuffer[BufferSize / ByteAlignment];
  char* m_cursor;
  IdentifierStack m_identifiers;
  uint16_t m_objectForIdentifierOffset[MaxNumberOfObjects];
  static_assert(k_maxObjectOffset < UINT16_MAX &&
                    sizeof(m_objectForIdentifierOffset[0]) == sizeof(uint16_t),
                "The tree pool object offsets in m_objectForIdentifierOffset "
                "cannot be written with the chosen data size (uint16_t)");
};

}  // namespace Poincare

#endif
