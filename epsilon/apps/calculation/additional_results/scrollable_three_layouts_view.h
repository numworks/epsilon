#ifndef CALCULATION_SCROLLABLE_THREE_LAYOUTS_CELL_H
#define CALCULATION_SCROLLABLE_THREE_LAYOUTS_CELL_H

#include <escher/menu_cell.h>
#include <escher/scrollable_multiple_layouts_view.h>
#include <poincare/layout.h>

#include "../calculation.h"
#include "layout_with_equal_sign_view.h"

namespace Calculation {

class ScrollableThreeLayoutsView
    : public Escher::AbstractScrollableMultipleLayoutsView {
 public:
  ScrollableThreeLayoutsView(Responder* parentResponder = nullptr)
      : Escher::AbstractScrollableMultipleLayoutsView(parentResponder,
                                                      &m_contentCell) {
    resetMargins();  // margins are already added by MenuCell
    setBackgroundColor(KDColorWhite);
  }
  void subviewFrames(KDRect* leftFrame, KDRect* centerFrame,
                     KDRect* approximateSignFrame, KDRect* rightFrame) {
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
  void setLayouts(Poincare::Layout formulaLayout, Poincare::Layout exactLayout,
                  Poincare::Layout approximateLayout) override;

 protected:
  void handleResponderChainEvent(
      Responder::ResponderChainEvent event) override {
    if (event.type == ResponderChainEventType::HasBecomeFirst) {
      setSelectedSubviewPosition(leftMostPosition());
      reloadScroll();
    } else {
      Escher::AbstractScrollableMultipleLayoutsView::handleResponderChainEvent(
          event);
    }
  }

 private:
  class ContentCell
      : public Escher::AbstractScrollableMultipleLayoutsView::ContentCell {
    friend ScrollableThreeLayoutsView;

   public:
    using Escher::AbstractScrollableMultipleLayoutsView::ContentCell::
        ContentCell;
    KDColor backgroundColor() const override {
      return m_highlightWholeCell ? defaultBackgroundColor() : KDColorWhite;
    }
    void setEven(bool even) override { return; }
    Escher::LayoutView* leftLayoutView() const override {
      return const_cast<LayoutWithEqualSignView*>(&m_leftLayoutView);
    }
    void setShowEqualSignAfterFormula(bool showEqual) {
      m_leftLayoutView.setShowEqual(showEqual);
    }
    void setHighlightWholeCell(bool highlightWholeCell) {
      m_highlightWholeCell = highlightWholeCell;
    };

   private:
    void reloadTextColor() override;
    LayoutWithEqualSignView m_leftLayoutView;
    bool m_highlightWholeCell;
  };

  ContentCell* contentCell() override { return &m_contentCell; };
  const ContentCell* contentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

}  // namespace Calculation

#endif
