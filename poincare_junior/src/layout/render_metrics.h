#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H

#include <escher/metric.h>
#include <kandinsky/coordinate.h>

#include "assert.h"
#include "indices.h"
#include "render.h"

namespace PoincareJ {

namespace Fraction {
constexpr static KDCoordinate k_lineMargin = 2;
constexpr static KDCoordinate k_lineHeight = 1;
constexpr static KDCoordinate k_horizontalOverflow =
    Escher::Metric::FractionAndConjugateHorizontalOverflow;
constexpr static KDCoordinate k_horizontalMargin =
    Escher::Metric::FractionAndConjugateHorizontalMargin;
}  // namespace Fraction

namespace CodePoint {
constexpr static KDCoordinate k_middleDotWidth = 5;
}

namespace VerticalOffset {
constexpr static KDCoordinate k_indiceHeight = 10;
}

namespace Pair {
constexpr static KDCoordinate k_lineThickness = 1;

constexpr static KDCoordinate k_minimalChildHeight =
    Escher::Metric::MinimalBracketAndParenthesisChildHeight;

constexpr static uint8_t k_temporaryBlendAlpha = 0x60;

static bool ChildHeightDictatesHeight(KDCoordinate childHeight) {
  return childHeight >= k_minimalChildHeight;
}
static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight,
                                           KDCoordinate verticalMargin) {
  return (ChildHeightDictatesHeight(childHeight) ? childHeight
                                                 : k_minimalChildHeight) +
         verticalMargin * 2;
}
static KDCoordinate BaselineGivenChildHeightAndBaseline(
    KDCoordinate childHeight, KDCoordinate childBaseline,
    KDCoordinate verticalMargin) {
  return childBaseline + verticalMargin +
         (ChildHeightDictatesHeight(childHeight)
              ? 0
              : (k_minimalChildHeight - childHeight) / 2);
}
static KDPoint ChildOffset(KDCoordinate verticalMargin,
                           KDCoordinate bracketWidth) {
  return KDPoint(bracketWidth, verticalMargin);
}
static KDPoint PositionGivenChildHeightAndBaseline(
    bool left, KDCoordinate bracketWidth, KDSize childSize,
    KDCoordinate childBaseline, KDCoordinate verticalMargin) {
  return KDPoint(
      left ? -bracketWidth : childSize.width(),
      ChildHeightDictatesHeight(childSize.height())
          ? -verticalMargin
          : childBaseline -
                HeightGivenChildHeight(childSize.height(), verticalMargin) / 2);
}
static KDCoordinate OptimalChildHeightGivenLayoutHeight(
    KDCoordinate layoutHeight, KDCoordinate verticalMargin) {
  return layoutHeight - verticalMargin * 2;
}

}  // namespace Pair

namespace SquareBracketPair {
using Pair::k_lineThickness;
constexpr static KDCoordinate k_internalWidthMargin = 5;
constexpr static KDCoordinate k_externalWidthMargin = 2;
constexpr static KDCoordinate k_bracketWidth =
    k_internalWidthMargin + k_lineThickness + k_externalWidthMargin;
constexpr static KDCoordinate k_verticalMargin = 1;
constexpr static KDCoordinate k_doubleBarMargin = 2;

static KDSize SizeGivenChildSize(KDSize childSize) {
  return KDSize(
      2 * k_bracketWidth + childSize.width(),
      Pair::HeightGivenChildHeight(childSize.height(), k_verticalMargin));
}
static KDPoint ChildOffset() {
  return Pair::ChildOffset(k_verticalMargin, k_bracketWidth);
}

}  // namespace SquareBracketPair

namespace AbsoluteValue {
constexpr static KDCoordinate k_k_innerWidthMargin = 2;
constexpr static KDCoordinate k_bracketWidth =
    Pair::k_lineThickness + k_k_innerWidthMargin +
    SquareBracketPair::k_externalWidthMargin;
constexpr static KDCoordinate k_verticalMargin = 0;
}  // namespace AbsoluteValue

