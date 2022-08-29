#ifndef POINCARE_SEQUENCE_LAYOUT_NODE_H
#define POINCARE_SEQUENCE_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class SequenceLayoutNode : public LayoutNode {
public:
  constexpr static KDCoordinate k_symbolHeight = 29;
  constexpr static KDCoordinate k_symbolWidth = 22;

  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  LayoutNode * layoutToPointWhenInserting(Expression * correspondingExpression, bool * forceCursorLeftOfText = nullptr) override { return lowerBoundLayout(); }
  Layout XNTLayout(int childIndex = -1) const override;

  // TreeNode
  int numberOfChildren() const override { return 4; }

protected:
  constexpr static KDCoordinate k_boundHeightMargin = 2;
  constexpr static KDCoordinate k_argumentWidthMargin = 2;
  constexpr static const char * k_equal = "=";

  KDSize lowerBoundSizeWithVariableEquals(KDFont::Size font);

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override;

  int writeDerivedClassInBuffer(const char * operatorName, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
  LayoutNode * argumentLayout() { return childAtIndex(k_argumentLayoutIndex); }
  LayoutNode * variableLayout() { return childAtIndex(k_variableLayoutIndex); }
  LayoutNode * lowerBoundLayout() { return childAtIndex(2); }
  LayoutNode * upperBoundLayout() { return childAtIndex(3); }
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
private:
  constexpr static int k_argumentLayoutIndex = 0;
  constexpr static int k_variableLayoutIndex = 1;
  KDCoordinate completeLowerBoundX(KDFont::Size font);
  KDCoordinate subscriptBaseline(KDFont::Size font);
};

}

#endif
