#include "layout_selection.h"

#include <poincare_junior/src/n_ary.h>

#include "k_tree.h"

namespace PoincareJ {

Tree* LayoutSelection::cloneSelection() const {
  if (!m_node) {
    return KRackL()->clone();
  }
  return NAry::CloneSubRange(m_node, leftPosition(), rightPosition());
}

}  // namespace PoincareJ
