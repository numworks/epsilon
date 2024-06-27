#include "layout_selection.h"

#include <poincare/src/memory/n_ary.h>

#include "k_tree.h"

namespace Poincare::Internal {

Tree* LayoutSelection::cloneSelection() const {
  if (!m_layout) {
    return KRackL()->cloneTree();
  }
  return NAry::CloneSubRange(m_layout, leftPosition(), rightPosition());
}

}  // namespace Poincare::Internal
