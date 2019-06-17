#ifndef SHARED_SCROLLABLE_EXACT_APPROXIMATE_EXPRESSIONS_CELL_H
#define SHARED_SCROLLABLE_EXACT_APPROXIMATE_EXPRESSIONS_CELL_H

#include <escher.h>
#include "scrollable_exact_approximate_expressions_view.h"

namespace Shared {

class ScrollableExactApproximateExpressionsCell : public ::EvenOddCell, public Responder {
public:
  ScrollableExactApproximateExpressionsCell(Responder * parentResponder = nullptr);
  void setLayouts(Poincare::Layout approximateLayout, Poincare::Layout exactLayout);
  void setEqualMessage(I18n::Message equalSignMessage) {
    return m_view.setEqualMessage(equalSignMessage);
  }
  void setHighlighted(bool highlight) override;
  void setEven(bool even) override;
  void reloadScroll();
  Responder * responder() override {
    return this;
  }
  Poincare::Layout layout() const override { return m_view.layout(); }
  void didBecomeFirstResponder() override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  ScrollableExactApproximateExpressionsView m_view;
};

}

#endif
