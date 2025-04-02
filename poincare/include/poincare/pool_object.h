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

#include "pool_checkpoint.h"

/* What's in a PoolObject, really?
 *  - a vtable pointer
 *  - an identifier
 *  - a reference counter
 */

/* CAUTION: To make operations faster, the pool needs all addresses and sizes of
 * PoolObjects to be a multiple of 4. */

namespace Poincare {

class PoolObject {
  friend class Pool;

 public:
  constexpr static uint16_t NoObjectIdentifier = -2;
  // Used for Integer
  constexpr static uint16_t OverflowIdentifier =
      PoolObject::NoObjectIdentifier + 1;

  // Constructor and destructor
  virtual ~PoolObject() {}
  typedef PoolObject* (*const Initializer)(void*);

  // Attributes
  virtual size_t size() const = 0;
  uint16_t identifier() const { return m_identifier; }
  int retainCount() const { return m_referenceCounter; }

  // Object operations
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
  void release();
  void rename(uint16_t identifier, bool unregisterPreviousIdentifier);

  // PoolCheckpoint
  bool isAfterTopmostCheckpoint() const {
    return this >= PoolCheckpoint::TopmostEndOfPool();
  }

#if POINCARE_TREE_LOG
  virtual void logObjectName(std::ostream& stream) const = 0;
  virtual void logAttributes(std::ostream& stream) const {}
  void log(std::ostream& stream, int indentation = 0, bool verbose = true);
  void log() {
    log(std::cout);
    std::cout << std::endl;
  }
#endif

  static bool IsValidIdentifier(uint16_t id) { return id < NoObjectIdentifier; }

 protected:
  PoolObject() : m_identifier(NoObjectIdentifier), m_referenceCounter(0) {}

 private:
  uint16_t m_identifier;
  int8_t m_referenceCounter;
};

template <typename T>
static PoolObject* Initializer(void* buffer) {
  return new (buffer) T;
}

}  // namespace Poincare

#endif
