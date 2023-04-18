#ifndef SHARED_SCROLLABLE_TWO_LAYOUTS_CELL_H
#define SHARED_SCROLLABLE_TWO_LAYOUTS_CELL_H

#include "scrollable_multiple_layouts_view.h"

namespace Shared {

class ScrollableTwoLayoutsCell : public Escher::EvenOddCell,
                                 public Escher::Responder {
 public:
  ScrollableTwoLayoutsCell(Escher::Responder* parentResponder = nullptr,
                           float horizontalAlignment = KDGlyph::k_alignLeft,
                           KDFont::Size font = KDFont::Size::Large);
  void setLayouts(Poincare::Layout exactLayout,
                  Poincare::Layout approximateLayout) {
    m_view.setLayouts(Poincare::Layout(), exactLayout, approximateLayout);
  }
  void resetLayouts() { return m_view.resetLayouts(); }
  void setExactAndApproximateAreStriclyEqual(bool isEqual) {
    return m_view.setExactAndApproximateAreStriclyEqual(isEqual);
  }
  void setHighlighted(bool highlight) override {
    m_view.evenOddCell()->setHighlighted(highlight);
  }
  void setEven(bool even) override;
  Escher::Responder* responder() override { return this; }
  Poincare::Layout layout() const override { return m_view.layout(); }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_view.minimalSizeForOptimalDisplay();
  }
  void didBecomeFirstResponder() override;
  void reinitSelection();

 private:
  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int index) override { return &m_view; }
  void layoutSubviews(bool force = false) override {
    setChildFrame(&m_view, bounds(), force);
  }
  ScrollableTwoLayoutsView m_view;
};

}  // namespace Shared

#endif
