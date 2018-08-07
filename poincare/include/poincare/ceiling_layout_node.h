#ifndef POINCARE_CEILING_LAYOUT_NODE_H
#define POINCARE_CEILING_LAYOUT_NODE_H

#include <poincare/bracket_pair_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class CeilingLayoutNode : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixSerializableRefTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "ceil");
  }
  // TreeNode
  size_t size() const override { return sizeof(CeilingLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "CeilingLayout"; }
#endif

protected:
  bool renderBottomBar() const override { return false; }
};

class CeilingLayoutRef : public LayoutReference {
public:
  CeilingLayoutRef(TreeNode * n) : LayoutReference(n) {}
  CeilingLayoutRef(LayoutRef l) : CeilingLayoutRef() {
    replaceChildAtIndexInPlace(0, l);
  }
private:
  CeilingLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<CeilingLayoutNode>(), true) {}
};

}

#endif
