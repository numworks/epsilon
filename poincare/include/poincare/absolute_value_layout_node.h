#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H

#include <poincare/allocation_failure_layout_node.h>
#include <poincare/bracket_pair_layout_node.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class AbsoluteValueLayoutNode : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "abs");
  }

  // TreeNode
  static AbsoluteValueLayoutNode * FailedAllocationStaticNode();
  AbsoluteValueLayoutNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
  size_t size() const override { return sizeof(AbsoluteValueLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const {
    stream << "AbsoluteValueLayout";
  }
#endif

private:
  KDCoordinate widthMargin() const override { return 2; }
  virtual KDCoordinate verticalExternMargin() const override { return 1; }
  bool renderTopBar() const override { return false; }
  bool renderBottomBar() const override { return false; }
};

class AbsoluteValueLayoutRef : public LayoutReference {
public:
  AbsoluteValueLayoutRef(LayoutRef l) :
    LayoutReference(TreePool::sharedPool()->createTreeNode<AbsoluteValueLayoutNode>())
  {
    replaceChildAtIndexInPlace(0, l);
  }
};

}

#endif
