#ifndef POINCARE_POOL_HANDLE_H
#define POINCARE_POOL_HANDLE_H

#include <initializer_list>

#include "pool.h"

namespace Poincare {

/* A PoolHandle references a PoolObject stored somewhere in the OExpression
 * Pool, and identified by its identifier. Any method that can possibly move the
 * object ("break the this") therefore needs to be implemented in the Handle
 * rather than the Object.
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
      : m_identifier(PoolObject::NoObjectIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }

  PoolHandle(PoolHandle&& tr) : m_identifier(tr.m_identifier) {
    tr.m_identifier = PoolObject::NoObjectIdentifier;
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
    tr.m_identifier = PoolObject::NoObjectIdentifier;
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
    return hasObject(m_identifier) && object() == nullptr;
  }
  int objectRetainCount() const { return object()->retainCount(); }
  size_t size() const;
  void* addressInPool() const { return reinterpret_cast<void*>(object()); }

  bool isUninitialized() const {
    return m_identifier == PoolObject::NoObjectIdentifier;
  }
  bool isDownstreamOf(const PoolObject* treePoolCursor) {
    return !isUninitialized() &&
           (object() == nullptr || object() >= treePoolCursor);
  }

  /* Logging */
#if POINCARE_TREE_LOG
  void log() const;
#endif

  typedef std::initializer_list<PoolHandle> Tuple;

 protected:
  /* Constructor */
  PoolHandle(const PoolObject* object);
  // Un-inlining this constructor actually increases the firmware size
  PoolHandle(uint16_t objectIdentifier = PoolObject::NoObjectIdentifier)
      : m_identifier(objectIdentifier) {
    if (hasObject(objectIdentifier)) {
      object()->retain();
    }
  }

  static PoolHandle Build(PoolObject* object);

  void setIdentifierAndRetain(uint16_t newId);
  void setTo(const PoolHandle& tr);

  static bool hasObject(uint16_t identifier) {
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
