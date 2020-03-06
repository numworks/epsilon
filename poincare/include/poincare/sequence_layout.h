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
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  LayoutNode * layoutToPointWhenInserting(Expression * correspondingExpression) override { return lowerBoundLayout(); }
  CodePoint XNTCodePoint(int childIndex = -1) const override;

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
  LayoutNode * argumentLayout() { return childAtIndex(k_argumentLayoutIndex); }
  LayoutNode * variableLayout() { return childAtIndex(k_variableLayoutIndex); }
  LayoutNode * lowerBoundLayout() { return childAtIndex(2); }
  LayoutNode * upperBoundLayout() { return childAtIndex(3); }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
private:
  static constexpr int k_argumentLayoutIndex = 0;
  static constexpr int k_variableLayoutIndex = 1;
  KDCoordinate completeLowerBoundX();
  KDCoordinate subscriptBaseline();
};

}

#endif
