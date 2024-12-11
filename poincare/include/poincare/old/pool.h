#ifndef POINCARE_TREE_POOL_H
#define POINCARE_TREE_POOL_H

#include <stddef.h>
#include <string.h>

#include <new>

#include "ghost_node.h"
#include "pool_object.h"
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

// TODO_PCJ: See comment in pool_object.h
#define PCJ_DELETE 1

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

  PoolObject *cursor() const {
    return reinterpret_cast<PoolObject *>(m_cursor);
  }

  // Node
  PoolObject *node(uint16_t identifier) const {
    assert(PoolObject::IsValidIdentifier(identifier) &&
           identifier < MaxNumberOfNodes);
    if (m_nodeForIdentifierOffset[identifier] != UINT16_MAX) {
      return const_cast<PoolObject *>(reinterpret_cast<const PoolObject *>(
          m_alignedBuffer + m_nodeForIdentifierOffset[identifier]));
    }
    return nullptr;
  }

  // Pool memory
  void *alloc(size_t size);
  void move(PoolObject *destination, PoolObject *source,
            int realNumberOfSourceChildren);
#if PCJ_DELETE
  void removeChildren(PoolObject *node, int nodeNumberOfChildren);
  void removeChildrenAndDestroy(PoolObject *nodeToDestroy,
                                int nodeNumberOfChildren);
#endif
  PoolObject *deepCopy(PoolObject *node);
  PoolObject *copyTreeFromAddress(const void *address, size_t size);

#if POINCARE_TREE_LOG
  void flatLog(std::ostream &stream);
  void treeLog(std::ostream &stream, bool verbose = true);
  __attribute__((__used__)) void log() { treeLog(std::cout, false); }
  __attribute__((__used__)) void verboseLog() { treeLog(std::cout, true); }
#endif
  int numberOfNodes() const;

 private:
#ifdef SMALL_POINCARE_POOL
  constexpr static int BufferSize = 32768;
#else
  /* 32kb (previous size) + 8kb (newly available size)
   * This can still be increased if needed */
  constexpr static int BufferSize = 32768 + 8192;
#endif
  constexpr static int MaxNumberOfNodes = BufferSize / sizeof(PoolObject);
  constexpr static int k_maxNodeOffset = BufferSize / ByteAlignment;
#if ASSERTIONS
  static bool s_treePoolLocked;
#endif

  // PoolObject
  void discardPoolObject(PoolObject *node);
  void registerNode(PoolObject *node);
  void unregisterNode(PoolObject *node) { freeIdentifier(node->identifier()); }
  void updateNodeForIdentifierFromNode(PoolObject *node);
  void renameNode(PoolObject *node, bool unregisterPreviousIdentifier = true) {
    assert(node->isAfterTopmostCheckpoint());
    node->rename(generateIdentifier(), unregisterPreviousIdentifier);
  }

  // Iterators
  PoolObject *first() const {
    return reinterpret_cast<PoolObject *>(const_cast<char *>(constBuffer()));
  }
  PoolObject *last() const {
    return reinterpret_cast<PoolObject *>(const_cast<char *>(m_cursor));
  }

  class Nodes final {
   public:
    Nodes(PoolObject *node) : m_node(node) {}
    class Iterator : public PoolObject::Iterator<PoolObject> {
     public:
      using PoolObject::Iterator<PoolObject>::Iterator;
      Iterator &operator++() {
        m_node = m_node->next();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node); }
    Iterator end() const { return Iterator(Pool::sharedPool->last()); }

   private:
    PoolObject *m_node;
  };
  Nodes allNodes() { return Nodes(first()); }

#if PCJ_DELETE
  class RootNodes final {
   public:
    RootNodes(PoolObject *node) : m_node(node) {}
    class Iterator : public PoolObject::Iterator<PoolObject> {
     public:
      using PoolObject::Iterator<PoolObject>::Iterator;
      Iterator &operator++() {
        m_node = m_node->nextSibling();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node); }
    Iterator end() const { return Iterator(Pool::sharedPool->last()); }

   private:
    PoolObject *m_node;
  };
  RootNodes roots() { return RootNodes(first()); }
#endif

  // Pool memory
  void dealloc(PoolObject *ptr, size_t size);
  void moveNodes(PoolObject *destination, PoolObject *source,
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
    void resetNodeForIdentifierOffsets(uint16_t *nodeForIdentifierOffset) const;

   private:
    uint16_t m_currentIndex;
    uint16_t m_availableIdentifiers[MaxNumberOfNodes];
    static_assert(MaxNumberOfNodes < INT16_MAX &&
                      sizeof(m_availableIdentifiers[0]) == sizeof(uint16_t),
                  "Tree node identifiers do not have the right data size.");
  };

  void freePoolFromNode(PoolObject *firstNodeToDiscard);

  char *buffer() { return reinterpret_cast<char *>(m_alignedBuffer); }
  const char *constBuffer() const {
    return reinterpret_cast<const char *>(m_alignedBuffer);
  }
  AlignedNodeBuffer m_alignedBuffer[BufferSize / ByteAlignment];
  char *m_cursor;
  IdentifierStack m_identifiers;
  uint16_t m_nodeForIdentifierOffset[MaxNumberOfNodes];
  static_assert(k_maxNodeOffset < UINT16_MAX &&
                    sizeof(m_nodeForIdentifierOffset[0]) == sizeof(uint16_t),
                "The tree pool node offsets in m_nodeForIdentifierOffset "
                "cannot be written with the chosen data size (uint16_t)");
};

}  // namespace Poincare

#endif
