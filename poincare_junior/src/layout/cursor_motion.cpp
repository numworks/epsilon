#include "cursor_motion.h"

#include <poincare_junior/include/layout.h>

namespace PoincareJ {

int CursorMotion::IndexAfterHorizontalCursorMove(
    const Tree* node, OMG::HorizontalDirection direction, int currentIndex,
    bool* shouldRedraw) {
  int nChildren = node->numberOfChildren();
  if (nChildren == 0) {
    assert(currentIndex == k_outsideIndex);
    return k_outsideIndex;
  }
  if (Layout::IsHorizontal(node)) {
    nChildren += 1;
  }
  if (nChildren == 1) {
    assert(currentIndex == k_outsideIndex || currentIndex == 0);
    return currentIndex == k_outsideIndex ? 0 : k_outsideIndex;
  }
#if 0
  assert(false);
  return k_cantMoveIndex;
#else
  // TODO Implement other layout's logic instead of this dummy generalization
  currentIndex += (direction.isRight() ? 1 : -1);
  if (currentIndex == nChildren) {
    return k_outsideIndex;
  }
  if (currentIndex == k_outsideIndex - 1) {
    return nChildren - 1;
  }
  return currentIndex;
#endif
}

}  // namespace PoincareJ
