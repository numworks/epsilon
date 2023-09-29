#include "vertical_offset_layout.h"

namespace PoincareJ {

const Tree* VerticalOffsetLayout::BaseLayout(const Tree* node,
                                             const Tree* root) {
  int index;
  const Tree* parent = root->parentOfDescendant(node, &index);
  if (parent->type() != BlockType::RackLayout) {
    return nullptr;
  }
  assert(IsSuffixSuperscript(node));
  if (index == 0) {
    return nullptr;
  }
  return parent->child(index - 1);
}

}  // namespace PoincareJ
