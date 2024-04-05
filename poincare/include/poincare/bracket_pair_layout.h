#ifndef POINCARE_BRACKET_PAIR_LAYOUT_H
#define POINCARE_BRACKET_PAIR_LAYOUT_H

#include <escher/metric.h>
#include <poincare/layout_node.h>

namespace Poincare {

class BracketPairLayoutNode : public LayoutNode {
 public:
  constexpr static KDCoordinate k_lineThickness = 1;
  // PoolObject
  size_t size() const override { return sizeof(BracketPairLayoutNode); }
  int numberOfChildren() const override { return 1; }

  // LayoutNode
  DeletionMethod deletionMethodForCursorLeftOfChild(
      int childIndex) const override;

 protected:
  // Minimal height at which the children dictates bracket height
  static constexpr KDCoordinate k_minimalChildHeight =
      Escher::Metric::MinimalBracketAndParenthesisChildHeight;

  static KDCoordinate VerticalMargin(KDCoordinate childHeight,
                                     KDCoordinate minVerticalMargin) {
    /* If the child height is below the threshold, make it bigger so that
     * The bracket pair maintains the right height */
    KDCoordinate verticalMargin = minVerticalMargin;

    if (childHeight < k_minimalChildHeight) {
      verticalMargin += (k_minimalChildHeight - childHeight) / 2;
    }
    return verticalMargin;
  }
  static KDCoordinate Height(KDCoordinate childHeight,
                             KDCoordinate minVerticalMargin) {
    return childHeight + 2 * VerticalMargin(childHeight, minVerticalMargin);
  }

  static KDCoordinate Baseline(KDCoordinate childHeight,
                               KDCoordinate childBaseline,
                               KDCoordinate minVerticalMargin) {
    return childBaseline + VerticalMargin(childHeight, minVerticalMargin);
  }

  static KDPoint ChildOffset(KDCoordinate minVerticalMargin,
                             KDCoordinate bracketWidth,
                             KDCoordinate childHeight) {
    return KDPoint(bracketWidth,
                   VerticalMargin(childHeight, minVerticalMargin));
  }

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override;
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) override;

  LayoutNode* childLayout() const { return childAtIndex(0); }
  size_t serializeWithSymbol(char symbolOpen, char symbolClose, char* buffer,
                             size_t bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const;
  virtual KDCoordinate bracketWidth() const = 0;
  virtual KDCoordinate minVerticalMargin() const = 0;
  virtual void renderOneBracket(bool left, KDContext* ctx, KDPoint p,
                                KDGlyph::Style style) = 0;
};

}  // namespace Poincare

#endif
