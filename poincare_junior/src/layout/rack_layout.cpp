#include "rack_layout.h"
#include "empty_rectangle.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

namespace PoincareJ {

KDSize RackLayout::Size(const Node node, KDFont::Size font) {
  return SizeBetweenIndexes(node, 0, node.numberOfChildren(), font);
}

KDCoordinate RackLayout::Baseline(const Node node, KDFont::Size font) {
  return BaselineBetweenIndexes(node, 0, node.numberOfChildren(), font);
}

KDPoint RackLayout::PositionOfChild(const Node node, int childIndex, KDFont::Size font) {
  KDCoordinate x = 0;
  KDCoordinate childBaseline = 0;
  for (auto [child, index] : NodeIterator::Children<Forward, NoEditable>(node)) {
    if (index == childIndex) {
      childBaseline = Render::Baseline(child, font);
      break;
    }
    KDSize childSize = Render::Size(child, font);
    x += childSize.width();
  }
  KDCoordinate y = Render::Baseline(node, font) - childBaseline;
  return KDPoint(x, y);
}

EditionReference RackLayout::Parse(const Node node) {
  return RackParser(node).parse();
}

KDSize RackLayout::SizeBetweenIndexes(const Node node, int leftIndex, int rightIndex, KDFont::Size font) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node.numberOfChildren());
  if (node.numberOfChildren() == 0) {
    KDSize emptyRectangleSize = EmptyRectangle::RectangleSize(font);
    KDCoordinate width =
        ShouldDrawEmptyRectangle(node) ? emptyRectangleSize.width() : 0;
    return KDSize(width, emptyRectangleSize.height());
  }
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    const Node childi = node.childAtIndex(i);
    KDSize childSize = Render::Size(childi, font);
    totalWidth += childSize.width();
    KDCoordinate childBaseline = Render::Baseline(childi, font);
    maxUnderBaseline = std::max<KDCoordinate>(
        maxUnderBaseline, childSize.height() - childBaseline);
    maxAboveBaseline = std::max(maxAboveBaseline, childBaseline);
  }
  return KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline);
}

KDCoordinate RackLayout::BaselineBetweenIndexes(const Node node, int leftIndex, int rightIndex, KDFont::Size font) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node.numberOfChildren());
  if (node.numberOfChildren() == 0) {
    return EmptyRectangle::RectangleBaseLine(font);
  }
  KDCoordinate result = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    result = std::max(result, Render::Baseline(node.childAtIndex(i), font));
  }
  return result;
}

bool RackLayout::ShouldDrawEmptyRectangle(const Node node) {
  // TODO : complete this method
  return node.numberOfChildren() == 0;
}

void RackLayout::RenderNode(const Node node, KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  if (ShouldDrawEmptyRectangle(node)) {
    EmptyRectangle::DrawEmptyRectangle(ctx, p, font, EmptyRectangle::Color::Yellow);
  }
}

}
