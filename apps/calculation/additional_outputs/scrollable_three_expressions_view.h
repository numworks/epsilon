#ifndef CALCULATION_SCROLLABLE_THREE_EXPRESSIONS_CELL_H
#define CALCULATION_SCROLLABLE_THREE_EXPRESSIONS_CELL_H

#include <escher/menu_cell.h>
#include <poincare/layout.h>

#include "../../shared/scrollable_multiple_expressions_view.h"
#include "../calculation.h"
#include "expression_with_equal_sign_view.h"

namespace Calculation {

class ScrollableThreeExpressionsView
    : public Shared::AbstractScrollableMultipleExpressionsView {
 public:
  ScrollableThreeExpressionsView(Responder *parentResponder = nullptr)
      : Shared::AbstractScrollableMultipleExpressionsView(parentResponder,
                                                          &m_contentCell) {
    setMargins(0, 0, 0, 0);  // margins are already added by MenuCell
    setBackgroundColor(KDColorWhite);
  }
  void subviewFrames(KDRect *leftFrame, KDRect *centerFrame,
                     KDRect *approximateSignFrame, KDRect *rightFrame) {
    return m_contentCell.subviewFrames(leftFrame, centerFrame,
                                       approximateSignFrame, rightFrame);
  }
  void setShowEqualSignAfterFormula(bool showEqual) {
    m_contentCell.setShowEqualSignAfterFormula(showEqual);
  }
  SubviewPosition leftMostPosition();
  void setHighlightWholeCell(bool highlightWholeCell) {
    m_contentCell.setHighlightWholeCell(highlightWholeCell);
  }
  void didBecomeFirstResponder() override {
    setSelectedSubviewPosition(leftMostPosition());
    reloadScroll();
  }
  void setLayouts(Poincare::Layout formulaLayout, Poincare::Layout exactLayout,
                  Poincare::Layout approximateLayout) override;

 private:
  class ContentCell
      : public Shared::AbstractScrollableMultipleExpressionsView::ContentCell {
    friend ScrollableThreeExpressionsView;

   public:
    using Shared::AbstractScrollableMultipleExpressionsView::ContentCell::
        ContentCell;
    KDColor backgroundColor() const override {
      return m_highlightWholeCell ? defaultBackgroundColor() : KDColorWhite;
    }
    void setEven(bool even) override { return; }
    Escher::ExpressionView *leftExpressionView() const override {
      return const_cast<ExpressionWithEqualSignView *>(&m_leftExpressionView);
    }
    void setShowEqualSignAfterFormula(bool showEqual) {
      m_leftExpressionView.setShowEqual(showEqual);
    }
    void setHighlightWholeCell(bool highlightWholeCell) {
      m_highlightWholeCell = highlightWholeCell;
    };

   private:
    void reloadTextColor() override;
    ExpressionWithEqualSignView m_leftExpressionView;
    bool m_highlightWholeCell;
  };

  ContentCell *contentCell() override { return &m_contentCell; };
  const ContentCell *contentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

}  // namespace Calculation

#endif
