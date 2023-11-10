#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_METRICS_H

#include <escher/metric.h>
#include <kandinsky/coordinate.h>

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

namespace Grid {
constexpr static KDCoordinate EntryMargin = 6;
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

namespace Parenthesis {
constexpr static KDCoordinate WidthMargin = 1;
constexpr static KDCoordinate CurveWidth = 5;
constexpr static KDCoordinate CurveHeight = 7;
constexpr static KDCoordinate VerticalMargin = 2;
constexpr static KDCoordinate Width = 2 * WidthMargin + CurveWidth;

constexpr static KDCoordinate VerticalPadding = 2;

constexpr static KDCoordinate HorizontalPadding(KDFont::Size font) {
  return KDFont::GlyphSize(font).width();
}
}  // namespace Parenthesis

namespace Binomial {
static KDCoordinate KNHeight(const Tree* node, KDFont::Size font) {
  return Render::Height(node->child(0)) + Grid::EntryMargin +
         Render::Height(node->child(1));
}
}  // namespace Binomial

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

namespace Integral {
constexpr static int DifferentialIndex = 0;
constexpr static int LowerBoundIndex = 1;
constexpr static int UpperBoundIndex = 2;
constexpr static int IntegrandIndex = 3;
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

}  // namespace PoincareJ

#endif
