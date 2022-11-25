#ifndef CALCULATION_SCROLLABLE_THREE_EXPRESSIONS_CELL_H
#define CALCULATION_SCROLLABLE_THREE_EXPRESSIONS_CELL_H

#include "../../shared/scrollable_multiple_expressions_view.h"
#include "../calculation.h"
#include "expression_with_equal_sign_view.h"
#include <poincare/layout.h>
#include <escher/table_cell.h>

namespace Calculation {

class ScrollableThreeExpressionsView : public Shared::AbstractScrollableMultipleExpressionsView {
public:
  ScrollableThreeExpressionsView(Responder * parentResponder) : Shared::AbstractScrollableMultipleExpressionsView(parentResponder, &m_contentCell) {
    setMargins(0, 0, 0, 0); // margins are already added by TableCell
    setBackgroundColor(KDColorWhite);
  }
  void resetMemoization();
  void subviewFrames(KDRect * leftFrame, KDRect * centerFrame, KDRect * approximateSignFrame, KDRect * rightFrame) {
    return m_contentCell.subviewFrames(leftFrame, centerFrame, approximateSignFrame, rightFrame);
  }
  void setShowEqual(bool showEqual) {
    m_contentCell.setShowEqual(showEqual);
  }
  SubviewPosition leftMostPosition();
  void setHighlightWholeCell(bool highlightWholeCell) { m_contentCell.setHighlightWholeCell(highlightWholeCell); }
private:
  class ContentCell : public Shared::AbstractScrollableMultipleExpressionsView::ContentCell {
    friend ScrollableThreeExpressionsView;
  public:
    using Shared::AbstractScrollableMultipleExpressionsView::ContentCell::ContentCell;
    KDColor backgroundColor() const override { return m_highlightWholeCell ? defaultBackgroundColor() : KDColorWhite; }
    void setEven(bool even) override { return; }
    Escher::ExpressionView * leftExpressionView() const override { return const_cast<ExpressionWithEqualSignView *>(&m_leftExpressionView); }
    void setShowEqual(bool showEqual) {
      m_leftExpressionView.setShowEqual(showEqual);
    }
    void setHighlightWholeCell(bool highlightWholeCell) { m_highlightWholeCell = highlightWholeCell; };

  private:
    ExpressionWithEqualSignView m_leftExpressionView;
    bool m_highlightWholeCell;
  };

  ContentCell * contentCell() override { return &m_contentCell; };
  const ContentCell * contentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

class ScrollableThreeExpressionsCell : public Escher::TableCell, public Escher::Responder {
public:
  ScrollableThreeExpressionsCell() :
    Responder(nullptr),
    m_view(this) {}

  // Cell
  Poincare::Layout layout() const override { return m_view.layout(); }
  void setLayouts(Poincare::Layout leftLayout, Poincare::Layout centerLayout, Poincare::Layout rightLayout);

  // Responder cell
  Escher::Responder * responder() override {
    return this;
  }
  void didBecomeFirstResponder() override;

  // Table cell
  const View * labelView() const override { return &m_view; }

  void setHighlighted(bool highlight) override { m_view.evenOddCell()->setHighlighted(highlight); }
  void resetMemoization() { m_view.resetMemoization(); }
  void setRightIsStrictlyEqual(bool isEqual) { m_view.setRightIsStrictlyEqual(isEqual); }
  ScrollableThreeExpressionsView::SubviewPosition selectedSubviewPosition() { return m_view.selectedSubviewPosition(); }
  void setSelectedSubviewPosition(ScrollableThreeExpressionsView::SubviewPosition subviewPosition) { m_view.setSelectedSubviewPosition(subviewPosition); }

  void reinitSelection();
protected:
  ScrollableThreeExpressionsView m_view;
};

}

#endif
