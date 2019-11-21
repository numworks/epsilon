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
    KDCoordinate leftBaseline() const override { return !m_leftExpressionView.layout().isUninitialized() ? m_leftExpressionView.layout().baseline() : 0; }
    KDSize leftMinimalSizeForOptimalDisplay() const override { return m_leftExpressionView.minimalSizeForOptimalDisplay(); }
    ExpressionView m_leftExpressionView;
  };

  ContentCell *  contentCell() override { return &m_contentCell; };
  const ContentCell *  constContentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

class ScrollableInputExactApproximateExpressionsCell : public TableCell, public Responder {
public:
  ScrollableInputExactApproximateExpressionsCell() :
    Responder(nullptr),
    m_view(this) {}

  // Responder cell
  Responder * responder() override {
    return this;
  }
  void didBecomeFirstResponder() override;

  // Table cell
  View * labelView() const override { return (View *)&m_view; }

  void setHighlighted(bool highlight) override {
    m_view.evenOddCell()->setHighlighted(highlight);
    m_view.reloadScroll();
  }
  void setCalculation(Calculation * calculation) { m_view.setCalculation(calculation); }
  void setDisplayCenter(bool display) { m_view.setDisplayCenter(display); }
  void setDisplayLeft(bool display) { m_view.setDisplayLeft(display); }

private:
  ScrollableInputExactApproximateExpressionsView m_view;
};

}

#endif
