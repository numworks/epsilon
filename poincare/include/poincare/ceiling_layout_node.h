#ifndef POINCARE_CEILING_LAYOUT_NODE_H
#define POINCARE_CEILING_LAYOUT_NODE_H

#include <poincare/bracket_pair_layout_node.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class CeilingLayoutNode : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "ceil");
  }

  // TreeNode
  size_t size() const override { return sizeof(CeilingLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "CeilingLayout";
  }
#endif

protected:
  bool renderBottomBar() const override { return false; }
};

class CeilingLayoutRef : public LayoutReference {
public:
  CeilingLayoutRef(LayoutRef l) : LayoutReference(TreePool::sharedPool()->createTreeNode<CeilingLayoutNode>()) {
    replaceChildAtIndexInPlace(0, l);
  }
};

}

#endif
