#ifndef POINCARE_RIGHT_SQUARE_BRACKET_LAYOUT_NODE_H
#define POINCARE_RIGHT_SQUARE_BRACKET_LAYOUT_NODE_H

#include <poincare/square_bracket_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class RightSquareBracketLayoutNode : public SquareBracketLayoutNode {
public:
  using SquareBracketLayoutNode::SquareBracketLayoutNode;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, ']');
  }
  bool isRightBracket() const override { return true; }

  // TreeNode
  size_t size() const override { return sizeof(RightSquareBracketLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "RightSquareBracketLayout"; }
#endif

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class RightSquareBracketLayoutRef : public LayoutReference {
public:
  RightSquareBracketLayoutRef(TreeNode * n) : LayoutReference(n) {}

  RightSquareBracketLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<RightSquareBracketLayoutNode>();
    m_identifier = node->identifier();
  }


};

}

#endif
