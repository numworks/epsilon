#ifndef SHARED_SCROLLABLE_TWO_EXPRESSIONS_CELL_H
#define SHARED_SCROLLABLE_TWO_EXPRESSIONS_CELL_H

#include <escher.h>
#include "scrollable_multiple_expressions_view.h"

namespace Shared {

class ScrollableTwoExpressionsCell : public ::EvenOddCell, public Responder {
public:
  ScrollableTwoExpressionsCell(Responder * parentResponder = nullptr);
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
  void reinitSelection();
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ScrollableTwoExpressionsView m_view;
};

}

#endif
