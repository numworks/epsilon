#include "layout_selection.h"

#include <poincare_junior/src/n_ary.h>

#include "k_tree.h"

namespace PoincareJ {

Tree* LayoutSelection::cloneSelection() const {
  if (!m_node) {
    return KRackL()->clone();
  }
  Tree* copy = KRackL()->clone();
  for (int i = leftPosition(); i < rightPosition(); i++) {
    NAry::AddChild(copy, m_node->child(i)->clone());
  }
  return copy;
}

}  // namespace PoincareJ
