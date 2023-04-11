#ifndef CALCULATION_JUNIOR_LAYOUT_SELECTION_H
#define CALCULATION_JUNIOR_LAYOUT_SELECTION_H

#include <poincare_junior/include/layout.h>

namespace CalculationJunior {

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
  LayoutSelection(const PoincareJ::Node n, int startPosition, int endPosition)
      : m_node(n), m_startPosition(startPosition), m_endPosition(endPosition) {
    assert(n.isUninitialized() ||
           (n.isHorizontal() && 0 <= startPosition &&
            startPosition <= n.numberOfChildren() && 0 <= endPosition &&
            endPosition <= n.numberOfChildren()) ||
           (startPosition >= 0 && startPosition <= 1 && endPosition >= 0 &&
            endPosition <= 1));
  }

  LayoutSelection() : LayoutSelection(PoincareJ::Node(), 0, 0) {}

#if 0
  LayoutSelection clone() {
    return LayoutSelection(m_node.clone(), m_startPosition, m_endPosition);
  }
#endif

  bool isEmpty() const {
    return m_node.isUninitialized() || m_startPosition == m_endPosition;
  }

  PoincareJ::Node layout() const { return m_node; }
  /* startPosition can be higher than endPosition if the selection is from
   * right to left. */
  int startPosition() const { return m_startPosition; }
  int endPosition() const { return m_endPosition; }
  int leftPosition() const { return std::min(m_startPosition, m_endPosition); }
  int rightPosition() const { return std::max(m_startPosition, m_endPosition); }

  bool containsNode(const PoincareJ::Node n) const {
    const PoincareJ::TypeBlock* b = n.block();
    return !isEmpty() &&
           (m_node.isHorizontal()
                ? (b >= m_node.childAtIndex(leftPosition()).block() &&
                   b <= m_node.childAtIndex(rightPosition() - 1).block())
                : (b >= m_node.block() && b < m_node.nextTree().block()));
  }

 private:
  const PoincareJ::Node m_node;
  int m_startPosition;
  int m_endPosition;
};

}  // namespace CalculationJunior

#endif
