#ifndef POINCARE_TREE_HANDLE_H
#define POINCARE_TREE_HANDLE_H

#include <initializer_list>

#include "pool.h"

namespace Poincare {

// TODO_PCJ: See comment in pool_object.h
#define PCJ_DELETE 1

/* A PoolHandle references a PoolObject stored somewhere is the OExpression
 * Pool, and identified by its identifier. Any method that can possibly move the
 * object ("break the this") therefore needs to be implemented in the Handle
 * rather than the Node.
 */
class PoolHandle {
  friend class PoolObject;
  friend class Pool;
  friend class Ghost;

 public:
  /* Constructors  */
  /* PoolHandle constructors that take only one argument and this argument is
   * a PoolHandle should be marked explicit. This prevents the code from
   * compiling with, for instance: Logarithm l = clone() (which would be
   * equivalent to Logarithm l = Logarithm(clone())). */
  PoolHandle(const PoolHandle& tr)
      : m_identifier(PoolObject::NoNodeIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }

  PoolHandle(PoolHandle&& tr) : m_identifier(tr.m_identifier) {
    tr.m_identifier = PoolObject::NoNodeIdentifier;
  }

  ~PoolHandle() { release(m_identifier); }

  /* Operators */
  PoolHandle& operator=(const PoolHandle& tr) {
    setTo(tr);
    return *this;
  }

  PoolHandle& operator=(PoolHandle&& tr) {
    release(m_identifier);
    m_identifier = tr.m_identifier;
    tr.m_identifier = PoolObject::NoNodeIdentifier;
    return *this;
  }

  /* Comparison */
  inline bool operator==(const PoolHandle& t) const {
    return m_identifier == t.identifier();
  }
  inline bool operator!=(const PoolHandle& t) const {
    return m_identifier != t.identifier();
  }

  /* Clone */
  PoolHandle clone() const;

  uint16_t identifier() const { return m_identifier; }
  PoolObject* object() const;
  bool wasErasedByException() const {
    return hasNode(m_identifier) && object() == nullptr;
  }
  int nodeRetainCount() const { return object()->retainCount(); }
  size_t size() const;
  void* addressInPool() const { return reinterpret_cast<void*>(object()); }

  bool isGhost() const { return object()->isGhost(); }
  bool isUninitialized() const {
    return m_identifier == PoolObject::NoNodeIdentifier;
  }
  bool isDownstreamOf(PoolObject* treePoolCursor) {
    return !isUninitialized() &&
           (object() == nullptr || object() >= treePoolCursor);
  }

/* Hierarchy */
#if PCJ_DELETE
  int numberOfChildren() const { return object()->numberOfChildren(); }
  void setParentIdentifier(uint16_t id) { object()->setParentIdentifier(id); }
  void deleteParentIdentifier() { object()->deleteParentIdentifier(); }
  void deleteParentIdentifierInChildren() {
    object()->deleteParentIdentifierInChildren();
  }
#endif

  /* Logging */
#if POINCARE_TREE_LOG
  void log() const;
#endif

  typedef std::initializer_list<PoolHandle> Tuple;

 protected:
  /* Constructor */
  PoolHandle(const PoolObject* node);
  // Un-inlining this constructor actually increases the firmware size
  PoolHandle(uint16_t nodeIdentifier = PoolObject::NoNodeIdentifier)
      : m_identifier(nodeIdentifier) {
    if (hasNode(nodeIdentifier)) {
      object()->retain();
    }
  }

#if PCJ_DELETE
  static PoolHandle BuildWithGhostChildren(PoolObject* node);
#endif

  void setIdentifierAndRetain(uint16_t newId);
  void setTo(const PoolHandle& tr);

  static bool hasNode(uint16_t identifier) {
    return PoolObject::IsValidIdentifier(identifier);
  }

  uint16_t m_identifier;

 private:
  template <class U>
  static PoolHandle Builder();

  void release(uint16_t identifier);
};

}  // namespace Poincare

#endif
