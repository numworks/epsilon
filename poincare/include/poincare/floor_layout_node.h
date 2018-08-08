#ifndef POINCARE_FLOOR_LAYOUT_NODE_H
#define POINCARE_FLOOR_LAYOUT_NODE_H

#include <poincare/bracket_pair_layout_node.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class FloorLayoutNode : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "floor");
  }

  // TreeNode
  size_t size() const override { return sizeof(FloorLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "FloorLayout"; }
#endif

protected:
  bool renderTopBar() const override { return false; }
};

class FloorLayoutRef : public LayoutReference {
public:
  FloorLayoutRef(LayoutRef l) : LayoutReference(TreePool::sharedPool()->createTreeNode<FloorLayoutNode>()) {
    replaceChildAtIndexInPlace(0, l);
  }
};

}

#endif
