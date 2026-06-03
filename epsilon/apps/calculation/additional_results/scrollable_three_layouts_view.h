#pragma once

#include <escher/menu_cell.h>
#include <escher/scrollable_multiple_layouts_view.h>
#include <escher/theme/components.h>
#include <escher/theme/highlight.h>
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
    setBackgroundColor(Escher::ThemeComponent::DefaultBackground);
  }
  void subviewFrames(KDRect* leftFrame, KDRect* centerFrame,
                     KDRect* comparisonSignFrame, KDRect* rightFrame) {
    return m_contentCell.subviewFrames(leftFrame, centerFrame,
                                       comparisonSignFrame, rightFrame);
  }
  void setShowEqualSignAfterFormula(bool showEqual) {
    m_contentCell.setShowEqualSignAfterFormula(showEqual);
  }
  SubviewPosition leftMostPosition();
  void setHighlightWholeCell(bool highlightWholeCell) {
    m_contentCell.setHighlightWholeCell(highlightWholeCell);
  }
  void setLayouts(const Poincare::Layout& formulaLayout,
                  const Poincare::Layout& exactLayout,
                  const Poincare::Layout& approximateLayout) override;

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
    Escher::ThemeComponent backgroundColor() const override {
      return Escher::Highlight(k_defaultBackground,
                               m_highlightWholeCell && isHighlighted());
    }
    void setEven(bool even) override { return; }
    Escher::LayoutView* leftLayoutView() const override {
      return const_cast<LayoutWithEqualSignView*>(&m_leftLayoutView);
    }
    void setShowEqualSignAfterFormula(bool showEqual) {
      m_leftLayoutView.setShowEqual(showEqual);
    }

   private:
    void reloadTextColor() override;
    LayoutWithEqualSignView m_leftLayoutView;
  };

  ContentCell* contentCell() override { return &m_contentCell; };
  const ContentCell* contentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

}  // namespace Calculation
