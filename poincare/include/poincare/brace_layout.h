#ifndef POINCARE_BRACE_LAYOUT_NODE_H
#define POINCARE_BRACE_LAYOUT_NODE_H

#include <poincare/bracket_layout.h>

namespace Poincare {

/* This class is used to implement the collapsing pattern shared between curly
 * braces and parentheses. */

class BraceLayoutNode : public BracketLayoutNode {
public:
  using BracketLayoutNode::BracketLayoutNode;
  // TreeNode
  size_t size() const override { return sizeof(BraceLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "BraceLayout";
  }
#endif
  bool isCollapsable(int * numberOfOpenBraces, bool goingLeft) const override;

protected:
  virtual Type leftLayoutType() const = 0;
  virtual Type rightLayoutType() const = 0;
};

}

#endif
