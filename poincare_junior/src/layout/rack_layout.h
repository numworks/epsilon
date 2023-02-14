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
};

}

#endif
