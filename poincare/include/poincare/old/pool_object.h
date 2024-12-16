#ifndef POINCARE_TREE_NODE_H
#define POINCARE_TREE_NODE_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <strings.h>
#if POINCARE_TREE_LOG
#include <iostream>
#include <ostream>
#endif
#include <omg/memory.h>
#include <poincare/preferences.h>

#include "pool_checkpoint.h"

/* What's in a PoolObject, really?
 *  - a vtable pointer
 *  - an identifier
 *  - a parent identifier
 *  - a reference counter
 */

/* CAUTION: To make node operations faster, the pool needs all addresses and
 * sizes of PoolObjects to be a multiple of 4. */

namespace Poincare {

/* TODO_PCJ: This marks methods that are bound to be removed with new Poincare
 * Indeed, PoolObject will no longer be related, or have parents, or children.*/
#define PCJ_DELETE 1

#if __EMSCRIPTEN__
/* Emscripten memory representation assumes loads and stores are aligned.
 * Because the Pool buffer is going to store double values, Node addresses
 * have to be aligned on 8 bytes (provided that emscripten addresses are 8 bytes
 * long which ensures that v-tables are also aligned). */
typedef uint64_t AlignedNodeBuffer;
#else
/* Memory copies are done quicker on 4 bytes aligned data. We force the Pool
 * to allocate 4-byte aligned range to leverage this. */
typedef uint32_t AlignedNodeBuffer;
#endif
constexpr static int ByteAlignment = sizeof(AlignedNodeBuffer);

class PoolObject {
  friend class Pool;

 public:
  constexpr static uint16_t NoNodeIdentifier = -2;
  // Used for Integer
  constexpr static uint16_t OverflowIdentifier =
      PoolObject::NoNodeIdentifier + 1;

  // Constructor and destructor
  virtual ~PoolObject() {}
  typedef PoolObject *(*const Initializer)(void *);

// Attributes
#if PCJ_DELETE
  void setParentIdentifier(uint16_t parentID) { m_parentIdentifier = parentID; }
  void deleteParentIdentifier() { m_parentIdentifier = NoNodeIdentifier; }
#endif
  virtual size_t size() const = 0;
  uint16_t identifier() const { return m_identifier; }
  int retainCount() const { return m_referenceCounter; }
#if PCJ_DELETE
  size_t deepSize(int realNumberOfChildren) const;
#endif
  // Ghost
  virtual bool isGhost() const { return false; }

  // Node operations
  void setReferenceCounter(int refCount) { m_referenceCounter = refCount; }
  /* Do not increase reference counters outside of the current checkpoint since
   * they won't be decreased if an exception is raised.
   *
   * WARNING: ref counters have a bugged behaviour in this case:
   *  OExpression a = Cosine::Builder();
   *  OExpression b;
   *  PoolCheckpoint() {
   *    ...
   *    b = a;
   *    ...
   *  }
   *
   * Here, the ref counter of the cos is not incremented while a and b point
   * towards it.
   * */
  void retain() { m_referenceCounter += isAfterTopmostCheckpoint(); }
  void release(int currentNumberOfChildren);
  void rename(uint16_t identifier, bool unregisterPreviousIdentifier,
              bool skipChildrenUpdate = false);

  // PoolCheckpoint
  bool isAfterTopmostCheckpoint() const {
    return this >= PoolCheckpoint::TopmostEndOfPool();
  }

// Hierarchy
#if PCJ_DELETE
  virtual int numberOfChildren() const = 0;
  /* The following methods are only used for nodes that have a variable number
   * of children like OList, HorizontalLayout or Randint */
  void deleteParentIdentifierInChildren() const {
    changeParentIdentifierInChildren(NoNodeIdentifier);
  }

  /* Serialization
   * Return the number of chars written, without the null-terminating char. */
  virtual size_t serialize(char *buffer, size_t bufferSize,
                           Preferences::PrintFloatMode floatDisplayMode,
                           int numberOfSignificantDigits) const {
    assert(false);
    return 0;
  }

  template <typename T>
  class Iterator {
   public:
    Iterator(const T *node) : m_node(const_cast<T *>(node)) {}
    T *operator*() { return m_node; }
    bool operator!=(const Iterator &it) const { return (m_node != it.m_node); }

   protected:
    T *m_node;
  };

  template <typename T>
  class Direct final {
   public:
    Direct(const T *node, int firstIndex = 0)
        : m_node(const_cast<T *>(node)), m_firstIndex(firstIndex) {}
    class Iterator : public PoolObject::Iterator<T> {
     public:
      using PoolObject::Iterator<T>::Iterator;
      Iterator &operator++() {
        this->m_node = static_cast<T *>(this->m_node->nextSibling());
        return *this;
      }
    };
    Iterator begin() const {
      PoolObject *n = m_node->next();
      for (int i = 0; i < m_firstIndex; i++) {
        n = n->nextSibling();
      }
      return Iterator(static_cast<T *>(n));
    }
    Iterator end() const {
      return Iterator(static_cast<T *>(m_node->nextSibling()));
    }

    T *node() const { return m_node; }

   private:
    T *m_node;
    int m_firstIndex;
  };

  template <typename T>
  class DepthFirst final {
   public:
    DepthFirst(const PoolObject *node)
        : m_node(const_cast<PoolObject *>(node)) {}
    class Iterator : public PoolObject::Iterator<T> {
     public:
      using PoolObject::Iterator<T>::Iterator;
      Iterator &operator++() {
        this->m_node = this->m_node->next();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node->next()); }
    Iterator end() const { return Iterator(m_node->nextSibling()); }

   private:
    T *m_node;
  };

  Direct<PoolObject> directChildren() const { return Direct<PoolObject>(this); }
  DepthFirst<PoolObject> depthFirstChildren() const {
    return DepthFirst<PoolObject>(this);
  }
#endif

  PoolObject *next() const {
    /* Simple version would be "return this + 1;", with pointer arithmetics
     * taken care of by the compiler. Unfortunately, we want PoolObject to have
     * a VARIABLE size */
    return reinterpret_cast<PoolObject *>(
        reinterpret_cast<char *>(const_cast<PoolObject *>(this)) +
        OMG::Memory::AlignedSize(size(), ByteAlignment));
  }
#if PCJ_DELETE
  PoolObject *nextSibling() const;
#endif

#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream &stream) const = 0;
  virtual void logAttributes(std::ostream &stream) const {}
  void log(std::ostream &stream, bool recursive = true, int indentation = 0,
           bool verbose = true);
  void log() {
    log(std::cout);
    std::cout << std::endl;
  }
#endif

  static bool IsValidIdentifier(uint16_t id) { return id < NoNodeIdentifier; }

 protected:
  PoolObject()
      : m_identifier(NoNodeIdentifier),
        m_parentIdentifier(NoNodeIdentifier),
        m_referenceCounter(0) {}

 private:
#if PCJ_DELETE
  void updateParentIdentifierInChildren() const {
    changeParentIdentifierInChildren(m_identifier);
  }
  void changeParentIdentifierInChildren(uint16_t id) const;
#endif
  uint16_t m_identifier;
#if PCJ_DELETE
  uint16_t m_parentIdentifier;
#endif
  int8_t m_referenceCounter;
};

template <typename T>
static PoolObject *Initializer(void *buffer) {
  return new (buffer) T;
}

}  // namespace Poincare

#endif