namespace VectorNorm {
constexpr static KDCoordinate k_innerWidthMargin = 2;
constexpr static KDCoordinate k_bracketWidth =
    2 * Pair::k_lineThickness + SquareBracketPair::k_doubleBarMargin +
    k_innerWidthMargin + SquareBracketPair::k_externalWidthMargin;
constexpr static KDCoordinate k_verticalMargin = 0;
}  // namespace VectorNorm

namespace CurlyBrace {
using Pair::k_lineThickness;
constexpr static KDCoordinate k_curveHeight = 6;
constexpr static KDCoordinate k_curveWidth = 5;
constexpr static KDCoordinate k_centerHeight = 3;
constexpr static KDCoordinate k_centerWidth = 3;
constexpr static KDCoordinate k_widthMargin = 1;
constexpr static KDCoordinate k_verticalMargin = 1;
constexpr static KDCoordinate k_curlyBraceWidth =
    2 * k_widthMargin + k_centerWidth + k_curveWidth - k_lineThickness;

static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) {
  return Pair::HeightGivenChildHeight(childHeight, k_verticalMargin);
}
static KDCoordinate BaselineGivenChildHeightAndBaseline(
    KDCoordinate childHeight, KDCoordinate childBaseline) {
  return Pair::BaselineGivenChildHeightAndBaseline(childHeight, childBaseline,
                                                   k_verticalMargin);
}
static KDPoint PositionGivenChildHeightAndBaseline(bool left, KDSize childSize,
                                                   KDCoordinate childBaseline) {
  return Pair::PositionGivenChildHeightAndBaseline(
      left, k_curlyBraceWidth, childSize, childBaseline, k_verticalMargin);
}
}  // namespace CurlyBrace

namespace Parenthesis {
constexpr static KDCoordinate k_widthMargin = 1;
constexpr static KDCoordinate k_curveWidth = 5;
constexpr static KDCoordinate k_curveHeight = 7;
constexpr static KDCoordinate k_verticalMargin = 2;
constexpr static KDCoordinate k_parenthesisWidth =
    2 * k_widthMargin + k_curveWidth;

static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) {
  return Pair::HeightGivenChildHeight(childHeight, k_verticalMargin);
}
static KDCoordinate BaselineGivenChildHeightAndBaseline(
    KDCoordinate childHeight, KDCoordinate childBaseline) {
  return Pair::BaselineGivenChildHeightAndBaseline(childHeight, childBaseline,
                                                   k_verticalMargin);
}
static KDPoint PositionGivenChildHeightAndBaseline(bool left, KDSize childSize,
                                                   KDCoordinate childBaseline) {
  return Pair::PositionGivenChildHeightAndBaseline(
      left, k_parenthesisWidth, childSize, childBaseline, k_verticalMargin);
}
}  // namespace Parenthesis

namespace Pair {
static KDCoordinate BracketWidth(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Ceiling:
    case LayoutType::Floor:
      return SquareBracketPair::k_bracketWidth;
    case LayoutType::AbsoluteValue:
      return AbsoluteValue::k_bracketWidth;
    case LayoutType::VectorNorm:
      return VectorNorm::k_bracketWidth;
    case LayoutType::CurlyBrace:
      return CurlyBrace::k_curlyBraceWidth;
    case LayoutType::Parenthesis:
      return Parenthesis::k_parenthesisWidth;
    default:
      assert(false);
  }
}

static KDCoordinate VerticalMargin(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Ceiling:
    case LayoutType::Floor:
      return SquareBracketPair::k_verticalMargin;
    case LayoutType::AbsoluteValue:
      return AbsoluteValue::k_verticalMargin;
    case LayoutType::VectorNorm:
      return VectorNorm::k_verticalMargin;
    case LayoutType::CurlyBrace:
      return CurlyBrace::k_verticalMargin;
    case LayoutType::Parenthesis:
      return Parenthesis::k_verticalMargin;
    default:
      assert(false);
  }
}

}  // namespace Pair

namespace Conjugate {
constexpr static KDCoordinate k_overlineWidth = 1;
constexpr static KDCoordinate k_overlineVerticalMargin = 1;
}  // namespace Conjugate

