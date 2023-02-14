#ifndef LETTER_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H
#define LETTER_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class LetterWithSubAndSuperscriptLayoutNode : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  int indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex, bool * shouldRedrawLayout) override;
  int indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout) override;

  // TreeNode
  size_t size() const override { return sizeof(LetterWithSubAndSuperscriptLayoutNode); }
  int numberOfChildren() const override { return 2; }

protected:
  // LayoutNode
  constexpr static KDCoordinate k_margin = 3;
  constexpr static KDCoordinate k_symbolHeight = 16;
  constexpr static KDCoordinate k_symbolBaseline = 11;
  constexpr static KDCoordinate k_symbolWidth = 12;
  constexpr static KDCoordinate k_symbolWidthWithMargins = k_symbolWidth + 2 * k_margin;
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) override;
  virtual void renderLetter(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) = 0;
private:
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override;
  KDCoordinate aboveSymbol(KDFont::Size font);
  KDCoordinate totalHeight(KDFont::Size font);

  constexpr static int k_nLayoutIndex = 0;
  constexpr static int k_kLayoutIndex = 1;
  LayoutNode * nLayout() { return childAtIndex(k_nLayoutIndex); }
  LayoutNode * kLayout() { return childAtIndex(k_kLayoutIndex); }
};

}

#endif
