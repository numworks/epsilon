#ifndef POINCARE_JUNIOR_LAYOUT_RACK_H
#define POINCARE_JUNIOR_LAYOUT_RACK_H

#include <poincare_junior/src/memory/tree_sub_class.h>

namespace PoincareJ {

class LayoutCursor;

class Rack;

// Will be called Layout as soon as the older Layout is hidden
struct LayoutT : TreeSubClass<LayoutT, Rack> {
  static void Check(const Tree* node) {
    assert(node->isLayout() && !node->isRackLayout());
  }
};

class Rack : public TreeSubClass<Rack, LayoutT> {
 public:
  static void Check(const Tree* node) { assert(node->isRackLayout()); }
};

}  // namespace PoincareJ

#endif
