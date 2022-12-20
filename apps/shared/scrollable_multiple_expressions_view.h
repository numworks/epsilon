#ifndef SHARED_SCROLLABLE_MULTIPLE_EXPRESSIONS_VIEW_H
#define SHARED_SCROLLABLE_MULTIPLE_EXPRESSIONS_VIEW_H

#include <apps/i18n.h>
#include <escher/even_odd_cell.h>
#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>
#include <escher/scrollable_view.h>

namespace Shared {

class AbstractScrollableMultipleExpressionsView : public Escher::ScrollableView, public Escher::ScrollViewDataSource {
public:
  constexpr static KDCoordinate k_horizontalMargin = Escher::Metric::CommonLargeMargin;
  enum class SubviewPosition : uint8_t {
    Left = 0,
    Center = 1,
    Right = 2
  };

  AbstractScrollableMultipleExpressionsView(Responder * parentResponder, View * contentCell);
  Escher::EvenOddCell * evenOddCell() {
    return contentCell();
  }
  void setLayouts(Poincare::Layout leftLayout, Poincare::Layout centerlayout, Poincare::Layout rightLayout);
  void setRightIsStrictlyEqual(bool isEqual) { contentCell()->setRightIsStrictlyEqual(isEqual); }
  SubviewPosition selectedSubviewPosition() {
    return contentCell()->selectedSubviewPosition();
  }
  void setSelectedSubviewPosition(SubviewPosition subviewPosition) {
    contentCell()->setSelectedSubviewPosition(subviewPosition);
  }
  bool displayCenter() const { return contentCell()->displayCenter(); }
  void setDisplayCenter(bool display);
  void setDisplayableCenter(bool displayable) { contentCell()->setDisplayableCenter(displayable); }
  void reloadScroll();
  bool handleEvent(Ion::Events::Event & event) override;
  Poincare::Layout layout() const { return contentCell()->layout(); }
  Poincare::Layout layoutAtPosition(SubviewPosition position) const { return contentCell()->layoutAtPosition(position); }
  KDCoordinate baseline() const { return contentCell()->baseline(); }
protected:
  class ContentCell : public Escher::EvenOddCell {
  public:
    ContentCell(float horizontalAlignment = KDContext::k_alignLeft, KDFont::Size font = KDFont::Size::Large);
    KDColor backgroundColor() const override;
    void setHighlighted(bool highlight) override;
    virtual void reloadTextColor();
    KDSize minimalSizeForOptimalDisplay() const override;
    KDSize minimalSizeForOptimalDisplayFullSize() const;
    KDFont::Size font() const {
      assert(m_rightExpressionView.font() == m_centeredExpressionView.font());
      return m_rightExpressionView.font();
    }
    virtual Escher::ExpressionView * leftExpressionView() const { return nullptr; }
    Escher::ExpressionView * rightExpressionView() {
      return &m_rightExpressionView;
    }
    Escher::ExpressionView * centeredExpressionView() {
      return &m_centeredExpressionView;
    }
    Escher::MessageTextView * approximateSign() {
      return &m_approximateSign;
    }
    SubviewPosition selectedSubviewPosition() const {
      return m_selectedSubviewPosition;
    }
    void setSelectedSubviewPosition(SubviewPosition subviewPosition);
    bool displayApproximateSign() const { return displayCenter() && !m_rightExpressionView.layout().isUninitialized(); }
    bool displayCenter() const { return m_displayCenter && !m_centeredExpressionView.layout().isUninitialized(); }
    bool displayableCenter() const { return m_displayableCenter && !m_centeredExpressionView.layout().isUninitialized(); }
    bool rightIsStrictlyEqual() const { return m_rightIsStrictlyEqual; }
    void setDisplayCenter(bool display);
    void setDisplayableCenter(bool displayable) {m_displayableCenter = displayable;}
    void setRightIsStrictlyEqual(bool isEqual);
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override;
    Poincare::Layout layout() const override { return layoutAtPosition(m_selectedSubviewPosition); }
    Poincare::Layout layoutAtPosition(SubviewPosition position) const;
    KDCoordinate baseline(KDCoordinate * leftBaseline = nullptr, KDCoordinate * centerBaseline = nullptr, KDCoordinate * rightBaseline = nullptr) const;
    void subviewFrames(KDRect * leftFrame, KDRect * centerFrame, KDRect * approximateSignFrame, KDRect * rightFrame);
  private:
    const static I18n::Message k_defaultApproximateMessage = I18n::Message::AlmostEqual;
    void updateSubviewsBackgroundAfterChangingState() override;
    KDSize privateMinimalSizeForOptimalDisplay(bool forceFullDisplay) const;
    View * subviewAtIndex(int index) override;
    Escher::ExpressionView m_rightExpressionView;
    Escher::MessageTextView m_approximateSign;
    Escher::ExpressionView m_centeredExpressionView;
    SubviewPosition m_selectedSubviewPosition;
    bool m_displayCenter;
    bool m_displayableCenter;
    bool m_rightIsStrictlyEqual;
    float m_horizontalAlignment;
  };
  virtual ContentCell * contentCell() = 0;
  virtual const ContentCell * contentCell() const = 0;
};

class ScrollableTwoExpressionsView : public AbstractScrollableMultipleExpressionsView {
public:
  ScrollableTwoExpressionsView(Escher::Responder * parentResponder, float horizontalAlignment = KDContext::k_alignLeft, KDFont::Size font = KDFont::Size::Large) :
    AbstractScrollableMultipleExpressionsView(parentResponder, &m_contentCell),
    m_contentCell(horizontalAlignment, font) {
    setMargins(
        Escher::Metric::CommonSmallMargin,
        Escher::Metric::CommonLargeMargin,
        Escher::Metric::CommonSmallMargin,
        Escher::Metric::CommonLargeMargin
    );
  }
  KDSize minimalSizeForOptimalDisplayFullSize() const;
  KDFont::Size font() const { return m_contentCell.font(); }
private:
  ContentCell *  contentCell() override { return &m_contentCell; };
  const ContentCell * contentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};


}

#endif
