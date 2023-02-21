#ifndef ESCHER_EVEN_ODD_CELL_WITH_ELLIPSIS_H
#define ESCHER_EVEN_ODD_CELL_WITH_ELLIPSIS_H

#include <assert.h>
#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>

namespace Escher {

class EvenOddCellWithEllipsis : public EvenOddCell {
 public:
  EvenOddCellWithEllipsis();

 private:
  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_ellipsisView;
  }
  void layoutSubviews(bool force = false) override;
  EllipsisView m_ellipsisView;
};

}  // namespace Escher

#endif
