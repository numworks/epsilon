#ifndef POINCARE_LAYOUT_SELECTION_H
#define POINCARE_LAYOUT_SELECTION_H

#include <poincare/src/memory/tree.h>

#include <algorithm>

#include "rack.h"

namespace Poincare::Internal {

struct SimpleLayoutCursor {
  const Rack* rack = nullptr;
  int position;
};

class LayoutSelection {
 public:
  /* The layout is a rack and the selection is between the children at
   * startPosition and endPosition - 1.
   * Ex: l = HorizontalLayout("0123456789")
   *     -> LayoutSelection(l, 2, 5) = "234"
   *     -> LayoutSelection(l, 0, 5) = "01234"
   *     -> LayoutSelection(l, 2, 10) = "23456789"
   *
   * */
  LayoutSelection(const Tree* l, int startPosition, int endPosition)
      : m_rackLayout(l),
        m_startPosition(startPosition),
        m_endPosition(endPosition) {
    assert(!l || (l->isRackLayout() && 0 <= startPosition &&
                  startPosition <= l->numberOfChildren() && 0 <= endPosition &&
                  endPosition <= l->numberOfChildren()));
  }

  LayoutSelection() : LayoutSelection(nullptr, 0, 0) {}

  bool isEmpty() const {
    return !m_rackLayout || m_startPosition == m_endPosition;
  }

  const Tree* layout() const { return m_rackLayout; }
  /* startPosition can be higher than endPosition if the selection is from
   * right to left. */
  int startPosition() const { return m_startPosition; }
  int endPosition() const { return m_endPosition; }
  int leftPosition() const { return std::min(m_startPosition, m_endPosition); }
  int rightPosition() const { return std::max(m_startPosition, m_endPosition); }

  Tree* cloneSelection() const;

 private:
  const Tree* m_rackLayout;
  int m_startPosition;
  int m_endPosition;
};

}  // namespace Poincare::Internal

#endif
