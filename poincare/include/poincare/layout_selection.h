#ifndef POINCARE_LAYOUT_SELECTION_H
#define POINCARE_LAYOUT_SELECTION_H

#include <poincare/layout.h>

namespace Poincare {

class LayoutSelection {
public:
  /* If the layout is horizontal, the selection is between the children at
   * startPosition and endPosition - 1.
   * Ex: l = HorizontalLayout("0123456789")
   *     -> LayoutSelection(l, 2, 5) = "234"
   *     -> LayoutSelection(l, 0, 5) = "01234"
   *     -> LayoutSelection(l, 2, 10) = "23456789"
   *
   * If the layout is not horizontal, the selection is either empty or selects
   * only this layout. The start and endPosition should only be 0 or 1.
   * Ex: l = CodePoint("A")
   *     -> LayoutSelection(l, 0, 1) = "A"
   *     -> LayoutSelection(l, 0, 0) = ""
   * */
  LayoutSelection(Layout l, int startPosition, int endPosition) :
    m_layout(l), m_startPosition(startPosition), m_endPosition(endPosition)
  {
    assert(l.isUninitialized() ||
           (l.isHorizontal() && 0 <= startPosition && startPosition <= l.numberOfChildren() && 0 <= endPosition && endPosition <= l.numberOfChildren()) ||
           (startPosition >= 0 && startPosition <= 1 && endPosition >= 0 && endPosition <= 1));
  }

  LayoutSelection() : LayoutSelection(Layout(), 0, 0) {}

  bool isEmpty() const { return m_layout.isUninitialized() || m_startPosition == m_endPosition; }

  Layout layout() const { return m_layout; }
  /* startPosition can be higher than endPosition if the selection is from
   * right to left. */
  int startPosition() const { return m_startPosition; }
  int endPosition() const { return m_endPosition; }
  int leftPosition() const { return std::min(m_startPosition, m_endPosition); }
  int rightPosition() const { return std::max(m_startPosition, m_endPosition); }

  bool containsNode(TreeNode * n) const {
    char * c = reinterpret_cast<char *>(n);
    return
      !isEmpty() &&
      ((m_layout.isHorizontal() &&
        c >= reinterpret_cast<char *>(m_layout.childAtIndex(leftPosition()).node()) &&
        c <= reinterpret_cast<char *>(m_layout.childAtIndex(rightPosition() - 1).node())) ||
       (c >= reinterpret_cast<char *>(m_layout.node()) &&
        c < reinterpret_cast<char *>(m_layout.node()->nextSibling())));
  }

private:
  Layout m_layout;
  int m_startPosition;
  int m_endPosition;
};

}

#endif