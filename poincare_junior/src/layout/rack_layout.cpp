#include "rack_layout.h"
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/layout/parsing/parser.h>

namespace PoincareJ {

KDSize RackLayout::Size(const Node node, KDFont::Size font) {
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (auto [child, index] : NodeIterator::Children<Forward, NoEditable>(node)) {
    KDSize childSize = Render::Size(child, font);
    totalWidth += childSize.width();
    KDCoordinate childBaseline = Render::Baseline(child, font);
    maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline, childSize.height() - childBaseline);
    maxAboveBaseline = std::max<KDCoordinate>(maxAboveBaseline, childBaseline);
  }
  return KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline);
}

KDCoordinate RackLayout::Baseline(const Node node, KDFont::Size font) {
  KDCoordinate result = 0;
  for (auto [child, index] : NodeIterator::Children<Forward, NoEditable>(node)) {
    result = std::max<KDCoordinate>(result, Render::Baseline(child, font));
  }
  return result;
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
  return Parser(node).parse();
}

}
