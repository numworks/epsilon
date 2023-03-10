#ifndef POINCARE_JUNIOR_RACK_LAYOUT_H
#define POINCARE_JUNIOR_RACK_LAYOUT_H

#include "render.h"
#include "../memory/edition_reference.h"

namespace PoincareJ {

class RackLayout {
public:
  static KDSize Size(const Node node, KDFont::Size font);
  static KDCoordinate Baseline(const Node node, KDFont::Size font);
  static KDPoint PositionOfChild(const Node node, int childIndex, KDFont::Size font);
  static EditionReference Parse(const Node node);
  static KDSize SizeBetweenIndexes(const Node node, int leftPosition, int rightPosition, KDFont::Size font);
  static KDCoordinate BaselineBetweenIndexes(const Node node, int leftPosition, int rightPosition, KDFont::Size font);
  static bool ShouldDrawEmptyRectangle(const Node node);
  static void RenderNode(const Node node, KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
};

}

#endif
