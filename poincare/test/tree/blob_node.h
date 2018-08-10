#ifndef POINCARE_TEST_BLOB_NODE_H
#define POINCARE_TEST_BLOB_NODE_H

#include <poincare/tree_node.h>
#include <poincare/allocation_failure_node.h>
#include <poincare/tree_by_reference.h>
#include <poincare/tree_by_value.h>

namespace Poincare {


class BlobNode : public TreeNode {
public:
  static BlobNode * FailedAllocationStaticNode() {
    static AllocationFailureNode<BlobNode> failureNode;
    TreePool::sharedPool()->registerStaticNodeIfRequired(&failureNode);
    return &failureNode;
  }
  virtual BlobNode * failedAllocationStaticNode() override {
    return BlobNode::FailedAllocationStaticNode();
  }
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


class BlobByReference : public TreeByReference {
public:
  BlobByReference(int data = 0) : TreeByReference(TreePool::sharedPool()->createTreeNode<BlobNode>()) {
    node()->setData(data);
  }
  int data() { return node()->data(); }
private:
  BlobNode * node() const override { return static_cast<BlobNode *>(TreeByReference::node()); }
};


class BlobByValue : public TreeByValue {
public:
  BlobByValue(int data = 0) : TreeByValue(TreePool::sharedPool()->createTreeNode<BlobNode>()) {
    node()->setData(data);
  }
  int data() { return node()->data(); }
private:
  BlobNode * node() const override { return static_cast<BlobNode *>(TreeByValue::node()); }
};


}

#endif
