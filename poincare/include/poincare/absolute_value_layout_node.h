#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H

#include <poincare/bracket_pair_layout_node.h>

namespace Poincare {

class AbsoluteValueLayoutNode : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixSerializableRefTextInBuffer(SerializableRef(const_cast<AbsoluteValueLayoutNode *>(this)), buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "abs");
  }
  // TreeNode
  size_t size() const override { return sizeof(AbsoluteValueLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "AbsoluteValueLayout"; }
#endif

private:
  KDCoordinate widthMargin() const override { return 2; }
  virtual KDCoordinate verticalExternMargin() const override { return 1; }
  bool renderTopBar() const override { return false; }
  bool renderBottomBar() const override { return false; }
};

class AbsoluteValueLayoutRef : public LayoutReference {
public:
  AbsoluteValueLayoutRef(TreeNode * n) : LayoutReference(n) {}
  AbsoluteValueLayoutRef(LayoutRef l) : AbsoluteValueLayoutRef() {
    addChildTreeAtIndex(l, 0, 0);
  }
private:
  AbsoluteValueLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<AbsoluteValueLayoutNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
