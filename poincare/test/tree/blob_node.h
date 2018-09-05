#ifndef POINCARE_TEST_BLOB_NODE_H
#define POINCARE_TEST_BLOB_NODE_H

#include <poincare/tree_node.h>
#include <poincare/tree_by_reference.h>

namespace Poincare {

class BlobNode : public TreeNode {
public:
  TreeNode * uninitializedStaticNode() const override;
  virtual size_t size() const override { return sizeof(BlobNode); }
  int data() { return m_data; }
  void setData(int data) { m_data = data; }
  virtual int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Blob";
  }
#endif
private:
  int m_data;
};

class UninitializedBlobNode : public BlobNode {
public:
  static UninitializedBlobNode * UninitializedBlobStaticNode();

  size_t size() const override { return sizeof(UninitializedBlobNode); }
  bool isUninitialized() const override { return true; }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UninitializedBlob";
  }
#endif
};

class BlobByReference : public TreeByReference {
public:
  BlobByReference(int data = 0) : TreeByReference(TreePool::sharedPool()->createTreeNode<BlobNode>()) {
    node()->setData(data);
  }
  int data() { return node()->data(); }
private:
  BlobNode * node() const { return static_cast<BlobNode *>(TreeByReference::node()); }
};

}

#endif
