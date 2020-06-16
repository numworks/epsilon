#include <poincare/parenthesis_layout.h>
#include <poincare/fraction_layout.h>

namespace Poincare {

bool ParenthesisLayoutNode::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (goingLeft == (type() == LayoutNode::Type::RightParenthesisLayout)) {
    /* This parenthesis is an opening parenthesis. */
    *numberOfOpenParenthesis = *numberOfOpenParenthesis + 1;
    return true;
  }

  /* This parenthesis is a closing parenthesis. We do not want to absorb it if
   * there is no corresponding opening parenthesis, as the absorber should be
   * enclosed by this parenthesis. */
  assert((goingLeft && type() == LayoutNode::Type::LeftParenthesisLayout) || (!goingLeft && type() == LayoutNode::Type::RightParenthesisLayout));
  if (*numberOfOpenParenthesis == 0) {
    return false;
  }
  *numberOfOpenParenthesis = *numberOfOpenParenthesis - 1;
  return true;
}

}
