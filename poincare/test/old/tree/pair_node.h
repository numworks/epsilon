#ifndef POINCARE_TEST_PAIR_NODE_H
#define POINCARE_TEST_PAIR_NODE_H

#include <poincare/old/pool_handle.h>
#include <poincare/old/pool_object.h>

namespace Poincare {

class PairNode : public PoolObject {
 public:
  size_t size() const override { return sizeof(PairNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream &stream) const override { stream << "Pair"; }
#endif
};

class PairByReference : public PoolHandle {
 public:
  static PairByReference Builder(PoolHandle t1, PoolHandle t2) {
    void *bufferNode = Pool::sharedPool->alloc(sizeof(PairNode));
    PairNode *node = new (bufferNode) PairNode();
    PoolHandle children[2] = {t1, t2};
    PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
    h.replaceChildAtIndexInPlace(0, t1);
    h.replaceChildAtIndexInPlace(1, t2);
    return static_cast<PairByReference &>(h);
  }
  PairByReference() = delete;
};

}  // namespace Poincare

#endif