namespace NthRoot {
constexpr static KDCoordinate k_heightMargin = 2;
constexpr static KDCoordinate k_widthMargin = 2;
constexpr static KDCoordinate k_radixLineThickness = 1;

constexpr static KDCoordinate k_leftRadixHeight = 9;
constexpr static KDCoordinate k_leftRadixWidth = 5;

KDSize AdjustedIndexSize(const Tree* node, KDFont::Size font) {
  return node->isSquareRootLayout()
             ? KDSize(k_leftRadixWidth, 0)
             : KDSize(std::max(k_leftRadixWidth, Render::Width(node->child(1))),
                      Render::Height(node->child(1)));
}
}  // namespace NthRoot

namespace Parametric {
constexpr static KDCoordinate SymbolHeight(KDFont::Size font) {
  return font == KDFont::Size::Large ? 29 : 21;
}
constexpr static KDCoordinate SymbolWidth(KDFont::Size font) {
  return font == KDFont::Size::Large ? 22 : 16;
}

constexpr static KDCoordinate UpperBoundVerticalMargin(KDFont::Size font) {
  return font == KDFont::Size::Large ? 2 : 0;
}
constexpr static KDCoordinate LowerBoundVerticalMargin(KDFont::Size font) {
  return font == KDFont::Size::Large ? 2 : 1;
}

constexpr static KDCoordinate ArgumentHorizontalMargin(KDFont::Size font) {
  return font == KDFont::Size::Large ? 2 : 0;
}
constexpr static const char* k_equalSign = "=";
constexpr static KDCoordinate k_lineThickness = 1;

KDCoordinate subscriptBaseline(const Tree* node, KDFont::Size font) {
  return std::max<KDCoordinate>(
      std::max(Render::Baseline(node->child(k_variableIndex)),
               Render::Baseline(node->child(k_lowerBoundIndex))),
      KDFont::Font(font)->stringSize(k_equalSign).height() / 2);
}

KDSize lowerBoundSizeWithVariableEquals(const Tree* node, KDFont::Size font) {
  KDSize variableSize = Render::Size(node->child(k_variableIndex));
  KDSize lowerBoundSize = Render::Size(node->child(k_lowerBoundIndex));
  KDSize equalSize = KDFont::Font(font)->stringSize(k_equalSign);
  return KDSize(
      variableSize.width() + equalSize.width() + lowerBoundSize.width(),
      subscriptBaseline(node, font) +
          std::max({variableSize.height() -
                        Render::Baseline(node->child(k_variableIndex)),
                    lowerBoundSize.height() -
                        Render::Baseline(node->child(k_lowerBoundIndex)),
                    equalSize.height() / 2}));
}

KDCoordinate completeLowerBoundX(const Tree* node, KDFont::Size font) {
  KDSize upperBoundSize = Render::Size(node->child(k_upperBoundIndex));
  return std::max({0,
                   (SymbolWidth(font) -
                    lowerBoundSizeWithVariableEquals(node, font).width()) /
                       2,
                   (upperBoundSize.width() -
                    lowerBoundSizeWithVariableEquals(node, font).width()) /
                       2});
}

}  // namespace Parametric

namespace Sum {
constexpr static int k_significantPixelWidth = 4;
static_assert(Parametric::SymbolHeight(KDFont::Size::Large) % 2 != 0 &&
                  Parametric::SymbolHeight(KDFont::Size::Small) % 2 != 0,
              "sum_layout : SymbolHeight is even");
}  // namespace Sum

