#ifndef POINCARE_TEST_BLOB_NODE_H
#define POINCARE_TEST_BLOB_NODE_H

#include <poincare/old/pool_handle.h>
#include <poincare/old/pool_object.h>

namespace Poincare {

class BlobNode : public PoolObject {
 public:
  BlobNode(int data) : m_data(data) {}
  size_t size() const override { return sizeof(BlobNode); }
  int data() { return m_data; }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream &stream) const override { stream << "Blob"; }
#endif
 private:
  int m_data;
};

class BlobByReference : public PoolHandle {
 public:
  static BlobByReference Builder(int data = 0) {
    void *bufferNode = Pool::sharedPool->alloc(sizeof(BlobNode));
    BlobNode *node = new (bufferNode) BlobNode(data);
    PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
    return static_cast<BlobByReference &>(h);
  }
  BlobByReference() = delete;
  int data() { return node()->data(); }

 private:
  BlobNode *node() const {
    return static_cast<BlobNode *>(PoolHandle::object());
  }
};

}  // namespace Poincare

#endif
