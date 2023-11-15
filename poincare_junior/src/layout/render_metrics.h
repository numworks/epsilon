#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H

#include <escher/metric.h>
#include <kandinsky/coordinate.h>

#include "indices.h"
#include "render.h"

namespace PoincareJ {

namespace Fraction {
constexpr static KDCoordinate LineMargin = 2;
constexpr static KDCoordinate LineHeight = 1;
constexpr static KDCoordinate HorizontalOverflow =
    Escher::Metric::FractionAndConjugateHorizontalOverflow;
constexpr static KDCoordinate HorizontalMargin =
    Escher::Metric::FractionAndConjugateHorizontalMargin;
}  // namespace Fraction

namespace CodePoint {
constexpr static KDCoordinate MiddleDotWidth = 5;
}

namespace VerticalOffset {
constexpr static KDCoordinate IndiceHeight = 10;
}

namespace Pair {
constexpr static KDCoordinate LineThickness = 1;

constexpr static KDCoordinate MinimalChildHeight =
    Escher::Metric::MinimalBracketAndParenthesisChildHeight;

static bool ChildHeightDictatesHeight(KDCoordinate childHeight) {
  return childHeight >= MinimalChildHeight;
}
static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight,
                                           KDCoordinate verticalMargin) {
  return (ChildHeightDictatesHeight(childHeight) ? childHeight
                                                 : MinimalChildHeight) +
         verticalMargin * 2;
}
static KDCoordinate BaselineGivenChildHeightAndBaseline(
    KDCoordinate childHeight, KDCoordinate childBaseline,
    KDCoordinate verticalMargin) {
  return childBaseline + verticalMargin +
         (ChildHeightDictatesHeight(childHeight)
              ? 0
              : (MinimalChildHeight - childHeight) / 2);
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
using Pair::LineThickness;
constexpr static KDCoordinate InternalWidthMargin = 5;
constexpr static KDCoordinate ExternalWidthMargin = 2;
constexpr static KDCoordinate BracketWidth =
    InternalWidthMargin + LineThickness + ExternalWidthMargin;
constexpr static KDCoordinate VerticalMargin = 1;
constexpr static KDCoordinate DoubleBarMargin = 2;

static KDSize SizeGivenChildSize(KDSize childSize) {
  return KDSize(
      2 * BracketWidth + childSize.width(),
      Pair::HeightGivenChildHeight(childSize.height(), VerticalMargin));
}
static KDPoint ChildOffset() {
  return Pair::ChildOffset(VerticalMargin, BracketWidth);
}

}  // namespace SquareBracketPair

namespace AbsoluteValue {
constexpr static KDCoordinate InnerWidthMargin = 2;
constexpr static KDCoordinate BracketWidth =
    Pair::LineThickness + InnerWidthMargin +
    SquareBracketPair::ExternalWidthMargin;
constexpr static KDCoordinate VerticalMargin = 0;
}  // namespace AbsoluteValue

namespace VectorNorm {
constexpr static KDCoordinate InnerWidthMargin = 2;
constexpr static KDCoordinate BracketWidth =
    2 * Pair::LineThickness + SquareBracketPair::DoubleBarMargin +
    InnerWidthMargin + SquareBracketPair::ExternalWidthMargin;
constexpr static KDCoordinate VerticalMargin = 0;
}  // namespace VectorNorm

namespace CurlyBrace {
using Pair::LineThickness;
constexpr static KDCoordinate CurveHeight = 6;
constexpr static KDCoordinate CurveWidth = 5;
constexpr static KDCoordinate CenterHeight = 3;
constexpr static KDCoordinate CenterWidth = 3;
constexpr static KDCoordinate WidthMargin = 1;
constexpr static KDCoordinate VerticalMargin = 1;
constexpr static KDCoordinate CurlyBraceWidth =
    2 * WidthMargin + CenterWidth + CurveWidth - LineThickness;

static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) {
  return Pair::HeightGivenChildHeight(childHeight, VerticalMargin);
}
static KDCoordinate BaselineGivenChildHeightAndBaseline(
    KDCoordinate childHeight, KDCoordinate childBaseline) {
  return Pair::BaselineGivenChildHeightAndBaseline(childHeight, childBaseline,
                                                   VerticalMargin);
}
static KDPoint PositionGivenChildHeightAndBaseline(bool left, KDSize childSize,
                                                   KDCoordinate childBaseline) {
  return Pair::PositionGivenChildHeightAndBaseline(
      left, CurlyBraceWidth, childSize, childBaseline, VerticalMargin);
}
}  // namespace CurlyBrace

