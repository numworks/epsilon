#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H

#include <escher/metric.h>
#include <kandinsky/coordinate.h>

#include "assert.h"
#include "indices.h"
#include "render.h"

namespace PoincareJ {

// Helpers to access the private methods of Render
inline KDCoordinate Baseline(const LayoutT* l) { return Render::Baseline(l); }
inline KDCoordinate Baseline(const Rack* r) { return Render::Baseline(r); }
inline KDSize Size(const Rack* r) { return Render::Size(r); }
inline KDCoordinate Width(const Rack* r) { return Size(r).width(); }
inline KDCoordinate Height(const Rack* r) { return Size(r).height(); }

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

constexpr KDCoordinate VerticalMargin(KDCoordinate childHeight,
                                      KDCoordinate minVerticalMargin) {
  /* If the child height is below the threshold, make it bigger so that
   * The bracket pair maintains the right height */
  KDCoordinate verticalMargin = minVerticalMargin;

  if (childHeight < k_minimalChildHeight) {
    verticalMargin += (k_minimalChildHeight - childHeight) / 2;
  }
  return verticalMargin;
}
constexpr KDCoordinate Height(KDCoordinate childHeight,
                              KDCoordinate minVerticalMargin) {
  return childHeight + 2 * VerticalMargin(childHeight, minVerticalMargin);
}

constexpr KDCoordinate Baseline(KDCoordinate childHeight,
                                KDCoordinate childBaseline,
                                KDCoordinate minVerticalMargin) {
  return childBaseline + VerticalMargin(childHeight, minVerticalMargin);
}

constexpr KDPoint ChildOffset(KDCoordinate minVerticalMargin,
                              KDCoordinate bracketWidth,
                              KDCoordinate childHeight) {
  return KDPoint(bracketWidth, VerticalMargin(childHeight, minVerticalMargin));
}
}  // namespace Pair

namespace SquareBracketPair {
using Pair::k_lineThickness;
constexpr static KDCoordinate k_internalWidthMargin = 5;
constexpr static KDCoordinate k_externalWidthMargin = 2;
constexpr static KDCoordinate k_bracketWidth =
    k_internalWidthMargin + k_lineThickness + k_externalWidthMargin;
constexpr static KDCoordinate k_minVerticalMargin = 1;
constexpr static KDCoordinate k_doubleBarMargin = 2;

constexpr KDSize SizeGivenChildSize(KDSize childSize) {
  return KDSize(2 * k_bracketWidth + childSize.width(),
                Pair::Height(childSize.height(), k_minVerticalMargin));
}

constexpr KDPoint ChildOffset(KDCoordinate childHeight) {
  return Pair::ChildOffset(k_minVerticalMargin, k_bracketWidth, childHeight);
}
}  // namespace SquareBracketPair

namespace AbsoluteValue {
constexpr static KDCoordinate k_k_innerWidthMargin = 2;
constexpr static KDCoordinate k_bracketWidth =
    Pair::k_lineThickness + k_k_innerWidthMargin +
    SquareBracketPair::k_externalWidthMargin;
constexpr static KDCoordinate k_minVerticalMargin = 0;
}  // namespace AbsoluteValue

namespace VectorNorm {
constexpr static KDCoordinate k_innerWidthMargin = 2;
constexpr static KDCoordinate k_bracketWidth =
    2 * Pair::k_lineThickness + SquareBracketPair::k_doubleBarMargin +
    k_innerWidthMargin + SquareBracketPair::k_externalWidthMargin;
constexpr static KDCoordinate k_minVerticalMargin = 0;
}  // namespace VectorNorm

namespace CurlyBrace {
using Pair::k_lineThickness;
constexpr static KDCoordinate k_curveHeight = 6;
constexpr static KDCoordinate k_curveWidth = 5;
constexpr static KDCoordinate k_centerHeight = 3;
constexpr static KDCoordinate k_centerWidth = 3;
constexpr static KDCoordinate k_widthMargin = 1;
constexpr static KDCoordinate k_minVerticalMargin = 1;
constexpr static KDCoordinate k_curlyBraceWidth =
    2 * k_widthMargin + k_centerWidth + k_curveWidth - k_lineThickness;
constexpr KDCoordinate Height(KDCoordinate childHeight) {
  return Pair::Height(childHeight, k_minVerticalMargin);
}
constexpr KDCoordinate Baseline(KDCoordinate childHeight,
                                KDCoordinate childBaseline) {
  return Pair::Baseline(childHeight, childBaseline, k_minVerticalMargin);
}
}  // namespace CurlyBrace

