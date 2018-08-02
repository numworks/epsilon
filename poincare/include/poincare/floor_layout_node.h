#ifndef POINCARE_FLOOR_LAYOUT_NODE_H
#define POINCARE_FLOOR_LAYOUT_NODE_H

#include <poincare/bracket_pair_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class FloorLayoutNode : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixSerializableRefTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "floor");
  }

  // TreeNode
  // size() does not need to be overrided
#if TREE_LOG
  const char * description() const override { return "FloorLayout"; }
#endif

protected:
  bool renderTopBar() const override { return false; }
};

class FloorLayoutRef : public LayoutReference {
public:
  FloorLayoutRef(TreeNode * n) : LayoutReference(n) {}
  FloorLayoutRef(LayoutRef l) : FloorLayoutRef() {
    addChildTreeAtIndex(l, 0, 0);
  }
private:
  FloorLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<FloorLayoutNode>();
    m_identifier = node->identifier();
  }

};

}

#endif
