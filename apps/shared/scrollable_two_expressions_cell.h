#ifndef SHARED_SCROLLABLE_TWO_EXPRESSIONS_CELL_H
#define SHARED_SCROLLABLE_TWO_EXPRESSIONS_CELL_H

#include "scrollable_multiple_expressions_view.h"

namespace Shared {

class ScrollableTwoExpressionsCell : public Escher::EvenOddCell, public Escher::Responder {
public:
  ScrollableTwoExpressionsCell(Escher::Responder * parentResponder = nullptr, float horizontalAlignment = KDContext::k_alignLeft, KDFont::Size font = KDFont::Size::Large);
  void setLayouts(Poincare::Layout exactLayout, Poincare::Layout approximateLayout);
  void setEqualMessage(I18n::Message equalSignMessage) {
    return m_view.setEqualMessage(equalSignMessage);
  }
  void setHighlighted(bool highlight) override;
  void setEven(bool even) override;
  void reloadScroll();
  Escher::Responder * responder() override {
    return this;
  }
  Poincare::Layout layout() const override { return m_view.layout(); }
  Poincare::Layout exactLayout() const { return m_view.layoutAtPosition(AbstractScrollableMultipleExpressionsView::SubviewPosition::Center); }
  KDSize minimalSizeForOptimalDisplay() const override { return m_view.minimalSizeForOptimalDisplay(); }
  void setInnerMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left) { m_view.setMargins(top, right, bottom, left); }
  void setDisplayCenter(bool display) { m_view.setDisplayCenter(display); }
  void didBecomeFirstResponder() override;
  void reinitSelection();
private:
  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ScrollableTwoExpressionsView m_view;
};

}

#endif