namespace Parenthesis {
constexpr static KDCoordinate WidthMargin = 1;
constexpr static KDCoordinate CurveWidth = 5;
constexpr static KDCoordinate CurveHeight = 7;
constexpr static KDCoordinate VerticalMargin = 2;
constexpr static KDCoordinate ParenthesisWidth = 2 * WidthMargin + CurveWidth;

static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) {
  return Pair::HeightGivenChildHeight(childHeight, VerticalMargin);
}
static KDCoordinate BaselineGivenChildHeightAndBaseline(
    KDCoordinate childHeight, KDCoordinate childBaseline) {
  return Pair::BaselineGivenChildHeightAndBaseline(childHeight, childBaseline,
                                                   VerticalMargin);
}
static KDPoint PositionGivenChildHeightAndBaseline(bool left, KDSize childSize,
                                                   KDCoordinate childBaseline) {
  return Pair::PositionGivenChildHeightAndBaseline(
      left, ParenthesisWidth, childSize, childBaseline, VerticalMargin);
}
}  // namespace Parenthesis

namespace Pair {
static KDCoordinate BracketWidth(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Ceiling:
    case LayoutType::Floor:
      return SquareBracketPair::BracketWidth;
    case LayoutType::AbsoluteValue:
      return AbsoluteValue::BracketWidth;
    case LayoutType::VectorNorm:
      return VectorNorm::BracketWidth;
    case LayoutType::CurlyBrace:
      return CurlyBrace::CurlyBraceWidth;
    case LayoutType::Parenthesis:
      return Parenthesis::ParenthesisWidth;
    default:
      assert(false);
  }
}

static KDCoordinate VerticalMargin(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Ceiling:
    case LayoutType::Floor:
      return SquareBracketPair::VerticalMargin;
    case LayoutType::AbsoluteValue:
      return AbsoluteValue::VerticalMargin;
    case LayoutType::VectorNorm:
      return VectorNorm::VerticalMargin;
    case LayoutType::CurlyBrace:
      return CurlyBrace::VerticalMargin;
    case LayoutType::Parenthesis:
      return Parenthesis::VerticalMargin;
    default:
      assert(false);
  }
}

}  // namespace Pair

namespace Conjugate {
constexpr static KDCoordinate OverlineWidth = 1;
constexpr static KDCoordinate OverlineVerticalMargin = 1;
}  // namespace Conjugate

