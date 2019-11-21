#ifndef CALCULATION_SCROLLABLE_INPUT_EXACT_APPROXIMATE_EXPRESSIONS_CELL_H
#define CALCULATION_SCROLLABLE_INPUT_EXACT_APPROXIMATE_EXPRESSIONS_CELL_H

#include <escher.h>
#include "../../shared/scrollable_exact_approximate_expressions_view.h"
#include "../calculation.h"

namespace Calculation {

class ScrollableInputExactApproximateExpressionsView : public Shared::AbstractScrollableExactApproximateExpressionsView {
public:
  ScrollableInputExactApproximateExpressionsView(Responder * parentResponder) : Shared::AbstractScrollableExactApproximateExpressionsView(parentResponder, &m_contentCell), m_contentCell() {}
  void setCalculation(Calculation * calculation);
private:
  class ContentCell : public Shared::AbstractScrollableExactApproximateExpressionsView::ContentCell {
  public:
    ContentCell() : m_leftExpressionView() {}
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

class ScrollableInputExactApproximateExpressionsCell : public HighlightCell {
public:
  ScrollableInputExactApproximateExpressionsCell() : m_view(nullptr) {}
  void setParentResponder(Responder * r) { m_view.setParentResponder(r); }
  void setCalculation(Calculation * calculation) { m_view.setCalculation(calculation); }
private:
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override { return &m_view; }
  void layoutSubviews(bool force = false) override { m_view.setFrame(bounds(), force); }
  ScrollableInputExactApproximateExpressionsView m_view;
};

}

#endif
