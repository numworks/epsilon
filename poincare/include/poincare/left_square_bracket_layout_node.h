#ifndef POINCARE_LEFT_SQUARE_BRACKET_LAYOUT_NODE_H
#define POINCARE_LEFT_SQUARE_BRACKET_LAYOUT_NODE_H

#include <poincare/square_bracket_layout_node.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class LeftSquareBracketLayoutNode : public SquareBracketLayoutNode {
public:
  using SquareBracketLayoutNode::SquareBracketLayoutNode;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Char(buffer, bufferSize, '[');
  }
  bool isLeftBracket() const override { return true; }

  // TreeNode
  size_t size() const override { return sizeof(LeftSquareBracketLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "LeftSquareBracketLayout"; }
#endif
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
};

class LeftSquareBracketLayoutRef : public LayoutReference {
public:
  LeftSquareBracketLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<LeftSquareBracketLayoutNode>()) {}
};

}

#endif
