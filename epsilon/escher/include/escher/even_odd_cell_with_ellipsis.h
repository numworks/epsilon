#pragma once

#include <assert.h>
#include <escher/ellipsis_view.h>
#include <escher/even_odd_cell.h>

namespace Escher {

class EvenOddCellWithEllipsis : public EvenOddCell {
 public:
  EvenOddCellWithEllipsis();

  void setHighlighted(bool highlight) override {
    EvenOddCell::setHighlighted(highlight);
    m_ellipsisView.setHighlighted(highlight);
  }

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
