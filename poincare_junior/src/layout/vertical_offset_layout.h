#ifndef POINCARE_JUNIOR_VERTICAL_OFFSET_LAYOUT_H
#define POINCARE_JUNIOR_VERTICAL_OFFSET_LAYOUT_H

#include "../memory/edition_reference.h"
#include "render.h"

namespace PoincareJ {

class VerticalOffsetLayout {
 public:
  static KDSize Size(const Node* node, const Node* root, KDFont::Size font);
  static KDCoordinate Baseline(const Node* node, const Node* root,
                               KDFont::Size font);
  static KDPoint PositionOfChild(const Node* node, int childIndex,
                                 const Node* root, KDFont::Size font);
  // TODO : Implement prefix and subscript logic
  static bool IsSuffixSuperscript(const Node* node) { return true; }

 private:
  constexpr static KDCoordinate k_indiceHeight = 10;

  static const Node* BaseLayout(const Node* node, const Node* root);
};

}  // namespace PoincareJ

#endif
