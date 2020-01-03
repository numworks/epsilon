#ifndef SHARED_SCROLLABLE_EXACT_APPROXIMATE_EXPRESSIONS_VIEW_H
#define SHARED_SCROLLABLE_EXACT_APPROXIMATE_EXPRESSIONS_VIEW_H

#include <escher.h>

namespace Shared {

class ScrollableExactApproximateExpressionsView : public ScrollableView, public ScrollViewDataSource {
public:
  enum class SubviewPosition {
    Left,
    Right
  };
  ScrollableExactApproximateExpressionsView(Responder * parentResponder);
  ::EvenOddCell * evenOddCell() {
    return &m_contentCell;
  }
  void setLayouts(Poincare::Layout rightlayout, Poincare::Layout leftLayout);
  void setEqualMessage(I18n::Message equalSignMessage);
  SubviewPosition selectedSubviewPosition() {
    return m_contentCell.selectedSubviewPosition();
  }
  void setSelectedSubviewPosition(SubviewPosition subviewPosition) {
    m_contentCell.setSelectedSubviewPosition(subviewPosition);
  }
  void setDisplayLeftLayout(bool display) { m_contentCell.setDisplayLeftExpression(display); }
  void reloadScroll();
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Poincare::Layout layout() const {
    return m_contentCell.layout();
  }
private:
  class ContentCell : public ::EvenOddCell {
  public:
    ContentCell();
    KDColor backgroundColor() const override;
    void setHighlighted(bool highlight) override;
    void setEven(bool even) override;
    void reloadTextColor();
    KDSize minimalSizeForOptimalDisplay() const override;
    ExpressionView * rightExpressionView() {
      return &m_rightExpressionView;
    }
    ExpressionView * leftExpressionView() {
      return &m_leftExpressionView;
    }
    MessageTextView * approximateSign() {
      return &m_approximateSign;
    }
    SubviewPosition selectedSubviewPosition() {
      return m_selectedSubviewPosition;
    }
    void setSelectedSubviewPosition(SubviewPosition subviewPosition);
    bool displayLeftExpression() const { return m_displayLeftExpression; }
    void setDisplayLeftExpression(bool display);
    void layoutSubviews() override;
    int numberOfSubviews() const override;
    Poincare::Layout layout() const override;
  private:
    View * subviewAtIndex(int index) override;
    ExpressionView m_rightExpressionView;
    MessageTextView m_approximateSign;
    ExpressionView m_leftExpressionView;
    SubviewPosition m_selectedSubviewPosition;
    bool m_displayLeftExpression;
  };
  ContentCell m_contentCell;
};

}

#endif
