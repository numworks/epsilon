#ifndef POINCARE_SQUARE_BRACKET_PAIR_LAYOUT_H
#define POINCARE_SQUARE_BRACKET_PAIR_LAYOUT_H

#include "bracket_pair_layout.h"
#include "layout_cursor.h"
#include "old_layout.h"

namespace Poincare {

class SquareBracketPairLayoutNode : public BracketPairLayoutNode {
 public:
  static void RenderWithChildSize(bool left, KDCoordinate childHeight,
                                  KDContext* ctx, KDPoint p,
                                  KDColor expressionColor,
                                  KDColor backgroundColor) {
    RenderWithParameters(left, childHeight, ctx, p, expressionColor,
                         backgroundColor, k_minVerticalMargin, k_bracketWidth,
                         k_renderTopBar, k_renderBottomBar, k_renderDoubleBar);
  }
  static KDSize SizeGivenChildSize(KDSize childSize) {
    return KDSize(2 * k_bracketWidth + childSize.width(),
                  Height(childSize.height(), k_minVerticalMargin));
  }
  static KDPoint ChildOffset(KDCoordinate childHeight) {
    return BracketPairLayoutNode::ChildOffset(k_minVerticalMargin,
                                              k_bracketWidth, childHeight);
  }

 protected:
  constexpr static KDCoordinate k_internalWidthMargin = 5;
  constexpr static KDCoordinate k_externalWidthMargin = 2;
  constexpr static KDCoordinate k_bracketWidth =
      k_internalWidthMargin + k_lineThickness + k_externalWidthMargin;
  constexpr static KDCoordinate k_minVerticalMargin = 1;
  constexpr static KDCoordinate k_doubleBarMargin = 2;
  constexpr static bool k_renderTopBar = true;
  constexpr static bool k_renderBottomBar = true;
  constexpr static bool k_renderDoubleBar = false;

  static void RenderWithParameters(bool left, KDCoordinate childHeight,
                                   KDContext* ctx, KDPoint p,
                                   KDColor expressionColor,
                                   KDColor backgroundColor,
                                   KDCoordinate minVerticalMargin,
                                   KDCoordinate bracketWidth, bool renderTopBar,
                                   bool renderBottomBar, bool renderDoubleBar);

  // BracketPairLayoutNode
  KDCoordinate bracketWidth() const override { return k_bracketWidth; }
  KDCoordinate minVerticalMargin() const override {
    return k_minVerticalMargin;
  }
  void renderOneBracket(bool left, KDContext* ctx, KDPoint p,
                        KDGlyph::Style style) override {
    RenderWithParameters(left, childLayout()->layoutSize(style.font).height(),
                         ctx, p, style.glyphColor, style.backgroundColor,
                         minVerticalMargin(), bracketWidth(), renderTopBar(),
                         renderBottomBar(), renderDoubleBar());
  }

  virtual bool renderTopBar() const { return k_renderTopBar; }
  virtual bool renderBottomBar() const { return k_renderBottomBar; }
  virtual bool renderDoubleBar() const { return k_renderDoubleBar; }
};

}  // namespace Poincare

#endif
