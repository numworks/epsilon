#ifndef SHARED_SCROLLABLE_TWO_EXPRESSIONS_CELL_H
#define SHARED_SCROLLABLE_TWO_EXPRESSIONS_CELL_H

#include "scrollable_multiple_expressions_view.h"

namespace Shared {

class ScrollableTwoExpressionsCell : public Escher::EvenOddCell,
                                     public Escher::Responder {
 public:
  ScrollableTwoExpressionsCell(Escher::Responder* parentResponder = nullptr,
                               float horizontalAlignment = KDGlyph::k_alignLeft,
                               KDFont::Size font = KDFont::Size::Large);
  void setLayouts(Poincare::Layout exactLayout,
                  Poincare::Layout approximateLayout) {
    m_view.setLayouts(Poincare::Layout(), exactLayout, approximateLayout);
  }
  void resetLayouts() { return m_view.resetLayouts(); }
  void setRightIsStrictlyEqual(bool isEqual) {
    return m_view.setRightIsStrictlyEqual(isEqual);
  }
  void setHighlighted(bool highlight) override {
    m_view.evenOddCell()->setHighlighted(highlight);
  }
  void setEven(bool even) override;
  void reloadScroll() { m_view.reloadScroll(); }
  Escher::Responder* responder() override { return this; }
  Poincare::Layout layout() const override { return m_view.layout(); }
  Poincare::Layout exactLayout() const {
    return m_view.layoutAtPosition(
        AbstractScrollableMultipleExpressionsView::SubviewPosition::Center);
  }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_view.minimalSizeForOptimalDisplay();
  }
  void setDisplayCenter(bool display) { m_view.setDisplayCenter(display); }
  void didBecomeFirstResponder() override;
  void reinitSelection();

 private:
  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int index) override { return &m_view; }
  void layoutSubviews(bool force = false) override {
    setChildFrame(&m_view, bounds(), force);
  }
  ScrollableTwoExpressionsView m_view;
};

}  // namespace Shared

#endif
