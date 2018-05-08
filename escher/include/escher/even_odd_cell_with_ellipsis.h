#ifndef ESCHER_EVEN_ODD_CELL_WITH_ELLIPSIS_H
#define ESCHER_EVEN_ODD_CELL_WITH_ELLIPSIS_H

#include <escher/even_odd_cell.h>
#include <escher/ellipsis_view.h>
#include <assert.h>

class EvenOddCellWithEllipsis : public EvenOddCell {
public:
  EvenOddCellWithEllipsis();
private:
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override {
    assert(index==0);
    return &m_ellipsisView;
  }
  void layoutSubviews() override;
  EllipsisView m_ellipsisView;
};

#endif
