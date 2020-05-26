#ifndef CALCULATION_SCROLLABLE_THREE_EXPRESSIONS_CELL_H
#define CALCULATION_SCROLLABLE_THREE_EXPRESSIONS_CELL_H

#include <escher.h>
#include "../../shared/scrollable_multiple_expressions_view.h"
#include "../calculation.h"
#include "expression_with_equal_sign_view.h"

namespace Calculation {

class ScrollableThreeExpressionsView : public Shared::AbstractScrollableMultipleExpressionsView {
public:
  static constexpr KDCoordinate k_margin = Metric::CommonSmallMargin;
  ScrollableThreeExpressionsView(Responder * parentResponder) : Shared::AbstractScrollableMultipleExpressionsView(parentResponder, &m_contentCell), m_contentCell() {
    setMargins(k_margin, k_margin, k_margin, k_margin); // Left Right margins are already added by TableCell
    setBackgroundColor(KDColorWhite);
  }
  void resetMemoization();
  void setCalculation(Calculation * calculation);
  void subviewFrames(KDRect * leftFrame, KDRect * centerFrame, KDRect * approximateSignFrame, KDRect * rightFrame) {
    return m_contentCell.subviewFrames(leftFrame, centerFrame, approximateSignFrame, rightFrame);
  }
private:
  class ContentCell : public Shared::AbstractScrollableMultipleExpressionsView::ContentCell {
  public:
    ContentCell() : m_leftExpressionView() {}
    KDColor backgroundColor() const override { return KDColorWhite; }
    void setEven(bool even) override { return; }
    ExpressionView * leftExpressionView() const override { return const_cast<ExpressionWithEqualSignView *>(&m_leftExpressionView); }
  private:
    ExpressionWithEqualSignView m_leftExpressionView;
  };

  ContentCell *  contentCell() override { return &m_contentCell; };
  const ContentCell * constContentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

class ScrollableThreeExpressionsCell : public TableCell, public Responder {
public:
  static KDCoordinate Height(Calculation * calculation);
  ScrollableThreeExpressionsCell() :
    Responder(nullptr),
    m_view(this) {}

  // Cell
  Poincare::Layout layout() const override { return m_view.layout(); }

  // Responder cell
  Responder * responder() override {
    return this;
  }
  void didBecomeFirstResponder() override;

  // Table cell
  View * labelView() const override { return (View *)&m_view; }

  void setHighlighted(bool highlight) override { m_view.evenOddCell()->setHighlighted(highlight); }
  void resetMemoization() { m_view.resetMemoization(); }
  void setCalculation(Calculation * calculation);
  void setDisplayCenter(bool display);
  ScrollableThreeExpressionsView::SubviewPosition selectedSubviewPosition() { return m_view.selectedSubviewPosition(); }
  void setSelectedSubviewPosition(ScrollableThreeExpressionsView::SubviewPosition subviewPosition) { m_view.setSelectedSubviewPosition(subviewPosition); }

  void reinitSelection();
  void subviewFrames(KDRect * leftFrame, KDRect * centerFrame, KDRect * approximateSignFrame, KDRect * rightFrame) {
    return m_view.subviewFrames(leftFrame, centerFrame, approximateSignFrame, rightFrame);
  }
private:
  // Remove label margin added by TableCell because they're already handled by ScrollableThreeExpressionsView
  KDCoordinate labelMargin() const override { return 0; }
  ScrollableThreeExpressionsView m_view;
};

}

#endif
