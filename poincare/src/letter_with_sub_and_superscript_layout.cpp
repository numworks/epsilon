#include <poincare/layout_helper.h>
#include <poincare/letter_with_sub_and_superscript_layout.h>

#include <algorithm>

namespace Poincare {

int LetterWithSubAndSuperscriptLayoutNode::indexAfterHorizontalCursorMove(
    OMG::HorizontalDirection direction, int currentIndex,
    bool* shouldRedrawLayout) {
  switch (currentIndex) {
    case k_outsideIndex:
      return direction.isRight() ? k_nLayoutIndex : k_kLayoutIndex;
    case k_nLayoutIndex:
      return direction.isRight() ? k_kLayoutIndex : k_outsideIndex;
    default:
      assert(currentIndex == k_kLayoutIndex);
      return direction.isRight() ? k_outsideIndex : k_nLayoutIndex;
  }
}

int LetterWithSubAndSuperscriptLayoutNode::indexAfterVerticalCursorMove(
    OMG::VerticalDirection direction, int currentIndex,
    PositionInLayout positionAtCurrentIndex, bool* shouldRedrawLayout) {
  if (direction.isUp() &&
      (currentIndex == k_kLayoutIndex ||
       (currentIndex == k_outsideIndex &&
        positionAtCurrentIndex == PositionInLayout::Left))) {
    return k_nLayoutIndex;
  }

  if (direction.isDown() &&
      (currentIndex == k_nLayoutIndex ||
       (currentIndex == k_outsideIndex &&
        positionAtCurrentIndex == PositionInLayout::Right))) {
    return k_kLayoutIndex;
  }
  return k_cantMoveIndex;
}

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::aboveSymbol(
    KDFont::Size font) {
  return std::max<KDCoordinate>(nLayout()->baseline(font),
                                kLayout()->baseline(font) - k_symbolHeight);
}

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::totalHeight(
    KDFont::Size font) {
  KDCoordinate underSymbol = std::max<KDCoordinate>(
      kLayout()->layoutSize(font).height() - kLayout()->baseline(font),
      nLayout()->layoutSize(font).height() - nLayout()->baseline(font) -
          k_symbolHeight);
  return aboveSymbol(font) + k_symbolHeight + underSymbol;
}

KDSize LetterWithSubAndSuperscriptLayoutNode::computeSize(KDFont::Size font) {
  KDCoordinate width = nLayout()->layoutSize(font).width() +
                       k_symbolWidthWithMargins +
                       kLayout()->layoutSize(font).width();
  return KDSize(width, totalHeight(font));
}

KDCoordinate LetterWithSubAndSuperscriptLayoutNode::computeBaseline(
    KDFont::Size font) {
  return std::max(0, aboveSymbol(font) + k_symbolBaseline);
}

KDPoint LetterWithSubAndSuperscriptLayoutNode::positionOfChild(
    LayoutNode* child, KDFont::Size font) {
  if (child == nLayout()) {
    return KDPoint(0, aboveSymbol(font) - nLayout()->baseline(font));
  }
  assert(child == kLayout());
  return KDPoint(
      nLayout()->layoutSize(font).width() + k_symbolWidthWithMargins,
      aboveSymbol(font) + k_symbolHeight - kLayout()->baseline(font));
}

void LetterWithSubAndSuperscriptLayoutNode::render(KDContext* ctx, KDPoint p,
                                                   KDFont::Size font,
                                                   KDColor expressionColor,
                                                   KDColor backgroundColor) {
  KDCoordinate combinationSymbolX = nLayout()->layoutSize(font).width();
  KDCoordinate combinationSymbolY = aboveSymbol(font);
  KDPoint base =
      p.translatedBy(KDPoint(combinationSymbolX, combinationSymbolY));

  // Margin around the letter is left to the letter renderer
  renderLetter(ctx, base, expressionColor, backgroundColor);
}

}  // namespace Poincare
