#ifndef POINCARE_JUNIOR_VERTICAL_OFFSET_LAYOUT_H
#define POINCARE_JUNIOR_VERTICAL_OFFSET_LAYOUT_H

#include "render.h"
#include "../memory/edition_reference.h"

namespace PoincareJ {

/* This is a simplified VerticalOffsetLayout, only suffix superscript and
 * ignoring previous layouts. */
class VerticalOffsetLayout {
public:
  static KDSize Size(const Node node, KDFont::Size font);
  static KDCoordinate Baseline(const Node node, KDFont::Size font);
  static KDPoint PositionOfChild(const Node node, int childIndex, KDFont::Size font);
  static EditionReference Parse(const Node node);
};

}

#endif
