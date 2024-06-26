#ifndef POINCARE_LAYOUT_RACK_H
#define POINCARE_LAYOUT_RACK_H

#include <poincare/src/memory/tree_sub_class.h>

namespace Poincare::Internal {

class LayoutCursor;

class Rack;

struct Layout : TreeSubClass<Layout, Rack> {
  static void Check(const Tree* l) {
    assert(l->isLayout() && !l->isRackLayout());
  }
};

class Rack : public TreeSubClass<Rack, Layout> {
 public:
  static void Check(const Tree* l) { assert(l->isRackLayout()); }

  static bool IsEmpty(const Tree* l) {
    assert(l->isRackLayout());
    return l->numberOfChildren() == 0;
  }
};

}  // namespace Poincare::Internal

#endif
