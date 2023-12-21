#include "rack_layout.h"

#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

#include "empty_rectangle.h"
#include "indices.h"
#include "layout_cursor.h"
#include "render.h"

namespace PoincareJ {

namespace VerticalOffset {
constexpr static KDCoordinate IndiceHeight = 10;
}  // namespace VerticalOffset

KDFont::Size RackLayout::font = KDFont::Size::Large;
const LayoutCursor* RackLayout::layoutCursor = nullptr;

KDSize RackLayout::Size(const Tree* node) {
  return SizeBetweenIndexes(node, 0, node->numberOfChildren());
}

KDCoordinate RackLayout::Baseline(const Tree* node) {
  return BaselineBetweenIndexes(node, 0, node->numberOfChildren());
}

KDCoordinate RackLayout::ChildBaseline(const Tree* node, int i) {
  const Tree* childI = node->child(i);
  if (!childI->isVerticalOffsetLayout()) {
    return Render::Baseline(childI);
  }
  // TODO prevent infinite loop with prefix of suffix
  int baseIndex = VerticalOffset::IsSuffix(childI) ? i - 1 : i + 1;
  KDCoordinate baseBaseline =
      (baseIndex == -1 || baseIndex == node->numberOfChildren())
          ? KDFont::GlyphHeight(font) / 2
          : ChildBaseline(node, baseIndex);
  if (!VerticalOffset::IsSuperscript(childI)) {
    return baseBaseline;
  }
  return baseBaseline + Render::Height(childI) - VerticalOffset::IndiceHeight;
}

KDCoordinate RackLayout::ChildYPosition(const Tree* node, int i) {
  const Tree* childI = node->child(i);
  if (!childI->isVerticalOffsetLayout() ||
      VerticalOffset::IsSuperscript(childI)) {
    return Baseline(node) - RackLayout::ChildBaseline(node, i);
  }
  int baseIndex = VerticalOffset::IsSuffix(childI) ? i - 1 : i + 1;

  KDCoordinate baseHeight =
      (baseIndex == -1 || baseIndex == node->numberOfChildren())
          ? KDFont::GlyphHeight(font)
          : SizeBetweenIndexes(node, baseIndex, baseIndex + 1).height();
  return Baseline(node) - RackLayout::ChildBaseline(node, i) + baseHeight -
         VerticalOffset::IndiceHeight;
}

KDSize RackLayout::SizeBetweenIndexes(const Tree* node, int leftIndex,
                                      int rightIndex) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  if (node->numberOfChildren() == 0) {
    KDSize emptyRectangleSize = EmptyRectangle::RectangleSize(font);
    KDCoordinate width =
        ShouldDrawEmptyRectangle(node) ? emptyRectangleSize.width() : 0;
    return KDSize(width, emptyRectangleSize.height());
  }
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    const Tree* childI = node->child(i);
    KDSize childSize = Render::Size(childI);
    if (childI->isVerticalOffsetLayout()) {
      int baseIndex = VerticalOffset::IsSuffix(childI) ? i - 1 : i + 1;
      KDCoordinate baseHeight =
          (baseIndex == -1 || baseIndex == node->numberOfChildren())
              ? KDFont::GlyphHeight(font)
              : SizeBetweenIndexes(node, baseIndex, baseIndex + 1).height();
      childSize =
          childSize + KDSize(0, baseHeight - VerticalOffset::IndiceHeight);
    }
    // TODO k_separationMargin
    totalWidth += childSize.width();
    KDCoordinate childBaseline = ChildBaseline(node, i);
    maxUnderBaseline = std::max<KDCoordinate>(
        maxUnderBaseline, childSize.height() - childBaseline);
    maxAboveBaseline = std::max(maxAboveBaseline, childBaseline);
  }
  return KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline);
}

KDCoordinate RackLayout::BaselineBetweenIndexes(const Tree* node, int leftIndex,
                                                int rightIndex) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  if (node->numberOfChildren() == 0) {
    return EmptyRectangle::RectangleBaseLine(font);
  }
  KDCoordinate result = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    result = std::max(result, ChildBaseline(node, i));
  }
  return result;
}

bool RackLayout::ShouldDrawEmptyRectangle(const Tree* node) {
  // TODO: complete this method
  if (!RackLayout::layoutCursor) {
    return false;
  }
  return node->numberOfChildren() == 0 && node != layoutCursor->cursorNode();
}

void RackLayout::RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                            bool isGridPlaceholder) {
  if (ShouldDrawEmptyRectangle(node)) {
    EmptyRectangle::DrawEmptyRectangle(ctx, p, font,
                                       isGridPlaceholder
                                           ? EmptyRectangle::Color::Gray
                                           : EmptyRectangle::Color::Yellow);
  }
}

}  // namespace PoincareJ
