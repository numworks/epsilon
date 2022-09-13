#ifndef POINCARE_SQUARE_BRACKET_PAIR_LAYOUT_H
#define POINCARE_SQUARE_BRACKET_PAIR_LAYOUT_H

#include <poincare/bracket_pair_layout.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class SquareBracketPairLayoutNode : public BracketPairLayoutNode {
public:
  static void RenderWithChildSize(bool left, KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
    RenderWithParameters(left, childHeight, ctx, p, expressionColor, backgroundColor, k_verticalMargin, k_bracketWidth, k_renderTopBar, k_renderBottomBar, k_renderDoubleBar);
  }
  static KDSize SizeGivenChildSize(KDSize childSize) { return KDSize(2 * k_bracketWidth + childSize.width(), HeightGivenChildHeight(childSize.height(), k_verticalMargin)); }
  static KDPoint ChildOffset() { return BracketPairLayoutNode::ChildOffset(k_verticalMargin, k_bracketWidth); }

  // LayoutNode
  bool shouldCollapseSiblingsOnRight() const override{ return true; }
  void didCollapseSiblings(LayoutCursor * cursor) override;

protected:
  constexpr static KDCoordinate k_internalWidthMargin = 5;
  constexpr static KDCoordinate k_externalWidthMargin = 2;
  constexpr static KDCoordinate k_bracketWidth = k_internalWidthMargin + k_lineThickness + k_externalWidthMargin;
  constexpr static KDCoordinate k_verticalMargin = 1;
  constexpr static KDCoordinate k_doubleBarMargin = 2;
  constexpr static bool k_renderTopBar = true;
  constexpr static bool k_renderBottomBar = true;
  constexpr static bool k_renderDoubleBar = false;

  static void RenderWithParameters(bool left, KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, KDCoordinate verticalMargin, KDCoordinate bracketWidth, bool renderTopBar, bool renderBottomBar, bool renderDoubleBar);

  // BracketPairLayoutNode
  KDCoordinate bracketWidth() const override { return k_bracketWidth; }
  KDCoordinate verticalMargin() const override { return k_verticalMargin; }
  void renderOneBracket(bool left, KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) override {
    RenderWithParameters(left, childLayout()->layoutSize(font).height(), ctx, p, expressionColor, backgroundColor, verticalMargin(), bracketWidth(), renderTopBar(), renderBottomBar(), renderDoubleBar());
  }

  virtual bool renderTopBar() const { return k_renderTopBar; }
  virtual bool renderBottomBar() const { return k_renderBottomBar; }
  virtual bool renderDoubleBar() const { return k_renderDoubleBar; }
};

}

#endif
