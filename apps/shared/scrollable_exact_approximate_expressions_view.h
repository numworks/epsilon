#ifndef SHARED_SCROLLABLE_EXACT_APPROXIMATE_EXPRESSIONS_VIEW_H
#define SHARED_SCROLLABLE_EXACT_APPROXIMATE_EXPRESSIONS_VIEW_H

#include <escher.h>

namespace Shared {

class ScrollableExactApproximateExpressionsView : public ScrollableView, public ScrollViewDataSource {
public:
  enum class SubviewType {
    ExactOutput,
    ApproximativeOutput
  };
  ScrollableExactApproximateExpressionsView(Responder * parentResponder);
  ::EvenOddCell * evenOddCell() {
    return &m_contentCell;
  }
  void setExpressions(Poincare::ExpressionLayout ** expressionsLayout);
  void setEqualMessage(I18n::Message equalSignMessage);
  SubviewType selectedSubviewType() {
    return m_contentCell.selectedSubviewType();
  }
  void setSelectedSubviewType(SubviewType subviewType) {
    m_contentCell.setSelectedSubviewType(subviewType);
  }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  class ContentCell : public ::EvenOddCell {
  public:
    ContentCell();
    KDColor backgroundColor() const override;
    void setHighlighted(bool highlight) override;
    void reloadCell() override;
    KDSize minimalSizeForOptimalDisplay() const override;
    ExpressionView * approximateExpressionView() {
      return &m_approximateExpressionView;
    }
    ExpressionView * exactExpressionView() {
      return &m_exactExpressionView;
    }
    MessageTextView * approximateSign() {
      return &m_approximateSign;
    }
    SubviewType selectedSubviewType() {
      return m_selectedSubviewType;
    }
    void setSelectedSubviewType(SubviewType subviewType);
    void layoutSubviews() override;
    int numberOfSubviews() const override;
  private:
    View * subviewAtIndex(int index) override;
    constexpr static KDCoordinate k_digitHorizontalMargin = 10;
    ExpressionView m_approximateExpressionView;
    MessageTextView m_approximateSign;
    ExpressionView m_exactExpressionView;
    SubviewType m_selectedSubviewType;
};
  ContentCell m_contentCell;
};

}

#endif
