#pragma once

#include <initializer_list>

#include "pool.h"

namespace Poincare {

/* A PoolHandle references a PoolObject stored somewhere in the Pool
 * and identified by its identifier. Any method that can possibly move the
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
  PoolHandle(const PoolHandle& tr) : PoolHandle() { setTo(tr); }

  PoolHandle(PoolHandle&& tr) : PoolHandle() { transferFrom(tr); }

  ~PoolHandle() { release(); }

  /* Operators */
  PoolHandle& operator=(const PoolHandle& tr) {
    setTo(tr);
    return *this;
  }

  PoolHandle& operator=(PoolHandle&& tr) {
    transferFrom(tr);
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
    return !isUninitialized() && object() == nullptr;
  }
  int objectRetainCount() const { return object()->retainCount(); }
  size_t size() const;
  void* addressInPool() const { return reinterpret_cast<void*>(object()); }

  bool isUninitialized() const {
    return !PoolObject::IsValidIdentifier(m_identifier);
  }
  bool isDownstreamOf(const PoolObject* treePoolCursor) const {
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
    retain();
  }

  static PoolHandle Build(PoolObject* object);

  void setIdentifierAndRetain(uint16_t newId);
  // Copies identifier and retains
  void setTo(const PoolHandle& tr);
  /* Transfers ownership of the identifier, releasing the previous one and
   * retaining the new one. */
  void transferFrom(PoolHandle& tr);

  uint16_t m_identifier;

 private:
  template <class U>
  static PoolHandle Builder();

  void retain();
  void release();

  /* Do not increase reference counters outside of the current checkpoint since
   * they won't be decreased if an exception is raised.
   *
   * WARNING: ref counters have a bugged behaviour in this case:
   *  Expression a = Cosine::Builder();
   *  Expression b;
   *  PoolCheckpoint() {
   *    ...
   *    b = a;
   *    ...
   *  }
   *
   * Here, the ref counter of the cos is not incremented while a and b point
   * towards it.
   * */
  bool canUpdateRefCounter() const {
    const PoolObject* object = this->object();
    return object && object->isAfterTopmostCheckpoint();
  }
};

}  // namespace Poincare
