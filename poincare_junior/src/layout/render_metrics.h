#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H

#include <escher/metric.h>
#include <kandinsky/coordinate.h>

#include "assert.h"
#include "indices.h"
#include "render.h"

namespace PoincareJ {

// Helpers to access the private methods of Render
inline KDCoordinate Baseline(const Layout* l) { return Render::Baseline(l); }
inline KDCoordinate Baseline(const Rack* r) { return Render::Baseline(r); }
inline KDSize Size(const Rack* r) { return Render::Size(r); }
inline KDCoordinate Width(const Rack* r) { return Size(r).width(); }
inline KDCoordinate Height(const Rack* r) { return Size(r).height(); }

namespace Fraction {
constexpr KDCoordinate k_lineMargin = 2;
constexpr KDCoordinate k_lineHeight = 1;
constexpr KDCoordinate k_horizontalOverflow =
    Escher::Metric::FractionAndConjugateHorizontalOverflow;
constexpr KDCoordinate k_horizontalMargin =
    Escher::Metric::FractionAndConjugateHorizontalMargin;
}  // namespace Fraction

namespace CodePoint {
constexpr KDCoordinate k_middleDotWidth = 5;
}

namespace VerticalOffset {
constexpr KDCoordinate k_indiceHeight = 10;
}

namespace Pair {
constexpr KDCoordinate k_lineThickness = 1;

constexpr KDCoordinate k_minimalChildHeight =
    Escher::Metric::MinimalBracketAndParenthesisChildHeight;

constexpr uint8_t k_temporaryBlendAlpha = 0x60;

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
constexpr KDCoordinate k_internalWidthMargin = 5;
constexpr KDCoordinate k_externalWidthMargin = 2;
constexpr KDCoordinate k_bracketWidth =
    k_internalWidthMargin + k_lineThickness + k_externalWidthMargin;
constexpr KDCoordinate k_minVerticalMargin = 1;
constexpr KDCoordinate k_doubleBarMargin = 2;

constexpr KDSize SizeGivenChildSize(KDSize childSize) {
  return KDSize(2 * k_bracketWidth + childSize.width(),
                Pair::Height(childSize.height(), k_minVerticalMargin));
}

constexpr KDPoint ChildOffset(KDCoordinate childHeight) {
  return Pair::ChildOffset(k_minVerticalMargin, k_bracketWidth, childHeight);
}
}  // namespace SquareBracketPair

namespace AbsoluteValue {
constexpr KDCoordinate k_k_innerWidthMargin = 2;
constexpr KDCoordinate k_bracketWidth =
    Pair::k_lineThickness + k_k_innerWidthMargin +
    SquareBracketPair::k_externalWidthMargin;
constexpr KDCoordinate k_minVerticalMargin = 0;
}  // namespace AbsoluteValue

namespace VectorNorm {
constexpr KDCoordinate k_innerWidthMargin = 2;
constexpr KDCoordinate k_bracketWidth =
    2 * Pair::k_lineThickness + SquareBracketPair::k_doubleBarMargin +
    k_innerWidthMargin + SquareBracketPair::k_externalWidthMargin;
constexpr KDCoordinate k_minVerticalMargin = 0;
}  // namespace VectorNorm

namespace CurlyBrace {
using Pair::k_lineThickness;
constexpr KDCoordinate k_curveHeight = 6;
constexpr KDCoordinate k_curveWidth = 5;
constexpr KDCoordinate k_centerHeight = 3;
constexpr KDCoordinate k_centerWidth = 3;
constexpr KDCoordinate k_widthMargin = 1;
constexpr KDCoordinate k_minVerticalMargin = 1;
constexpr KDCoordinate k_curlyBraceWidth =
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
constexpr KDCoordinate k_widthMargin = 1;
constexpr KDCoordinate k_curveWidth = 5;
constexpr KDCoordinate k_curveHeight = 7;
constexpr KDCoordinate k_minVerticalMargin = 2;
constexpr KDCoordinate k_parenthesisWidth = 2 * k_widthMargin + k_curveWidth;

constexpr KDCoordinate Height(KDCoordinate childHeight) {
  return Pair::Height(childHeight, k_minVerticalMargin);
}

constexpr KDCoordinate Baseline(KDCoordinate childHeight,
                                KDCoordinate childBaseline) {
  return Pair::Baseline(childHeight, childBaseline, k_minVerticalMargin);
}

constexpr KDCoordinate VerticalMargin(KDCoordinate childHeight) {
  return Pair::VerticalMargin(childHeight, k_minVerticalMargin);
}
}  // namespace Parenthesis

namespace Pair {
inline KDCoordinate BracketWidth(const Layout* node) {
  switch (node->layoutType()) {
    case LayoutType::Ceil:
    case LayoutType::Floor:
      return SquareBracketPair::k_bracketWidth;
    case LayoutType::Abs:
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

inline KDCoordinate MinVerticalMargin(const Layout* node) {
  switch (node->layoutType()) {
    case LayoutType::Ceil:
    case LayoutType::Floor:
      return SquareBracketPair::k_minVerticalMargin;
    case LayoutType::Abs:
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
constexpr KDCoordinate k_overlineWidth = 1;
constexpr KDCoordinate k_overlineVerticalMargin = 1;
}  // namespace Conjugate

namespace NthRoot {
constexpr KDCoordinate k_heightMargin = 2;
constexpr KDCoordinate k_widthMargin = 2;
constexpr KDCoordinate k_radixLineThickness = 1;

constexpr KDCoordinate k_leftRadixHeight = 9;
constexpr KDCoordinate k_leftRadixWidth = 5;

inline KDSize AdjustedIndexSize(const Layout* node, KDFont::Size font) {
  return node->isSqrtLayout()
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

constexpr const char* k_equalSign = "=";
constexpr KDCoordinate k_lineThickness = 1;

inline KDCoordinate SubscriptBaseline(const Layout* node, KDFont::Size font) {
  return std::max<KDCoordinate>(
      std::max(Baseline(node->child(k_variableIndex)),
               Baseline(node->child(k_lowerBoundIndex))),
      KDFont::Font(font)->stringSize(k_equalSign).height() / 2);
}

inline KDSize LowerBoundSizeWithVariableEquals(const Layout* node,
                                               KDFont::Size font) {
  KDSize variableSize = Size(node->child(k_variableIndex));
  KDSize lowerBoundSize = Size(node->child(k_lowerBoundIndex));
  KDSize equalSize = KDFont::Font(font)->stringSize(k_equalSign);
  return KDSize(
      variableSize.width() + equalSize.width() + lowerBoundSize.width(),
      SubscriptBaseline(node, font) +
          std::max(
              {variableSize.height() - Baseline(node->child(k_variableIndex)),
               lowerBoundSize.height() -
                   Baseline(node->child(k_lowerBoundIndex)),
               equalSize.height() / 2}));
}

inline KDCoordinate CompleteLowerBoundX(const Layout* node, KDFont::Size font) {
  KDSize upperBoundSize = Size(node->child(k_upperBoundIndex));
  return std::max({0,
                   (SymbolWidth(font) -
                    LowerBoundSizeWithVariableEquals(node, font).width()) /
                       2,
                   (upperBoundSize.width() -
                    LowerBoundSizeWithVariableEquals(node, font).width()) /
                       2});
}

inline KDCoordinate UpperBoundWidth(const Layout* node, KDFont::Size font) {
  return Width(node->child(k_upperBoundIndex));
}

inline KDPoint LeftParenthesisPosition(const Layout* node, KDFont::Size font) {
  KDSize argumentSize = Size(node->child(k_argumentIndex));
  KDCoordinate argumentBaseline = Baseline(node->child(k_argumentIndex));
  KDCoordinate lowerboundWidth =
      LowerBoundSizeWithVariableEquals(node, font).width();

  KDCoordinate x = std::max({SymbolWidth(font), lowerboundWidth,
                             UpperBoundWidth(node, font)}) +
                   ArgumentHorizontalMargin(font);
  KDCoordinate y = Baseline(node) - Parenthesis::Baseline(argumentSize.height(),
                                                          argumentBaseline);
  return {x, y};
}

inline KDPoint RightParenthesisPosition(const Layout* node, KDFont::Size font,
                                        KDSize argumentSize) {
  return LeftParenthesisPosition(node, font)
      .translatedBy(
          KDPoint(Parenthesis::k_parenthesisWidth + argumentSize.width(), 0));
}
}  // namespace Parametric

namespace Sum {
constexpr int k_significantPixelWidth = 4;
static_assert(Parametric::SymbolHeight(KDFont::Size::Large) % 2 != 0 &&
                  Parametric::SymbolHeight(KDFont::Size::Small) % 2 != 0,
              "sum_layout : SymbolHeight is even");
}  // namespace Sum

namespace Derivative {
constexpr KDCoordinate k_dxHorizontalMargin = 2;
constexpr KDCoordinate k_barHorizontalMargin = 2;
constexpr KDCoordinate k_barWidth = 1;

constexpr const char* k_dString = "d";

inline KDCoordinate OrderHeightOffset(const Layout* node, KDFont::Size font) {
  if (node->isDiffLayout()) {
    return 0;
  }
  return Height(node->child(k_orderIndex)) - VerticalOffset::k_indiceHeight;
}

inline KDCoordinate OrderWidth(const Layout* node, KDFont::Size font) {
  if (node->isDiffLayout()) {
    return 0;
  }
  return Width(node->child(k_orderIndex));
}

inline KDPoint PositionOfDInNumerator(const Layout* node, KDCoordinate baseline,
                                      KDFont::Size font) {
  return KDPoint(
      (Width(node->child(k_variableIndex)) + k_dxHorizontalMargin) / 2 +
          Escher::Metric::FractionAndConjugateHorizontalMargin +
          Escher::Metric::FractionAndConjugateHorizontalOverflow,
      baseline - KDFont::Font(font)->stringSize(k_dString).height() -
          Fraction::k_lineMargin - Fraction::k_lineHeight);
}

inline KDPoint PositionOfDInDenominator(const Layout* node,
                                        KDCoordinate baseline,
                                        KDFont::Size font) {
  return KDPoint(Escher::Metric::FractionAndConjugateHorizontalMargin +
                     Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 baseline + Fraction::k_lineMargin +
                     OrderHeightOffset(node, font) +
                     Height(node->child(k_variableIndex)) -
                     KDFont::Font(font)->stringSize(k_dString).height());
}

inline KDPoint PositionOfVariableInFractionSlot(const Layout* node,
                                                KDCoordinate baseline,
                                                KDFont::Size font) {
  KDPoint positionOfD = PositionOfDInDenominator(node, baseline, font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(k_dString).width() +
          k_dxHorizontalMargin,
      positionOfD.y() + KDFont::Font(font)->stringSize(k_dString).height() -
          Height(node->child(k_variableIndex)));
}

inline KDCoordinate FractionBarWidth(const Layout* node, KDFont::Size font) {
  return 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow +
         KDFont::Font(font)->stringSize(k_dString).width() +
         k_dxHorizontalMargin + Width(node->child(k_variableIndex)) +
         OrderWidth(node, font);
}

inline KDCoordinate ParenthesesWidth(const Layout* node, KDFont::Size font) {
  return 2 * Parenthesis::k_parenthesisWidth +
         Width(node->child(k_derivandIndex));
}

inline KDCoordinate AbscissaBaseline(const Layout* node, KDCoordinate baseline,
                                     KDFont::Size font) {
  KDCoordinate variableHeight = Height(node->child(k_variableIndex));
  KDCoordinate dfdxBottom =
      std::max(PositionOfVariableInFractionSlot(node, baseline, font).y() +
                   variableHeight,
               baseline + Height(node->child(k_derivandIndex)) -
                   Baseline(node->child(k_derivandIndex)));
  return dfdxBottom - variableHeight + Baseline(node->child(k_variableIndex));
}

inline KDPoint PositionOfVariableInAssignmentSlot(const Layout* node,
                                                  KDCoordinate baseline,
                                                  KDFont::Size font) {
  return KDPoint(2 * (Escher::Metric::FractionAndConjugateHorizontalMargin +
                      k_barHorizontalMargin) +
                     FractionBarWidth(node, font) +
                     ParenthesesWidth(node, font) + k_barWidth,
                 AbscissaBaseline(node, baseline, font) -
                     Baseline(node->child(k_variableIndex)));
}

inline KDCoordinate ParenthesisBaseline(const Layout* node, KDFont::Size font) {
  return Parenthesis::Baseline(Height(node->child(k_derivandIndex)),
                               Baseline(node->child(k_derivandIndex)));
}

inline KDPoint PositionOfLeftParenthesis(const Layout* node,
                                         KDCoordinate baseline,
                                         KDFont::Size font) {
  return KDPoint(PositionOfVariableInFractionSlot(node, baseline, font).x() +
                     Width(node->child(k_variableIndex)) +
                     OrderWidth(node, font) +
                     Escher::Metric::FractionAndConjugateHorizontalMargin +
                     Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Baseline(node) - ParenthesisBaseline(node, font));
}

inline KDPoint PositionOfRightParenthesis(const Layout* node,
                                          KDCoordinate baseline,
                                          KDFont::Size font,
                                          KDSize derivandSize) {
  return PositionOfLeftParenthesis(node, baseline, font)
      .translatedBy(
          KDPoint(Parenthesis::k_parenthesisWidth + derivandSize.width(), 0));
}

inline KDPoint PositionOfOrderInNumerator(const Layout* node,
                                          KDCoordinate baseline,
                                          KDFont::Size font) {
  KDPoint positionOfD = PositionOfDInNumerator(node, baseline, font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(k_dString).width(),
      positionOfD.y() - OrderHeightOffset(node, font));
}

inline KDPoint PositionOfOrderInDenominator(const Layout* node,
                                            KDCoordinate baseline,
                                            KDFont::Size font) {
  KDPoint positionOfX = PositionOfVariableInFractionSlot(node, baseline, font);
  return KDPoint(positionOfX.x() + Width(node->child(k_variableIndex)),
                 positionOfX.y() - OrderHeightOffset(node, font));
}
}  // namespace Derivative

namespace Integral {
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
 * +-----------------------------------------------------------------+
 * |                                                                 |
 * |                                     k_integrandHorizontalMargin |
 * |                 k_boundHorizontalMargin            |            |
 * |                            |                       |            |
 * |           |             |  |  |+---------------+|  |  |         |
 * |           |k_symbolWidth|<--->||upperBoundWidth||<--->|         |
 * |           |             |     |+---------------+|     |         |
 * |                      ***                                        |
 * |                    ***                                          |
 * |                  ***                                  |         |
 * |                ***                                              |
 * |              ***                                                |
 * |            ***                                        |         |
 * |            |||                                                  |
 * |            |||                                                  |
 * |            |||                                        |         |
 * |            |||                                                  |
 * |            |||                                         x dx     |
 * |            |||                                                  |
 * |            |||                                        |         |
 * |            |||                                                  |
 * |            |||                                                  |
 * |            |||                                        |         |
 * |            |||                                                  |
 * |            |||                                                  |
 * |            ***                                        |         |
 * |          ***                                                    |
 * |        ***                                                      |
 * |      ***                                              |         |
 * |    ***                                                          |
 * |  ***                                                            |
 * | |             |         |     |+---------------+|     |         |
 * | |k_symbolWidth|    a    |<--->||lowerBoundWidth||<--->|         |
 * | |             |         |  |  |+---------------+|  |  |         |
 * |                            |                       |            |
 * |                            |        k_integrandHorizontalMargin |
 * |                 k_boundHorizontalMargin                         |
 * |                                                                 |
 * +-----------------------------------------------------------------+
 * ||| = k_lineThickness
 * a = k_symbolWidth - k_lineThickness
 */
constexpr KDCoordinate k_symbolHeight = 9;
constexpr KDCoordinate k_symbolWidth = 4;
constexpr KDCoordinate k_boundVerticalMargin = 4;
constexpr KDCoordinate k_boundHorizontalMargin = 3;
constexpr KDCoordinate k_differentialHorizontalMargin = 3;
constexpr KDCoordinate k_integrandHorizontalMargin = 2;
constexpr KDCoordinate k_integrandVerticalMargin = 3;
constexpr KDCoordinate k_lineThickness = 1;

enum class BoundPosition : uint8_t { UpperBound, LowerBound };
enum class NestedPosition : uint8_t { Previous, Next };

/* Return pointer to the first or the last integral from left to right
 * (considering multiple integrals in a row). */
inline const Layout* MostNestedIntegral(const Layout* node,
                                        NestedPosition position) {
  // TODO
  return node;
}

inline KDCoordinate BoundMaxHeight(const Layout* node, BoundPosition position,
                                   KDFont::Size font) {
  // TODO
  return Height(node->child(position == BoundPosition::LowerBound
                                ? k_lowerBoundIndex
                                : k_upperBoundIndex));
}

inline KDCoordinate CentralArgumentHeight(const Layout* node,
                                          KDFont::Size font) {
  /* When integrals are in a row, the last one is the tallest. We take its
   * central argument height to define the one of the others integrals */
  const Layout* last = MostNestedIntegral(node, NestedPosition::Next);
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
    return CentralArgumentHeight(last, font);
  }
}
}  // namespace Integral

namespace PtCombinatorics {
constexpr KDCoordinate k_margin = 3;
constexpr KDCoordinate k_symbolHeight = 16;
constexpr KDCoordinate k_symbolBaseline = 11;
constexpr KDCoordinate k_symbolWidth = 12;
constexpr KDCoordinate k_symbolWidthWithMargins = k_symbolWidth + 2 * k_margin;

inline KDCoordinate AboveSymbol(const Layout* node, KDFont::Size font) {
  return std::max<KDCoordinate>(
      Baseline(node->child(k_nIndex)),
      Baseline(node->child(k_kIndex)) - k_symbolHeight);
}

inline KDCoordinate TotalHeight(const Layout* node, KDFont::Size font) {
  KDCoordinate underSymbol = std::max<KDCoordinate>(
      Height(node->child(k_kIndex)) - Baseline(node->child(k_kIndex)),
      Height(node->child(k_nIndex)) - Baseline(node->child(k_nIndex)) -
          k_symbolHeight);
  return AboveSymbol(node, font) + k_symbolHeight + underSymbol;
}
}  // namespace PtCombinatorics

namespace PtBinomial {
constexpr KDCoordinate k_barHeight = 6;
}

namespace PtPermute {
using PtCombinatorics::k_symbolHeight, PtCombinatorics::k_symbolWidth;
}

constexpr KDCoordinate k_gridEntryMargin = 6;

namespace TwoRows {
constexpr static KDCoordinate k_point2DRowsSeparator = 2;

inline KDCoordinate RowsSeparator(const Layout* node, KDFont::Size font) {
  return node->isBinomialLayout() ? k_gridEntryMargin : k_point2DRowsSeparator;
}

inline KDCoordinate RowsHeight(const Layout* node, KDFont::Size font) {
  return Height(node->child(k_upperIndex)) + RowsSeparator(node, font) +
         Height(node->child(k_lowerIndex));
}

inline KDCoordinate RowsWidth(const Layout* node, KDFont::Size font) {
  return std::max(Width(node->child(k_upperIndex)),
                  Width(node->child(k_lowerIndex)));
}

inline KDCoordinate UpperMargin(const Layout* node, KDFont::Size font) {
  return node->isPoint2DLayout()
             ? Parenthesis::VerticalMargin(Height(node->child(k_upperIndex)))
             : 0;
}

inline KDCoordinate LowerMargin(const Layout* node, KDFont::Size font) {
  return node->isPoint2DLayout()
             ? Parenthesis::VerticalMargin(Height(node->child(k_lowerIndex)))
             : 0;
}
}  // namespace TwoRows

namespace ListSequence {
constexpr KDCoordinate k_variableHorizontalMargin = 1;
constexpr KDCoordinate k_variableBaselineOffset = 2;

inline KDCoordinate VariableSlotBaseline(const Layout* node,
                                         KDFont::Size font) {
  return std::max(
      {KDCoordinate(CurlyBrace::Height(Height(node->child(k_functionIndex))) +
                    k_variableBaselineOffset),
       Baseline(node->child(k_upperBoundIndex)),
       Baseline(node->child(k_variableIndex))});
}

inline KDCoordinate BracesWidth(const Layout* node, KDFont::Size font) {
  return 2 * CurlyBrace::k_curlyBraceWidth +
         Width(node->child(k_functionIndex));
}

inline KDPoint PositionOfVariable(const Layout* node, KDFont::Size font) {
  return KDPoint(k_variableHorizontalMargin + BracesWidth(node, font),
                 VariableSlotBaseline(node, font) -
                     Baseline(node->child(k_variableIndex)));
}

}  // namespace ListSequence
}  // namespace PoincareJ

#endif
