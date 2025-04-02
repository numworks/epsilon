#ifndef POINCARE_TEST_BLOB_NODE_H
#define POINCARE_TEST_BLOB_NODE_H

#include <poincare/pool_handle.h>
#include <poincare/pool_object.h>

namespace Poincare {

class BlobObject : public PoolObject {
 public:
  BlobObject(int data) : m_data(data) {}
  size_t size() const override { return sizeof(BlobObject); }
  int data() { return m_data; }
#if POINCARE_TREE_LOG
  void logObjectName(std::ostream &stream) const override { stream << "Blob"; }
#endif
 private:
  int m_data;
};

class BlobByReference : public PoolHandle {
 public:
  static BlobByReference Builder(int data = 0) {
    void *bufferObject = Pool::sharedPool->alloc(sizeof(BlobObject));
    BlobObject *object = new (bufferObject) BlobObject(data);
    PoolHandle h = PoolHandle::Build(object);
    return static_cast<BlobByReference &>(h);
  }
  BlobByReference() = delete;
  int data() { return object()->data(); }

 private:
  BlobObject *object() const {
    return static_cast<BlobObject *>(PoolHandle::object());
  }
};

}  // namespace Poincare

#endif
