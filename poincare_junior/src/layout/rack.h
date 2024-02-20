#ifndef POINCARE_JUNIOR_LAYOUT_RACK_H
#define POINCARE_JUNIOR_LAYOUT_RACK_H

#include <poincare_junior/src/memory/tree_sub_class.h>

namespace PoincareJ {

class LayoutCursor;

class Rack;

struct Layout : TreeSubClass<Layout, Rack> {
  static void Check(const Tree* node) {
    /* It would be safer to check that layouts are actually layouts but it is
     * also practical to be able to assign to a layout the end element of a rack
     * (ie one past last).  We should rather check it when dereferencing it. */
    // assert(node->isLayout() && !node->isRackLayout());
  }
};

class Rack : public TreeSubClass<Rack, Layout> {
 public:
  static void Check(const Tree* node) {
    // assert(node->isRackLayout());
  }

  static bool IsEmpty(const Tree* node) {
    assert(node->isRackLayout());
    return node->numberOfChildren() == 0;
  }
};

}  // namespace PoincareJ

#endif