namespace Derivative {
constexpr static KDCoordinate k_dxHorizontalMargin = 2;
constexpr static KDCoordinate k_barHorizontalMargin = 2;
constexpr static KDCoordinate k_barWidth = 1;

constexpr static const char* k_dString = "d";

KDCoordinate orderHeightOffset(const Tree* node, KDFont::Size font) {
  if (node->isDerivativeLayout()) {
    return 0;
  }
  return Render::Height(node->child(k_orderIndex)) -
         VerticalOffset::k_indiceHeight;
}
KDCoordinate orderWidth(const Tree* node, KDFont::Size font) {
  if (node->isDerivativeLayout()) {
    return 0;
  }
  return Render::Width(node->child(k_orderIndex));
}

KDPoint positionOfDInNumerator(const Tree* node, KDFont::Size font) {
  return KDPoint(
      (Render::Width(node->child(k_variableIndex)) + k_dxHorizontalMargin) / 2 +
          Escher::Metric::FractionAndConjugateHorizontalMargin +
          Escher::Metric::FractionAndConjugateHorizontalOverflow,
      Render::Baseline(node) -
          KDFont::Font(font)->stringSize(k_dString).height() -
          Fraction::k_lineMargin - Fraction::k_lineHeight);
}

KDPoint positionOfDInDenominator(const Tree* node, KDFont::Size font) {
  return KDPoint(Escher::Metric::FractionAndConjugateHorizontalMargin +
                     Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Render::Baseline(node) + Fraction::k_lineMargin +
                     orderHeightOffset(node, font) +
                     Render::Height(node->child(k_variableIndex)) -
                     KDFont::Font(font)->stringSize(k_dString).height());
}

KDPoint positionOfVariableInFractionSlot(const Tree* node, KDFont::Size font) {
  KDPoint positionOfD = positionOfDInDenominator(node, font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(k_dString).width() +
          k_dxHorizontalMargin,
      positionOfD.y() + KDFont::Font(font)->stringSize(k_dString).height() -
          Render::Height(node->child(k_variableIndex)));
}

KDCoordinate fractionBarWidth(const Tree* node, KDFont::Size font) {
  return 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow +
         KDFont::Font(font)->stringSize(k_dString).width() +
         k_dxHorizontalMargin + Render::Width(node->child(k_variableIndex)) +
         orderWidth(node, font);
}

KDCoordinate parenthesesWidth(const Tree* node, KDFont::Size font) {
  return 2 * Parenthesis::k_parenthesisWidth +
         Render::Width(node->child(k_derivandIndex));
}

KDCoordinate abscissaBaseline(const Tree* node, KDFont::Size font) {
  KDCoordinate variableHeight = Render::Height(node->child(k_variableIndex));
  KDCoordinate dfdxBottom = std::max(
      positionOfVariableInFractionSlot(node, font).y() + variableHeight,
      Render::Baseline(node) + Render::Height(node->child(k_derivandIndex)) -
          Render::Baseline(node->child(k_derivandIndex)));
  return dfdxBottom - variableHeight +
         Render::Baseline(node->child(k_variableIndex));
}

KDPoint positionOfVariableInAssignmentSlot(const Tree* node,
                                           KDFont::Size font) {
  return KDPoint(2 * (Escher::Metric::FractionAndConjugateHorizontalMargin +
                      k_barHorizontalMargin) +
                     fractionBarWidth(node, font) +
                     parenthesesWidth(node, font) + k_barWidth,
                 abscissaBaseline(node, font) -
                     Render::Baseline(node->child(k_variableIndex)));
}

KDCoordinate parenthesisBaseline(const Tree* node, KDFont::Size font) {
  return Parenthesis::BaselineGivenChildHeightAndBaseline(
      Render::Height(node->child(k_derivandIndex)),
      Render::Baseline(node->child(k_derivandIndex)));
}

KDPoint positionOfLeftParenthesis(const Tree* node, KDFont::Size font) {
  return KDPoint(positionOfVariableInFractionSlot(node, font).x() +
                     Render::Width(node->child(k_variableIndex)) +
                     orderWidth(node, font) +
                     Escher::Metric::FractionAndConjugateHorizontalMargin +
                     Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Render::Baseline(node) - parenthesisBaseline(node, font));
}

KDPoint positionOfRightParenthesis(const Tree* node, KDFont::Size font,
                                   KDSize derivandSize) {
  return positionOfLeftParenthesis(node, font)
      .translatedBy(
          KDPoint(Parenthesis::k_parenthesisWidth + derivandSize.width(), 0));
}

KDPoint positionOfOrderInNumerator(const Tree* node, KDFont::Size font) {
  KDPoint positionOfD = positionOfDInNumerator(node, font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(k_dString).width(),
      positionOfD.y() - orderHeightOffset(node, font));
}

KDPoint positionOfOrderInDenominator(const Tree* node, KDFont::Size font) {
  KDPoint positionOfX = positionOfVariableInFractionSlot(node, font);
  return KDPoint(positionOfX.x() + Render::Width(node->child(k_variableIndex)),
                 positionOfX.y() - orderHeightOffset(node, font));
}

}  // namespace Derivative

