#include "rack_layout.h"

#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

#include "empty_rectangle.h"

namespace VerticalOffset {
constexpr static KDCoordinate IndiceHeight = 10;
}

namespace PoincareJ {

KDFont::Size RackLayout::font = KDFont::Size::Large;

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
  KDCoordinate baseBaseline = i > 0 ? Render::Baseline(node->child(i - 1))
                                    : KDFont::GlyphHeight(font) / 2;
  return baseBaseline + Render::Height(childI) - VerticalOffset::IndiceHeight;
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
      KDCoordinate baseHeight = i > 0 ? Render::Height(node->child(i - 1))
                                      : KDFont::GlyphHeight(font);
      childSize =
          childSize + KDSize(0, baseHeight - VerticalOffset::IndiceHeight);
    }
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
    // TODO vertical
    result = std::max(result, ChildBaseline(node, i));
  }
  return result;
}

bool RackLayout::ShouldDrawEmptyRectangle(const Tree* node) {
  // TODO : complete this method
  return node->numberOfChildren() == 0;
}

void RackLayout::RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                            KDColor expressionColor, KDColor backgroundColor) {
  if (ShouldDrawEmptyRectangle(node)) {
    EmptyRectangle::DrawEmptyRectangle(ctx, p, font,
                                       EmptyRectangle::Color::Yellow);
  }
}

}  // namespace PoincareJ
