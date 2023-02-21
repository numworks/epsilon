#ifndef POINCARE_SEQUENCE_LAYOUT_NODE_H
#define POINCARE_SEQUENCE_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class SequenceLayoutNode : public LayoutNode {
 public:
  constexpr static KDCoordinate k_symbolHeightLargeFont = 29;
  constexpr static KDCoordinate k_symbolWidthLargeFont = 22;
  constexpr static KDCoordinate k_symbolHeightSmallFont = 21;
  constexpr static KDCoordinate k_symbolWidthSmallFont = 16;

  constexpr static KDCoordinate SymbolHeight(KDFont::Size font) {
    return font == KDFont::Size::Large ? k_symbolHeightLargeFont
                                       : k_symbolHeightSmallFont;
  }
  constexpr static KDCoordinate SymbolWidth(KDFont::Size font) {
    return font == KDFont::Size::Large ? k_symbolWidthLargeFont
                                       : k_symbolWidthSmallFont;
  }

  using LayoutNode::LayoutNode;

  // LayoutNode
  int indexAfterHorizontalCursorMove(OMG::HorizontalDirection direction,
                                     int currentIndex,
                                     bool* shouldRedrawLayout) override;
  int indexAfterVerticalCursorMove(OMG::VerticalDirection direction,
                                   int currentIndex,
                                   PositionInLayout positionAtCurrentIndex,
                                   bool* shouldRedrawLayout) override;
  DeletionMethod deletionMethodForCursorLeftOfChild(
      int childIndex) const override;
  int indexOfChildToPointToWhenInserting() override {
    return k_lowerBoundLayoutIndex;
  }
  Layout XNTLayout(int childIndex = -1) const override;

  // TreeNode
  int numberOfChildren() const override { return 4; }

 protected:
  constexpr static KDCoordinate k_upperBoundVerticalMarginLargeFont = 2;
  constexpr static KDCoordinate k_lowerBoundVerticalMarginLargeFont = 2;
  // There is already a natural margin under the upperbound expression
  constexpr static KDCoordinate k_upperBoundVerticalMarginSmallFont = 0;
  constexpr static KDCoordinate k_lowerBoundVerticalMarginSmallFont = 1;
  constexpr static KDCoordinate UpperBoundVerticalMargin(KDFont::Size font) {
    return font == KDFont::Size::Large ? k_upperBoundVerticalMarginLargeFont
                                       : k_upperBoundVerticalMarginSmallFont;
  }
  constexpr static KDCoordinate LowerBoundVerticalMargin(KDFont::Size font) {
    return font == KDFont::Size::Large ? k_lowerBoundVerticalMarginLargeFont
                                       : k_lowerBoundVerticalMarginSmallFont;
  }

  constexpr static KDCoordinate k_argumentHorizontalMarginLargeFont = 2;
  constexpr static KDCoordinate k_argumentHorizontalMarginSmallFont = 0;
  constexpr static KDCoordinate ArgumentHorizontalMargin(KDFont::Size font) {
    return font == KDFont::Size::Large ? k_argumentHorizontalMarginLargeFont
                                       : k_argumentHorizontalMarginSmallFont;
  }
  constexpr static const char* k_equal = "=";

  KDSize lowerBoundSizeWithVariableEquals(KDFont::Size font);

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override;

  int writeDerivedClassInBuffer(const char* operatorName, char* buffer,
                                int bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const;
  LayoutNode* argumentLayout() { return childAtIndex(k_argumentLayoutIndex); }
  LayoutNode* variableLayout() { return childAtIndex(k_variableLayoutIndex); }
  LayoutNode* lowerBoundLayout() {
    return childAtIndex(k_lowerBoundLayoutIndex);
  }
  LayoutNode* upperBoundLayout() {
    return childAtIndex(k_upperBoundLayoutIndex);
  }
  void render(KDContext* ctx, KDPoint p, KDFont::Size font,
              KDColor expressionColor, KDColor backgroundColor) override;

 private:
  constexpr static int k_argumentLayoutIndex = 0;
  constexpr static int k_variableLayoutIndex = 1;
  constexpr static int k_lowerBoundLayoutIndex = 2;
  constexpr static int k_upperBoundLayoutIndex = 3;
  KDCoordinate completeLowerBoundX(KDFont::Size font);
  KDCoordinate subscriptBaseline(KDFont::Size font);
};

}  // namespace Poincare

#endif