namespace Integral {
// clang-format off
/*
 * Window configuration explained :
 * Vertical margins and offsets
 * +-----------------------------------------------------------------+
 * |                                                |                |
 * |                                        k_boundVerticalMargin    |
 * |                                                |                |
 * |                                       +------------------+      |
 * |                                       | upperBoundHeight |      |
 * |                                       +------------------+      |
 * |             +++       |                        |                |
 * |            +++   k_symbolHeight     k_integrandVerticalMargin   |
 * |           +++         |                        |                |
 * |                                                                 |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||         centralArgumentHeight                     |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |                                                                 |
 * |           +++         |                      |                  |
 * |          +++    k_symbolHeight     k_integrandVerticalMargin    |
 * |         +++           |                      |                  |
 * |                                     +------------------+        |
 * |                                     | lowerBoundHeight |        |
 * |                                     +------------------+        |
 * |                                              |                  |
 * |                                      k_boundVerticalMargin      |
 * |                                              |                  |
 * +-----------------------------------------------------------------+
 *
 * Horizontal margins and offsets
 * +-------------------------------------------------------------------------------------------------------+
 * |                                                                                                       |                                                                                                |
 * |           |             |                       |+---------------+|                           |       |
 * |           |k_symbolWidth|k_boundHorizontalMargin||upperBoundWidth||k_integrandHorizontalMargin|       |
 * |           |             |                       |+---------------+|                           |       |
 * |                      ***                                                                              |
 * |                    ***                                                                                |
 * |                  ***                                                                          |       |
 * |                ***                                                                                    |
 * |              ***                                                                                      |
 * |            ***                                                                                |       |
 * |            |||                                                                                        |
 * |            |||                                                                                        |
 * |            |||                                                                                |       |
 * |            |||                                                                                        |
 * |            |||                                                                                 x dx   |
 * |            |||                                                                                        |
 * |            |||                                                                                |       |
 * |            |||                                                                                        |
 * |            |||                                                                                        |
 * |            |||                                                                                |       |
 * |            |||                                                                                        |
 * |            |||                                                                                        |
 * |            ***                                                                                |       |
 * |          ***                                                                                          |
 * |        ***                                                                                            |
 * |      ***                                                                                      |       |
 * |    ***                                                                                                |
 * |  ***                                                                                                  |
 * | |             |         |                       |+---------------+|                           |       |
 * | |k_symbolWidth|    a    |k_boundHorizontalMargin||lowerBoundWidth||k_integrandHorizontalMargin|       |
 * | |             |         |                       |+---------------+|                           |       |
 * |                                                                                                       |
 * +-------------------------------------------------------------------------------------------------------+
 * ||| = k_lineThickness
 * a = k_symbolWidth - k_lineThickness
 */
// clang-format on
constexpr static KDCoordinate k_symbolHeight = 9;
constexpr static KDCoordinate k_symbolWidth = 4;
constexpr static KDCoordinate k_boundVerticalMargin = 4;
constexpr static KDCoordinate k_boundHorizontalMargin = 3;
constexpr static KDCoordinate k_differentialHorizontalMargin = 3;
constexpr static KDCoordinate k_integrandHorizontalMargin = 2;
constexpr static KDCoordinate k_integrandVerticalMargin = 3;
constexpr static KDCoordinate k_lineThickness = 1;

enum class BoundPosition : uint8_t { UpperBound, LowerBound };
enum class NestedPosition : uint8_t { Previous, Next };

/* Return pointer to the first or the last integral from left to right
 * (considering multiple integrals in a row). */
const Tree* mostNestedIntegral(const Tree* node, NestedPosition position) {
  // TODO
  return node;
}

KDCoordinate boundMaxHeight(const Tree* node, BoundPosition position,
                            KDFont::Size font) {
  // TODO
  return Render::Height(node->child(position == BoundPosition::LowerBound
                                        ? k_lowerBoundIndex
                                        : k_upperBoundIndex));
}

KDCoordinate centralArgumentHeight(const Tree* node, KDFont::Size font) {
  /* When integrals are in a row, the last one is the tallest. We take its
   * central argument height to define the one of the others integrals */
  const Tree* last = mostNestedIntegral(node, NestedPosition::Next);
  if (node == last) {
    KDCoordinate integrandHeight =
        Render::Height(node->child(k_integrandIndex));
    KDCoordinate integrandBaseline =
        Render::Baseline(node->child(k_integrandIndex));
    KDCoordinate differentialHeight =
        Render::Height(node->child(k_differentialIndex));
    KDCoordinate differentialBaseline =
        Render::Baseline(node->child(k_differentialIndex));
    return std::max(integrandBaseline, differentialBaseline) +
           std::max(integrandHeight - integrandBaseline,
                    differentialHeight - differentialBaseline);
  } else {
    return centralArgumentHeight(last, font);
  }
}

}  // namespace Integral

