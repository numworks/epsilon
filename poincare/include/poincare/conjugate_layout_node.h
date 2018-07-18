#ifndef POINCARE_CONJUGATE_LAYOUT_NODE_H
#define POINCARE_CONJUGATE_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class ConjugateLayoutNode : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  bool shouldCollapseSiblingsOnRight() const override{ return true; }

  // TreeNode
  size_t size() const override { return sizeof(ConjugateLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if TREE_LOG
  const char * description() const override {
    return "ConjugateLayout";
  }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  constexpr static KDCoordinate k_overlineWidth = 1;
  constexpr static KDCoordinate k_overlineVerticalMargin = 1;
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  LayoutNode * childLayout() { return childAtIndex(0); }
};

class ConjugateLayoutRef : public LayoutReference<ConjugateLayoutNode> {
public:
  ConjugateLayoutRef(LayoutRef l) :
    LayoutReference<ConjugateLayoutNode>()
  {
    addChildTreeAtIndex(l, 0);
  }
  ConjugateLayoutRef(TreeNode * n) : LayoutReference<ConjugateLayoutNode>(n) {}
};

}

#endif
