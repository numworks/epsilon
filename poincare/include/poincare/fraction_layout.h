#ifndef POINCARE_FRACTION_LAYOUT_NODE_H
#define POINCARE_FRACTION_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class FractionLayoutNode /*final*/ : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  // Layout
  Type type() const override { return Type::FractionLayout; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  bool shouldCollapseSiblingsOnLeft() const override { return true; }
  bool shouldCollapseSiblingsOnRight() const override { return true; }
  int leftCollapsingAbsorbingChildIndex() const override { return 0; }
  int rightCollapsingAbsorbingChildIndex() const override { return 1; }
  void didCollapseSiblings(LayoutCursor * cursor) override;
  LayoutNode * layoutToPointWhenInserting(Expression * correspondingExpression) override;
  bool canBeOmittedMultiplicationRightFactor() const override { return false; }
  /* WARNING: We need to override this function, else 1/2 3/4 would be
   * serialized as 1/2**3/4, as the two Fraction layouts think their sibling is
   * an omitted multiplication layout factor. We have the same problem with
   *  2^3 1/2 being serialized as 2^3**1/2, so must override the Right version
   * and not canBeOmittedMultiplicationLeftFactor. */

  // TreeNode
  size_t size() const override { return sizeof(FractionLayoutNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "FractionLayout";
  }
#endif

protected:
  // SerializationInterface
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override { return true; }
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  constexpr static KDCoordinate k_fractionLineMargin = 2;
  constexpr static KDCoordinate k_fractionLineHeight = 1;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  LayoutNode * numeratorLayout() { return childAtIndex(0); }
  LayoutNode * denominatorLayout() { return childAtIndex(1); }
};

class FractionLayout final : public Layout {
public:
  static FractionLayout Builder(Layout child0, Layout child1) { return TreeHandle::FixedArityBuilder<FractionLayout, FractionLayoutNode>({child0, child1}); }
  FractionLayout() = delete;
};

}

#endif
