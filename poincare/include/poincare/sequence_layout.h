#ifndef POINCARE_SEQUENCE_LAYOUT_NODE_H
#define POINCARE_SEQUENCE_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class SequenceLayoutNode : public LayoutNode {
public:
  constexpr static KDCoordinate k_symbolHeight = 15;
  constexpr static KDCoordinate k_symbolWidth = 9;

  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  LayoutNode * layoutToPointWhenInserting() override { return lowerBoundLayout(); }
  CodePoint XNTCodePoint() const override { return 'n'; }

  // TreeNode
  int numberOfChildren() const override { return 4; }

protected:
  constexpr static KDCoordinate k_boundHeightMargin = 2;
  constexpr static KDCoordinate k_argumentWidthMargin = 2;
  constexpr static const KDFont * k_font = KDFont::LargeFont;
  constexpr static const char * k_equal = "=";

  KDSize lowerBoundSizeWithVariableEquals();

  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;

  int writeDerivedClassInBuffer(const char * operatorName, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
  LayoutNode * argumentLayout() { return childAtIndex(0); }
  LayoutNode * variableLayout() { return childAtIndex(1); }
  LayoutNode * lowerBoundLayout() { return childAtIndex(2); }
  LayoutNode * upperBoundLayout() { return childAtIndex(3); }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
private:
  KDCoordinate completeLowerBoundX();
  KDCoordinate subscriptBaseline();
};

}

#endif
