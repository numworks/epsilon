#include <poincare/layout.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

bool CodePointLayoutNode::IsCodePoint(OLayout l, CodePoint c) {
  return l.otype() == Type::CodePointLayout &&
         static_cast<CodePointLayout &>(l).codePoint() == c;
}

bool CodePointLayoutNode::isCollapsable(
    int *numberOfOpenParenthesis, OMG::HorizontalDirection direction) const {
  if (*numberOfOpenParenthesis <= 0) {
    if (m_codePoint == '+' || m_codePoint == UCodePointRightwardsArrow ||
        m_codePoint.isEquationOperator() || m_codePoint == ',') {
      return false;
    }
    if (m_codePoint == '-') {
      /* If the expression is like 3ᴇ-200, we want '-' to be collapsable.
       * Otherwise, '-' is not collapsable. */
      OLayout thisRef = CodePointLayout(this);
      OLayout parent = thisRef.parent();
      if (!parent.isUninitialized()) {
        int indexOfThis = parent.indexOfChild(thisRef);
        if (indexOfThis > 0) {
          OLayout leftBrother = parent.childAtIndex(indexOfThis - 1);
          if (leftBrother.otype() == Type::CodePointLayout &&
              static_cast<CodePointLayout &>(leftBrother).codePoint() ==
                  UCodePointLatinLetterSmallCapitalE) {
            return true;
          }
        }
      }
      return false;
    }
    if (isMultiplicationCodePoint()) {
      /* We want '*' to be collapsable only if the following brother is not a
       * fraction, so that the user can write intuitively "1/2 * 3/4". */
      OLayout thisRef = CodePointLayout(this);
      OLayout parent = thisRef.parent();
      if (!parent.isUninitialized()) {
        int indexOfThis = parent.indexOfChild(thisRef);
        OLayout brother;
        if (indexOfThis > 0 && direction.isLeft()) {
          brother = parent.childAtIndex(indexOfThis - 1);
        } else if (indexOfThis < parent.numberOfChildren() - 1 &&
                   direction.isRight()) {
          brother = parent.childAtIndex(indexOfThis + 1);
        }
        if (!brother.isUninitialized() &&
            brother.otype() == LayoutNode::Type::FractionLayout) {
          return false;
        }
      }
    }
  }
  return true;
}

bool CodePointLayoutNode::isMultiplicationCodePoint() const {
  return m_codePoint == '*' || m_codePoint == UCodePointMultiplicationSign ||
         m_codePoint == UCodePointMiddleDot;
}

}  // namespace Poincare