namespace Parenthesis {
constexpr static KDCoordinate k_widthMargin = 1;
constexpr static KDCoordinate k_curveWidth = 5;
constexpr static KDCoordinate k_curveHeight = 7;
constexpr static KDCoordinate k_minVerticalMargin = 2;
constexpr static KDCoordinate k_parenthesisWidth =
    2 * k_widthMargin + k_curveWidth;

constexpr KDCoordinate Height(KDCoordinate childHeight) {
  return Pair::Height(childHeight, k_minVerticalMargin);
}

constexpr KDCoordinate Baseline(KDCoordinate childHeight,
                                KDCoordinate childBaseline) {
  return Pair::Baseline(childHeight, childBaseline, k_minVerticalMargin);
}
}  // namespace Parenthesis

namespace Pair {
inline KDCoordinate BracketWidth(const LayoutT* node) {
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

inline KDCoordinate MinVerticalMargin(const LayoutT* node) {
  switch (node->layoutType()) {
    case LayoutType::Ceiling:
    case LayoutType::Floor:
      return SquareBracketPair::k_minVerticalMargin;
    case LayoutType::AbsoluteValue:
      return AbsoluteValue::k_minVerticalMargin;
    case LayoutType::VectorNorm:
      return VectorNorm::k_minVerticalMargin;
    case LayoutType::CurlyBrace:
      return CurlyBrace::k_minVerticalMargin;
    case LayoutType::Parenthesis:
      return Parenthesis::k_minVerticalMargin;
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

inline KDSize AdjustedIndexSize(const LayoutT* node, KDFont::Size font) {
  return node->isSquareRootLayout()
             ? KDSize(k_leftRadixWidth, 0)
             : KDSize(std::max(k_leftRadixWidth, Width(node->child(1))),
                      Height(node->child(1)));
}
}  // namespace NthRoot

namespace Parametric {
constexpr KDCoordinate SymbolHeight(KDFont::Size font) {
  return font == KDFont::Size::Large ? 29 : 21;
}

constexpr KDCoordinate SymbolWidth(KDFont::Size font) {
  return font == KDFont::Size::Large ? 22 : 16;
}

constexpr KDCoordinate UpperBoundVerticalMargin(KDFont::Size font) {
  return font == KDFont::Size::Large ? 2 : 0;
}

constexpr KDCoordinate LowerBoundVerticalMargin(KDFont::Size font) {
  return font == KDFont::Size::Large ? 2 : 1;
}

constexpr KDCoordinate ArgumentHorizontalMargin(KDFont::Size font) {
  return font == KDFont::Size::Large ? 2 : 0;
}

constexpr static const char* k_equalSign = "=";
constexpr static KDCoordinate k_lineThickness = 1;

inline KDCoordinate subscriptBaseline(const LayoutT* node, KDFont::Size font) {
  return std::max<KDCoordinate>(
      std::max(Baseline(node->child(k_variableIndex)),
               Baseline(node->child(k_lowerBoundIndex))),
      KDFont::Font(font)->stringSize(k_equalSign).height() / 2);
}

inline KDSize lowerBoundSizeWithVariableEquals(const LayoutT* node,
                                               KDFont::Size font) {
  KDSize variableSize = Size(node->child(k_variableIndex));
  KDSize lowerBoundSize = Size(node->child(k_lowerBoundIndex));
  KDSize equalSize = KDFont::Font(font)->stringSize(k_equalSign);
  return KDSize(
      variableSize.width() + equalSize.width() + lowerBoundSize.width(),
      subscriptBaseline(node, font) +
          std::max(
              {variableSize.height() - Baseline(node->child(k_variableIndex)),
               lowerBoundSize.height() -
                   Baseline(node->child(k_lowerBoundIndex)),
               equalSize.height() / 2}));
}

inline KDCoordinate completeLowerBoundX(const LayoutT* node,
                                        KDFont::Size font) {
  KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
  return std::max({0,
                   (SymbolWidth(font) -
                    lowerBoundSizeWithVariableEquals(node, font).width()) /
                       2,
                   (upperBoundSize.width() -
                    lowerBoundSizeWithVariableEquals(node, font).width()) /
                       2});
}

inline KDCoordinate upperBoundWidth(const LayoutT* node, KDFont::Size font) {
  return Width(node->child(k_upperBoundIndex));
}

inline KDPoint leftParenthesisPosition(const LayoutT* node, KDFont::Size font) {
  KDSize argumentSize = Size(node->child(k_argumentIndex));
  KDCoordinate argumentBaseline = Baseline(node->child(k_argumentIndex));
  KDCoordinate lowerboundWidth =
      lowerBoundSizeWithVariableEquals(node, font).width();

  KDCoordinate x = std::max({SymbolWidth(font), lowerboundWidth,
                             upperBoundWidth(node, font)}) +
                   ArgumentHorizontalMargin(font);
  KDCoordinate y = Baseline(node) - Parenthesis::Baseline(argumentSize.height(),
                                                          argumentBaseline);
  return {x, y};
}

inline KDPoint rightParenthesisPosition(const LayoutT* node, KDFont::Size font,
                                        KDSize argumentSize) {
  return leftParenthesisPosition(node, font)
      .translatedBy(
          KDPoint(Parenthesis::k_parenthesisWidth + argumentSize.width(), 0));
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

inline KDCoordinate orderHeightOffset(const LayoutT* node, KDFont::Size font) {
  if (node->isDerivativeLayout()) {
    return 0;
  }
  return Height(node->child(k_orderIndex)) - VerticalOffset::k_indiceHeight;
}

inline KDCoordinate orderWidth(const LayoutT* node, KDFont::Size font) {
  if (node->isDerivativeLayout()) {
    return 0;
  }
  return Width(node->child(k_orderIndex));
}

inline KDPoint positionOfDInNumerator(const LayoutT* node, KDFont::Size font) {
  return KDPoint(
      (Width(node->child(k_variableIndex)) + k_dxHorizontalMargin) / 2 +
          Escher::Metric::FractionAndConjugateHorizontalMargin +
          Escher::Metric::FractionAndConjugateHorizontalOverflow,
      Baseline(node) - KDFont::Font(font)->stringSize(k_dString).height() -
          Fraction::k_lineMargin - Fraction::k_lineHeight);
}

inline KDPoint positionOfDInDenominator(const LayoutT* node,
                                        KDFont::Size font) {
  return KDPoint(Escher::Metric::FractionAndConjugateHorizontalMargin +
                     Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Baseline(node) + Fraction::k_lineMargin +
                     orderHeightOffset(node, font) +
                     Height(node->child(k_variableIndex)) -
                     KDFont::Font(font)->stringSize(k_dString).height());
}

inline KDPoint positionOfVariableInFractionSlot(const LayoutT* node,
                                                KDFont::Size font) {
  KDPoint positionOfD = positionOfDInDenominator(node, font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(k_dString).width() +
          k_dxHorizontalMargin,
      positionOfD.y() + KDFont::Font(font)->stringSize(k_dString).height() -
          Height(node->child(k_variableIndex)));
}

inline KDCoordinate fractionBarWidth(const LayoutT* node, KDFont::Size font) {
  return 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow +
         KDFont::Font(font)->stringSize(k_dString).width() +
         k_dxHorizontalMargin + Width(node->child(k_variableIndex)) +
         orderWidth(node, font);
}

inline KDCoordinate parenthesesWidth(const LayoutT* node, KDFont::Size font) {
  return 2 * Parenthesis::k_parenthesisWidth +
         Width(node->child(k_derivandIndex));
}

inline KDCoordinate abscissaBaseline(const LayoutT* node, KDFont::Size font) {
  KDCoordinate variableHeight = Height(node->child(k_variableIndex));
  KDCoordinate dfdxBottom = std::max(
      positionOfVariableInFractionSlot(node, font).y() + variableHeight,
      Baseline(node) + Height(node->child(k_derivandIndex)) -
          Baseline(node->child(k_derivandIndex)));
  return dfdxBottom - variableHeight + Baseline(node->child(k_variableIndex));
}

inline KDPoint positionOfVariableInAssignmentSlot(const LayoutT* node,
                                                  KDFont::Size font) {
  return KDPoint(
      2 * (Escher::Metric::FractionAndConjugateHorizontalMargin +
           k_barHorizontalMargin) +
          fractionBarWidth(node, font) + parenthesesWidth(node, font) +
          k_barWidth,
      abscissaBaseline(node, font) - Baseline(node->child(k_variableIndex)));
}

inline KDCoordinate parenthesisBaseline(const LayoutT* node,
                                        KDFont::Size font) {
  return Parenthesis::Baseline(Height(node->child(k_derivandIndex)),
                               Baseline(node->child(k_derivandIndex)));
}

inline KDPoint positionOfLeftParenthesis(const LayoutT* node,
                                         KDFont::Size font) {
  return KDPoint(positionOfVariableInFractionSlot(node, font).x() +
                     Width(node->child(k_variableIndex)) +
                     orderWidth(node, font) +
                     Escher::Metric::FractionAndConjugateHorizontalMargin +
                     Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Baseline(node) - parenthesisBaseline(node, font));
}

inline KDPoint positionOfRightParenthesis(const LayoutT* node,
                                          KDFont::Size font,
                                          KDSize derivandSize) {
  return positionOfLeftParenthesis(node, font)
      .translatedBy(
          KDPoint(Parenthesis::k_parenthesisWidth + derivandSize.width(), 0));
}

inline KDPoint positionOfOrderInNumerator(const LayoutT* node,
                                          KDFont::Size font) {
  KDPoint positionOfD = positionOfDInNumerator(node, font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(k_dString).width(),
      positionOfD.y() - orderHeightOffset(node, font));
}

inline KDPoint positionOfOrderInDenominator(const LayoutT* node,
                                            KDFont::Size font) {
  KDPoint positionOfX = positionOfVariableInFractionSlot(node, font);
  return KDPoint(positionOfX.x() + Width(node->child(k_variableIndex)),
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
inline const LayoutT* mostNestedIntegral(const LayoutT* node,
                                         NestedPosition position) {
  // TODO
  return node;
}

inline KDCoordinate boundMaxHeight(const LayoutT* node, BoundPosition position,
                                   KDFont::Size font) {
  // TODO
  return Height(node->child(position == BoundPosition::LowerBound
                                ? k_lowerBoundIndex
                                : k_upperBoundIndex));
}

inline KDCoordinate centralArgumentHeight(const LayoutT* node,
                                          KDFont::Size font) {
  /* When integrals are in a row, the last one is the tallest. We take its
   * central argument height to define the one of the others integrals */
  const LayoutT* last = mostNestedIntegral(node, NestedPosition::Next);
  if (node == last) {
    KDCoordinate integrandHeight = Height(node->child(k_integrandIndex));
    KDCoordinate integrandBaseline = Baseline(node->child(k_integrandIndex));
    KDCoordinate differentialHeight = Height(node->child(k_differentialIndex));
    KDCoordinate differentialBaseline =
        Baseline(node->child(k_differentialIndex));
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

inline KDCoordinate AboveSymbol(const LayoutT* node, KDFont::Size font) {
  return std::max<KDCoordinate>(
      Baseline(node->child(k_nIndex)),
      Baseline(node->child(k_kIndex)) - k_symbolHeight);
}

inline KDCoordinate TotalHeight(const LayoutT* node, KDFont::Size font) {
  KDCoordinate underSymbol = std::max<KDCoordinate>(
      Height(node->child(k_kIndex)) - Baseline(node->child(k_kIndex)),
      Height(node->child(k_nIndex)) - Baseline(node->child(k_nIndex)) -
          k_symbolHeight);
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
inline KDCoordinate KNHeight(const LayoutT* node, KDFont::Size font) {
  return Height(node->child(k_nIndex)) + k_gridEntryMargin +
         Height(node->child(k_kIndex));
}
}  // namespace Binomial

namespace ListSequence {
constexpr static KDCoordinate k_variableHorizontalMargin = 1;
constexpr static KDCoordinate k_variableBaselineOffset = 2;

inline KDCoordinate variableSlotBaseline(const LayoutT* node,
                                         KDFont::Size font) {
  return std::max(
      {KDCoordinate(CurlyBrace::Height(Height(node->child(k_functionIndex))) +
                    k_variableBaselineOffset),
       Baseline(node->child(k_upperBoundIndex)),
       Baseline(node->child(k_variableIndex))});
}

inline KDCoordinate bracesWidth(const LayoutT* node, KDFont::Size font) {
  return 2 * CurlyBrace::k_curlyBraceWidth +
         Width(node->child(k_functionIndex));
}

inline KDPoint positionOfVariable(const LayoutT* node, KDFont::Size font) {
  return KDPoint(k_variableHorizontalMargin + bracesWidth(node, font),
                 variableSlotBaseline(node, font) -
                     Baseline(node->child(k_variableIndex)));
}

}  // namespace ListSequence
}  // namespace PoincareJ

#endif
