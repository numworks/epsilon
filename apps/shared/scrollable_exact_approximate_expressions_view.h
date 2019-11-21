#ifndef SHARED_SCROLLABLE_EXACT_APPROXIMATE_EXPRESSIONS_VIEW_H
#define SHARED_SCROLLABLE_EXACT_APPROXIMATE_EXPRESSIONS_VIEW_H

#include <escher.h>

namespace Shared {

class AbstractScrollableExactApproximateExpressionsView : public ScrollableView, public ScrollViewDataSource {
public:
  enum class SubviewPosition : uint8_t {
    Left = 0,
    Center = 1,
    Right = 2
  };
  AbstractScrollableExactApproximateExpressionsView(Responder * parentResponder, View * contentCell);
  ::EvenOddCell * evenOddCell() {
    return contentCell();
  }
  void setLayouts(Poincare::Layout rightlayout, Poincare::Layout leftLayout);
  void setEqualMessage(I18n::Message equalSignMessage);
  SubviewPosition selectedSubviewPosition() {
    return contentCell()->selectedSubviewPosition();
  }
  void setSelectedSubviewPosition(SubviewPosition subviewPosition) {
    contentCell()->setSelectedSubviewPosition(subviewPosition);
  }
  void setDisplayCenter(bool display) { contentCell()->setDisplayCenter(display); }
  void setDisplayLeft(bool display) { contentCell()->setDisplayLeft(display); }
  void reloadScroll();
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Poincare::Layout layout() const {
    return constContentCell()->layout();
  }
protected:
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
    ExpressionView * centeredExpressionView() {
      return &m_centeredExpressionView;
    }
    MessageTextView * approximateSign() {
      return &m_approximateSign;
    }
    SubviewPosition selectedSubviewPosition() const {
      return m_selectedSubviewPosition;
    }
    void setSelectedSubviewPosition(SubviewPosition subviewPosition);
    bool displayCenter() const { return m_displayCenter; }
    void setDisplayCenter(bool display);
    bool displayLeft() const { return m_displayLeft; }
    void setDisplayLeft(bool display) { m_displayLeft = display; }
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override;
    virtual Poincare::Layout layout() const override;

    virtual View * leftView() = 0;
  private:
    virtual void setLeftViewBackgroundColor(KDColor color) = 0;
    virtual KDSize leftMinimalSizeForOptimalDisplay() const = 0;
    virtual KDCoordinate leftBaseline() const = 0;

    View * subviewAtIndex(int index) override;
    ExpressionView m_rightExpressionView;
    MessageTextView m_approximateSign;
    ExpressionView m_centeredExpressionView;
    SubviewPosition m_selectedSubviewPosition;
    bool m_displayCenter;
    bool m_displayLeft;
  };
  virtual ContentCell *  contentCell() = 0;
  virtual const ContentCell *  constContentCell() const = 0;
};


class ScrollableExactApproximateExpressionsView : public AbstractScrollableExactApproximateExpressionsView {
public:
  ScrollableExactApproximateExpressionsView(Responder * parentResponder) : AbstractScrollableExactApproximateExpressionsView(parentResponder, &m_contentCell) {
    setMargins(
        Metric::CommonSmallMargin,
        Metric::CommonLargeMargin,
        Metric::CommonSmallMargin,
        Metric::CommonLargeMargin
    );
  }

private:
  class ContentCell : public AbstractScrollableExactApproximateExpressionsView::ContentCell {
  public:
    View * leftView() override { return ContentCell::burgerMenuView(); }
  private:
    /* We keep only one instance of BurgerMenuView to avoid wasting space when
     * we know that only one ScrollableExactApproximateExpressionsView display
     * the burger view at a time. */
    static BurgerMenuView * burgerMenuView();
    void setLeftViewBackgroundColor(KDColor color) override { burgerMenuView()->setBackgroundColor(color); }
    KDSize leftMinimalSizeForOptimalDisplay() const override { return burgerMenuView()->minimalSizeForOptimalDisplay(); }
    KDCoordinate leftBaseline() const override { return leftMinimalSizeForOptimalDisplay().height()/2; }
  };
  ContentCell *  contentCell() override { return &m_contentCell; };
  const ContentCell *  constContentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

}

#endif
