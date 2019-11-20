#ifndef CALCULATION_SCROLLABLE_INPUT_EXACT_APPROXIMATE_EXPRESSIONS_CELL_H
#define CALCULATION_SCROLLABLE_INPUT_EXACT_APPROXIMATE_EXPRESSIONS_CELL_H

#include <escher.h>
#include "../../shared/scrollable_exact_approximate_expressions_view.h"
#include "../calculation.h"

namespace Calculation {

class ScrollableInputExactApproximateExpressionsView : public Shared::AbstractScrollableExactApproximateExpressionsView {
public:
  ScrollableInputExactApproximateExpressionsView(Responder * parentResponder) : Shared::AbstractScrollableExactApproximateExpressionsView(parentResponder, &m_contentCell) {}
  void setCalculation(Calculation * calculation);
private:
  class ContentCell : public Shared::AbstractScrollableExactApproximateExpressionsView::ContentCell {
  public:
    Poincare::Layout layout() const override;
    KDColor backgroundColor() const override { return KDColorWhite; }
    void setEven(bool even) override { return; }
    View * leftView() override { return &m_leftExpressionView; }
    ExpressionView * leftExpressionView() { return &m_leftExpressionView; }
  private:
    void setLeftViewBackgroundColor(KDColor color) override { m_leftExpressionView.setBackgroundColor(color); }
    KDCoordinate leftBaseline() const override { return m_leftExpressionView.layout().isUninitialized() ? m_leftExpressionView.layout().baseline() : 0; }
    KDSize leftMinimalSizeForOptimalDisplay() const override { return m_leftExpressionView.minimalSizeForOptimalDisplay(); }
    ExpressionView m_leftExpressionView;
  };

  ContentCell *  contentCell() override { return &m_contentCell; };
  const ContentCell *  constContentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

}

#endif
