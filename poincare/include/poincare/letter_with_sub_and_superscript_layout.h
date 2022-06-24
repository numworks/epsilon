#ifndef LETTER_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H
#define LETTER_WITH_SUB_AND_SUPERSCRIPT_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class LetterWithSubAndSuperscriptLayoutNode : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;

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
  KDSize computeSize(const KDFont * font) override;
  KDCoordinate computeBaseline(const KDFont * font) override;
  KDPoint positionOfChild(LayoutNode * child, const KDFont * font) override;
  virtual void renderLetter(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) = 0;
private:
  void render(KDContext * ctx, KDPoint p, const KDFont * font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
  KDCoordinate aboveSymbol(const KDFont * font);
  KDCoordinate totalHeight(const KDFont * font);
  LayoutNode * nLayout() { return childAtIndex(0); }
  LayoutNode * kLayout() { return childAtIndex(1); }
};

}

#endif
