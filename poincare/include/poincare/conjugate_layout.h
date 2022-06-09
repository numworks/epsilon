#ifndef POINCARE_CONJUGATE_LAYOUT_NODE_H
#define POINCARE_CONJUGATE_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class ConjugateLayoutNode : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::ConjugateLayout; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool shouldCollapseSiblingsOnRight() const override { return true; }

  // TreeNode
  size_t size() const override { return sizeof(ConjugateLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ConjugateLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
  constexpr static KDCoordinate k_overlineWidth = 1;
  constexpr static KDCoordinate k_overlineVerticalMargin = 1;
  LayoutNode * childLayout() { return childAtIndex(0); }

private:
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
};

class ConjugateLayout final : public LayoutOneChild<ConjugateLayout, ConjugateLayoutNode> {
public:
  ConjugateLayout() = delete;
};

}

#endif
