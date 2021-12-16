#include <poincare/brace_layout.h>

namespace Poincare {

bool BraceLayoutNode::isCollapsable(int * numberOfOpenBraces, bool goingLeft) const {
  if (goingLeft == (type() == rightLayoutType())) {
    /* This parenthesis is an opening parenthesis. */
    *numberOfOpenBraces = *numberOfOpenBraces + 1;
    return true;
  }

  /* This parenthesis is a closing parenthesis. We do not want to absorb it if
   * there is no corresponding opening parenthesis, as the absorber should be
   * enclosed by this parenthesis. */
  assert((goingLeft && type() == leftLayoutType()) || (!goingLeft && type() == rightLayoutType()));
  if (*numberOfOpenBraces == 0) {
    return false;
  }
  *numberOfOpenBraces = *numberOfOpenBraces - 1;
  return true;
}

}
