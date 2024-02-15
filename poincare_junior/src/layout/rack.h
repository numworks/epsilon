#ifndef POINCARE_JUNIOR_LAYOUT_RACK_H
#define POINCARE_JUNIOR_LAYOUT_RACK_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class LayoutCursor;
class Rack;

// Will be called Layout as soon as the older Layout is hidden
class LayoutT : public Tree {
 public:
  static const LayoutT* From(const Tree* node) {
    assert(node->isLayout() && !node->isRackLayout());
    return static_cast<const LayoutT*>(node);
  }

  static LayoutT* From(Tree* node) {
    assert(node->isLayout() && !node->isRackLayout());
    return static_cast<LayoutT*>(node);
  }

  const Rack* child(int index) const;
  Rack* child(int index);
  const LayoutT* nextTree() const { return LayoutT::From(Tree::nextTree()); }
  LayoutT* nextTree() { return LayoutT::From(Tree::nextTree()); }

  // TODO cast children()
};

class Rack : public Tree {
 public:
  static const Rack* From(const Tree* node) {
    assert(node->isRackLayout());
    return static_cast<const Rack*>(node);
  }

  static Rack* From(Tree* node) {
    assert(node->isRackLayout());
    return static_cast<Rack*>(node);
  }

  const LayoutT* child(int index) const;
  LayoutT* child(int index);

  const Rack* nextTree() const { return Rack::From(Tree::nextTree()); }
  Rack* nextTree() { return Rack::From(Tree::nextTree()); }
};

inline const LayoutT* Rack::child(int index) const {
  return static_cast<const LayoutT*>(Tree::child(index));
}

inline LayoutT* Rack::child(int index) {
  return static_cast<LayoutT*>(Tree::child(index));
}

inline const Rack* LayoutT::child(int index) const {
  return static_cast<const Rack*>(Tree::child(index));
}

inline Rack* LayoutT::child(int index) {
  return static_cast<Rack*>(Tree::child(index));
}
}  // namespace PoincareJ

#endif
