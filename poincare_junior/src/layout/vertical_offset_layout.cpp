#include "vertical_offset_layout.h"
#include "parser.h"

namespace PoincareJ {

KDSize VerticalOffsetLayout::Size(const Node node, KDFont::Size font) {
  return Render::Size(node.childAtIndex(0), font);
}

KDCoordinate VerticalOffsetLayout::Baseline(const Node node, KDFont::Size font) {
  // TODO : Use previous layout for accurate baseline
  return Render::Size(node.childAtIndex(0), font).height();
}

KDPoint VerticalOffsetLayout::PositionOfChild(const Node node, int childIndex, KDFont::Size font) {
  return KDPointZero;
}

}