namespace PtCombinatorics {
constexpr static KDCoordinate k_margin = 3;
constexpr static KDCoordinate k_symbolHeight = 16;
constexpr static KDCoordinate k_symbolBaseline = 11;
constexpr static KDCoordinate k_symbolWidth = 12;
constexpr static KDCoordinate k_symbolWidthWithMargins =
    k_symbolWidth + 2 * k_margin;

KDCoordinate AboveSymbol(const Tree* node, KDFont::Size font) {
  return std::max<KDCoordinate>(
      Render::Baseline(node->child(k_nIndex)),
      Render::Baseline(node->child(k_kIndex)) - k_symbolHeight);
}

KDCoordinate TotalHeight(const Tree* node, KDFont::Size font) {
  KDCoordinate underSymbol = std::max<KDCoordinate>(
      Render::Height(node->child(k_kIndex)) -
          Render::Baseline(node->child(k_kIndex)),
      Render::Height(node->child(k_nIndex)) -
          Render::Baseline(node->child(k_nIndex)) - k_symbolHeight);
  return AboveSymbol(node, font) + k_symbolHeight + underSymbol;
}
}  // namespace PtCombinatorics

namespace PtBinomial {
constexpr static KDCoordinate k_barHeight = 6;
}

namespace PtPermute {
using PtCombinatorics::k_symbolHeight, PtCombinatorics::k_symbolWidth;
}

constexpr static KDCoordinate k_gridEntryMargin = 6;

namespace Binomial {
static KDCoordinate KNHeight(const Tree* node, KDFont::Size font) {
  return Render::Height(node->child(k_nIndex)) + k_gridEntryMargin +
         Render::Height(node->child(k_kIndex));
}
}  // namespace Binomial

namespace ListSequence {
constexpr static KDCoordinate k_variableHorizontalMargin = 1;
constexpr static KDCoordinate k_variableBaselineOffset = 2;

KDCoordinate variableSlotBaseline(const Tree* node, KDFont::Size font) {
  return std::max(
      {KDCoordinate(CurlyBrace::HeightGivenChildHeight(
                        Render::Height(node->child(k_functionIndex))) +
                    k_variableBaselineOffset),
       Render::Baseline(node->child(k_upperBoundIndex)),
       Render::Baseline(node->child(k_variableIndex))});
}

KDCoordinate bracesWidth(const Tree* node, KDFont::Size font) {
  return 2 * CurlyBrace::k_curlyBraceWidth +
         Render::Width(node->child(k_functionIndex));
}

KDPoint positionOfVariable(const Tree* node, KDFont::Size font) {
  return KDPoint(k_variableHorizontalMargin + bracesWidth(node, font),
                 variableSlotBaseline(node, font) -
                     Render::Baseline(node->child(k_variableIndex)));
}

}  // namespace ListSequence

}  // namespace PoincareJ

#endif
