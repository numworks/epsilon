#ifndef POINCARE_TEST_PAIR_NODE_H
#define POINCARE_TEST_PAIR_NODE_H

#include <poincare/pool_handle.h>
#include <poincare/pool_object.h>

namespace Poincare {

class PairObject : public PoolObject {
 public:
  PairObject(PoolHandle t1, PoolHandle t2) : m_t1(t1), m_t2(t2) {}
  PoolHandle t1() { return m_t1; }
  PoolHandle t2() { return m_t2; }
  size_t size() const override { return sizeof(PairObject); }
#if POINCARE_TREE_LOG
  void logObjectName(std::ostream &stream) const override { stream << "Pair"; }
#endif
 private:
  PoolHandle m_t1;
  PoolHandle m_t2;
};

class PairByReference : public PoolHandle {
 public:
  static PairByReference Builder(PoolHandle t1, PoolHandle t2) {
    void *bufferObject = Pool::sharedPool->alloc(sizeof(PairObject));
    PairObject *object = new (bufferObject) PairObject(t1, t2);
    PoolHandle h = PoolHandle::Build(object);
    return static_cast<PairByReference &>(h);
  }
  PairByReference() = delete;
};

}  // namespace Poincare

#endif
