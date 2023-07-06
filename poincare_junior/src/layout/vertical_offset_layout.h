#ifndef POINCARE_JUNIOR_VERTICAL_OFFSET_LAYOUT_H
#define POINCARE_JUNIOR_VERTICAL_OFFSET_LAYOUT_H

#include "../memory/edition_reference.h"
#include "render.h"

namespace PoincareJ {

class VerticalOffsetLayout {
 public:
  static KDSize Size(const Tree* node, const Tree* root, KDFont::Size font);
  static KDCoordinate Baseline(const Tree* node, const Tree* root,
                               KDFont::Size font);
  static KDPoint PositionOfChild(const Tree* node, int childIndex,
                                 const Tree* root, KDFont::Size font);
  // TODO : Implement prefix and subscript logic
  static bool IsSuffixSuperscript(const Tree* node) { return true; }

 private:
  constexpr static KDCoordinate k_indiceHeight = 10;

  static const Tree* BaseLayout(const Tree* node, const Tree* root);
};

}  // namespace PoincareJ

#endif
