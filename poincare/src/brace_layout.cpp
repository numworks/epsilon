#include <poincare/brace_layout.h>

namespace Poincare {

bool BraceLayoutNode::isCollapsable(int * numberOfOpenBraces, bool goingLeft) const {
  if (goingLeft == IsRightBracket(type())) {
    /* This brace is an opening brace. */
    *numberOfOpenBraces = *numberOfOpenBraces + 1;
    return true;
  }

  /* This brace is a closing brace. We do not want to absorb it if
   * there is no corresponding opening brace, as the absorber should be
   * enclosed by this brace. */
  assert((goingLeft && IsLeftBracket(type())) || (!goingLeft && IsRightBracket(type())));
  if (*numberOfOpenBraces == 0) {
    return false;
  }
  *numberOfOpenBraces = *numberOfOpenBraces - 1;
  return true;
}

}