namespace NthRoot {
constexpr static KDCoordinate HeightMargin = 2;
constexpr static KDCoordinate WidthMargin = 2;
constexpr static KDCoordinate RadixLineThickness = 1;

constexpr static KDCoordinate LeftRadixHeight = 9;
constexpr static KDCoordinate LeftRadixWidth = 5;

KDSize AdjustedIndexSize(const Tree* node, KDFont::Size font) {
  return node->isSquareRootLayout()
             ? KDSize(LeftRadixWidth, 0)
             : KDSize(std::max(LeftRadixWidth, Render::Width(node->child(1))),
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
constexpr static const char* EqualSign = "=";
constexpr static KDCoordinate LineThickness = 1;

KDCoordinate subscriptBaseline(const Tree* node, KDFont::Size font) {
  return std::max<KDCoordinate>(
      std::max(Render::Baseline(node->child(VariableIndex)),
               Render::Baseline(node->child(LowerBoundIndex))),
      KDFont::Font(font)->stringSize(EqualSign).height() / 2);
}

KDSize lowerBoundSizeWithVariableEquals(const Tree* node, KDFont::Size font) {
  KDSize variableSize = Render::Size(node->child(VariableIndex));
  KDSize lowerBoundSize = Render::Size(node->child(LowerBoundIndex));
  KDSize equalSize = KDFont::Font(font)->stringSize(EqualSign);
  return KDSize(
      variableSize.width() + equalSize.width() + lowerBoundSize.width(),
      subscriptBaseline(node, font) +
          std::max({variableSize.height() -
                        Render::Baseline(node->child(VariableIndex)),
                    lowerBoundSize.height() -
                        Render::Baseline(node->child(LowerBoundIndex)),
                    equalSize.height() / 2}));
}

KDCoordinate completeLowerBoundX(const Tree* node, KDFont::Size font) {
  KDSize upperBoundSize = Render::Size(node->child(UpperBoundIndex));
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
constexpr static int SignificantPixelWidth = 4;
static_assert(Parametric::SymbolHeight(KDFont::Size::Large) % 2 != 0 &&
                  Parametric::SymbolHeight(KDFont::Size::Small) % 2 != 0,
              "sum_layout : SymbolHeight is even");
}  // namespace Sum

namespace Derivative {
constexpr static KDCoordinate DxHorizontalMargin = 2;
constexpr static KDCoordinate BarHorizontalMargin = 2;
constexpr static KDCoordinate BarWidth = 1;

constexpr static const char* dString = "d";

KDCoordinate orderHeightOffset(const Tree* node, KDFont::Size font) {
  if (node->isDerivativeLayout()) {
    return 0;
  }
  return Render::Height(node->child(OrderIndex)) - VerticalOffset::IndiceHeight;
}
KDCoordinate orderWidth(const Tree* node, KDFont::Size font) {
  if (node->isDerivativeLayout()) {
    return 0;
  }
  return Render::Width(node->child(OrderIndex));
}

KDPoint positionOfDInNumerator(const Tree* node, KDFont::Size font) {
  return KDPoint(
      (Render::Width(node->child(VariableIndex)) + DxHorizontalMargin) / 2 +
          Escher::Metric::FractionAndConjugateHorizontalMargin +
          Escher::Metric::FractionAndConjugateHorizontalOverflow,
      Render::Baseline(node) -
          KDFont::Font(font)->stringSize(dString).height() -
          Fraction::LineMargin - Fraction::LineHeight);
}

KDPoint positionOfDInDenominator(const Tree* node, KDFont::Size font) {
  return KDPoint(Escher::Metric::FractionAndConjugateHorizontalMargin +
                     Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Render::Baseline(node) + Fraction::LineMargin +
                     orderHeightOffset(node, font) +
                     Render::Height(node->child(VariableIndex)) -
                     KDFont::Font(font)->stringSize(dString).height());
}

KDPoint positionOfVariableInFractionSlot(const Tree* node, KDFont::Size font) {
  KDPoint positionOfD = positionOfDInDenominator(node, font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(dString).width() +
          DxHorizontalMargin,
      positionOfD.y() + KDFont::Font(font)->stringSize(dString).height() -
          Render::Height(node->child(VariableIndex)));
}

KDCoordinate fractionBarWidth(const Tree* node, KDFont::Size font) {
  return 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow +
         KDFont::Font(font)->stringSize(dString).width() + DxHorizontalMargin +
         Render::Width(node->child(VariableIndex)) + orderWidth(node, font);
}

KDCoordinate parenthesesWidth(const Tree* node, KDFont::Size font) {
  return 2 * Parenthesis::ParenthesisWidth +
         Render::Width(node->child(DerivandIndex));
}

KDCoordinate abscissaBaseline(const Tree* node, KDFont::Size font) {
  KDCoordinate variableHeight = Render::Height(node->child(VariableIndex));
  KDCoordinate dfdxBottom = std::max(
      positionOfVariableInFractionSlot(node, font).y() + variableHeight,
      Render::Baseline(node) + Render::Height(node->child(DerivandIndex)) -
          Render::Baseline(node->child(DerivandIndex)));
  return dfdxBottom - variableHeight +
         Render::Baseline(node->child(VariableIndex));
}

KDPoint positionOfVariableInAssignmentSlot(const Tree* node,
                                           KDFont::Size font) {
  return KDPoint(2 * (Escher::Metric::FractionAndConjugateHorizontalMargin +
                      BarHorizontalMargin) +
                     fractionBarWidth(node, font) +
                     parenthesesWidth(node, font) + BarWidth,
                 abscissaBaseline(node, font) -
                     Render::Baseline(node->child(VariableIndex)));
}

KDCoordinate parenthesisBaseline(const Tree* node, KDFont::Size font) {
  return Parenthesis::BaselineGivenChildHeightAndBaseline(
      Render::Height(node->child(DerivandIndex)),
      Render::Baseline(node->child(DerivandIndex)));
}

KDPoint positionOfLeftParenthesis(const Tree* node, KDFont::Size font) {
  return KDPoint(positionOfVariableInFractionSlot(node, font).x() +
                     Render::Width(node->child(VariableIndex)) +
                     orderWidth(node, font) +
                     Escher::Metric::FractionAndConjugateHorizontalMargin +
                     Escher::Metric::FractionAndConjugateHorizontalOverflow,
                 Render::Baseline(node) - parenthesisBaseline(node, font));
}

KDPoint positionOfRightParenthesis(const Tree* node, KDFont::Size font,
                                   KDSize derivandSize) {
  return positionOfLeftParenthesis(node, font)
      .translatedBy(
          KDPoint(Parenthesis::ParenthesisWidth + derivandSize.width(), 0));
}

KDPoint positionOfOrderInNumerator(const Tree* node, KDFont::Size font) {
  KDPoint positionOfD = positionOfDInNumerator(node, font);
  return KDPoint(
      positionOfD.x() + KDFont::Font(font)->stringSize(dString).width(),
      positionOfD.y() - orderHeightOffset(node, font));
}

KDPoint positionOfOrderInDenominator(const Tree* node, KDFont::Size font) {
  KDPoint positionOfX = positionOfVariableInFractionSlot(node, font);
  return KDPoint(positionOfX.x() + Render::Width(node->child(VariableIndex)),
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
constexpr static KDCoordinate SymbolHeight = 9;
constexpr static KDCoordinate SymbolWidth = 4;
constexpr static KDCoordinate BoundVerticalMargin = 4;
constexpr static KDCoordinate BoundHorizontalMargin = 3;
constexpr static KDCoordinate DifferentialHorizontalMargin = 3;
constexpr static KDCoordinate IntegrandHorizontalMargin = 2;
constexpr static KDCoordinate IntegrandVerticalMargin = 3;
constexpr static KDCoordinate LineThickness = 1;

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
                                        ? LowerBoundIndex
                                        : UpperBoundIndex));
}

KDCoordinate centralArgumentHeight(const Tree* node, KDFont::Size font) {
  /* When integrals are in a row, the last one is the tallest. We take its
   * central argument height to define the one of the others integrals */
  const Tree* last = mostNestedIntegral(node, NestedPosition::Next);
  if (node == last) {
    KDCoordinate integrandHeight = Render::Height(node->child(IntegrandIndex));
    KDCoordinate integrandBaseline =
        Render::Baseline(node->child(IntegrandIndex));
    KDCoordinate differentialHeight =
        Render::Height(node->child(DifferentialIndex));
    KDCoordinate differentialBaseline =
        Render::Baseline(node->child(DifferentialIndex));
    return std::max(integrandBaseline, differentialBaseline) +
           std::max(integrandHeight - integrandBaseline,
                    differentialHeight - differentialBaseline);
  } else {
    return centralArgumentHeight(last, font);
  }
}

}  // namespace Integral

namespace PtCombinatorics {
constexpr static KDCoordinate Margin = 3;
constexpr static KDCoordinate SymbolHeight = 16;
constexpr static KDCoordinate SymbolBaseline = 11;
constexpr static KDCoordinate SymbolWidth = 12;
constexpr static KDCoordinate SymbolWidthWithMargins = SymbolWidth + 2 * Margin;

KDCoordinate AboveSymbol(const Tree* node, KDFont::Size font) {
  return std::max<KDCoordinate>(
      Render::Baseline(node->child(nIndex)),
      Render::Baseline(node->child(kIndex)) - SymbolHeight);
}

KDCoordinate TotalHeight(const Tree* node, KDFont::Size font) {
  KDCoordinate underSymbol = std::max<KDCoordinate>(
      Render::Height(node->child(kIndex)) -
          Render::Baseline(node->child(kIndex)),
      Render::Height(node->child(nIndex)) -
          Render::Baseline(node->child(nIndex)) - SymbolHeight);
  return AboveSymbol(node, font) + SymbolHeight + underSymbol;
}
}  // namespace PtCombinatorics

namespace PtBinomial {
constexpr static KDCoordinate BarHeight = 6;
}

namespace PtPermute {
using PtCombinatorics::SymbolHeight, PtCombinatorics::SymbolWidth;
}

namespace Grid {
constexpr static KDCoordinate EntryMargin = 6;

KDCoordinate horizontalGridEntryMargin(const Tree* node, KDFont::Size font) {
  return node->isPiecewiseLayout() ? 2 * EntryMargin + KDFont::GlyphWidth(font)
                                   : EntryMargin;
}
KDCoordinate verticalGridEntryMargin(const Tree* node, KDFont::Size font) {
  return EntryMargin;
}

KDCoordinate rowBaseline(const Tree* node, int row, KDFont::Size font) {
  KDCoordinate rowBaseline = 0;
  int column = 0;
  const Tree* l = node->child(row * NumberOfColumns(node));
  while (column < NumberOfColumns(node)) {
    rowBaseline = std::max(rowBaseline, Render::Baseline(l));
    column++;
    l = l->nextTree();
  }
  return rowBaseline;
}

KDCoordinate rowHeight(const Tree* node, int row, KDFont::Size font) {
  KDCoordinate underBaseline = 0;
  KDCoordinate aboveBaseline = 0;
  int column = 0;
  const Tree* l = node->child(row * NumberOfColumns(node));
  while (column < NumberOfColumns(node)) {
    KDCoordinate b = Render::Baseline(l);
    underBaseline =
        std::max<KDCoordinate>(underBaseline, Render::Height(l) - b);
    aboveBaseline = std::max(aboveBaseline, b);
    column++;
    l = l->nextTree();
  }
  return aboveBaseline + underBaseline;
}

KDCoordinate height(const Tree* node, KDFont::Size font) {
  KDCoordinate totalHeight = 0;
  for (int row = 0; row < NumberOfRows(node); row++) {
    totalHeight += rowHeight(node, row, font);
  }
  totalHeight +=
      NumberOfRows(node) > 0
          ? (NumberOfRows(node) - 1) * verticalGridEntryMargin(node, font)
          : 0;
  return totalHeight;
}

KDCoordinate columnWidth(const Tree* node, int column, KDFont::Size font) {
  KDCoordinate columnWidth = 0;
  int childIndex = 0;
  int lastIndex = (NumberOfRows(node) - 1) * NumberOfColumns(node) + column;
  for (const Tree* l : node->children()) {
    if (childIndex % NumberOfColumns(node) == column) {
      columnWidth = std::max(columnWidth, Render::Width(l));
      if (childIndex >= lastIndex) {
        break;
      }
    }
    childIndex++;
  }
  return columnWidth;
}

KDCoordinate width(const Tree* node, KDFont::Size font) {
  KDCoordinate totalWidth = 0;
  for (int j = 0; j < NumberOfColumns(node); j++) {
    totalWidth += columnWidth(node, j, font);
  }
  totalWidth +=
      NumberOfColumns(node) > 0
          ? (NumberOfColumns(node) - 1) * horizontalGridEntryMargin(node, font)
          : 0;
  return totalWidth;
}

KDSize size(const Tree* node, KDFont::Size font) {
  return KDSize(width(node, font), height(node, font));
}

}  // namespace Grid

namespace Binomial {
static KDCoordinate KNHeight(const Tree* node, KDFont::Size font) {
  return Render::Height(node->child(nIndex)) + Grid::EntryMargin +
         Render::Height(node->child(kIndex));
}
}  // namespace Binomial

namespace ListSequence {
constexpr static KDCoordinate VariableHorizontalMargin = 1;
constexpr static KDCoordinate VariableBaselineOffset = 2;

KDCoordinate variableSlotBaseline(const Tree* node, KDFont::Size font) {
  return std::max(
      {KDCoordinate(CurlyBrace::HeightGivenChildHeight(
                        Render::Height(node->child(FunctionIndex))) +
                    VariableBaselineOffset),
       Render::Baseline(node->child(UpperBoundIndex)),
       Render::Baseline(node->child(VariableIndex))});
}

KDCoordinate bracesWidth(const Tree* node, KDFont::Size font) {
  return 2 * CurlyBrace::CurlyBraceWidth +
         Render::Width(node->child(FunctionIndex));
}

KDPoint positionOfVariable(const Tree* node, KDFont::Size font) {
  return KDPoint(VariableHorizontalMargin + bracesWidth(node, font),
                 variableSlotBaseline(node, font) -
                     Render::Baseline(node->child(VariableIndex)));
}

}  // namespace ListSequence

}  // namespace PoincareJ

#endif
