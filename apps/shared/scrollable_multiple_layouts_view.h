#ifndef SHARED_SCROLLABLE_MULTIPLE_LAYOUTS_VIEW_H
#define SHARED_SCROLLABLE_MULTIPLE_LAYOUTS_VIEW_H

#include <apps/i18n.h>
#include <escher/even_odd_cell.h>
#include <escher/layout_view.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>
#include <escher/scrollable_view.h>

namespace Shared {

class AbstractScrollableMultipleLayoutsView
    : public Escher::ScrollableView<Escher::ScrollView::ArrowDecorator>,
      public Escher::ScrollViewDataSource,
      public Escher::CellWidget {
 public:
  constexpr static KDCoordinate k_horizontalMargin =
      Escher::Metric::CommonLargeMargin;
  enum class SubviewPosition : uint8_t { Left = 0, Center = 1, Right = 2 };

  AbstractScrollableMultipleLayoutsView(Responder* parentResponder,
                                        View* contentCell);
  Escher::EvenOddCell* evenOddCell() { return contentCell(); }
  virtual void setLayouts(Poincare::Layout formulaLayout,
                          Poincare::Layout exactLayout,
                          Poincare::Layout approximateLayout);
  void resetLayouts() {
    setSelectedSubviewPosition(SubviewPosition::Left);
    setLayouts(Poincare::Layout(), Poincare::Layout(), Poincare::Layout());
  }
  void setExactAndApproximateAreStriclyEqual(bool isEqual) {
    contentCell()->setExactAndApproximateAreStriclyEqual(isEqual);
  }
  SubviewPosition selectedSubviewPosition() {
    return contentCell()->selectedSubviewPosition();
  }
  void setSelectedSubviewPosition(SubviewPosition subviewPosition) {
    contentCell()->setSelectedSubviewPosition(subviewPosition);
  }
  bool displayCenter() const { return contentCell()->displayCenter(); }
  void setDisplayCenter(bool display);
  void setDisplayableCenter(bool displayable) {
    contentCell()->setDisplayableCenter(displayable);
  }
  void reloadScroll();
  bool handleEvent(Ion::Events::Event event) override;
  Poincare::Layout layout() const override { return contentCell()->layout(); }
  Poincare::Layout layoutAtPosition(SubviewPosition position) const {
    return contentCell()->layoutAtPosition(position);
  }
  KDCoordinate baseline() const { return contentCell()->baseline(); }

  // CellWidget
  const View* view() const override { return this; }
  Responder* responder() override { return this; }
  void setBackgroundColor(KDColor color) override {
    Escher::ScrollableView<
        Escher::ScrollView::ArrowDecorator>::setBackgroundColor(color);
  }
  void setHighlighted(bool highlighted) override {
    setBackgroundColor(Escher::AbstractMenuCell::BackgroundColor(highlighted));
  }

 protected:
  class ContentCell : public Escher::EvenOddCell {
   public:
    ContentCell(float horizontalAlignment = KDGlyph::k_alignLeft,
                KDFont::Size font = KDFont::Size::Large);
    KDColor backgroundColor() const override;
    void setHighlighted(bool highlight) override;
    virtual void reloadTextColor();
    KDSize minimalSizeForOptimalDisplay() const override;
    KDSize minimalSizeForOptimalDisplayFullSize() const;
    KDFont::Size font() const {
      assert(m_rightLayoutView.font() == m_centeredLayoutView.font());
      return m_rightLayoutView.font();
    }
    virtual Escher::LayoutView* leftLayoutView() const { return nullptr; }
    Escher::LayoutView* rightLayoutView() { return &m_rightLayoutView; }
    Escher::LayoutView* centeredLayoutView() { return &m_centeredLayoutView; }
    Escher::MessageTextView* approximateSign() { return &m_approximateSign; }
    SubviewPosition selectedSubviewPosition() const {
      return m_selectedSubviewPosition;
    }
    void setSelectedSubviewPosition(SubviewPosition subviewPosition);
    bool displayApproximateSign() const {
      return displayCenter() && !m_rightLayoutView.layout().isUninitialized();
    }
    bool displayCenter() const {
      return m_displayCenter &&
             !m_centeredLayoutView.layout().isUninitialized();
    }
    bool displayableCenter() const {
      return m_displayableCenter &&
             !m_centeredLayoutView.layout().isUninitialized();
    }
    bool rightIsStrictlyEqual() const { return m_rightIsStrictlyEqual; }
    void setDisplayCenter(bool display);
    void setDisplayableCenter(bool displayable) {
      m_displayableCenter = displayable;
    }
    void setExactAndApproximateAreStriclyEqual(bool isEqual);
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override;
    Poincare::Layout layout() const override {
      return layoutAtPosition(m_selectedSubviewPosition);
    }
    Poincare::Layout layoutAtPosition(SubviewPosition position) const;
    KDCoordinate baseline(KDCoordinate* leftBaseline = nullptr,
                          KDCoordinate* centerBaseline = nullptr,
                          KDCoordinate* rightBaseline = nullptr) const;
    void subviewFrames(KDRect* leftFrame, KDRect* centerFrame,
                       KDRect* approximateSignFrame, KDRect* rightFrame);

   private:
    const static I18n::Message k_defaultApproximateMessage =
        I18n::Message::AlmostEqual;
    void updateSubviewsBackgroundAfterChangingState() override;
    KDSize privateMinimalSizeForOptimalDisplay(bool forceFullDisplay) const;
    View* subviewAtIndex(int index) override;
    Escher::LayoutView m_rightLayoutView;
    Escher::MessageTextView m_approximateSign;
    Escher::LayoutView m_centeredLayoutView;
    SubviewPosition m_selectedSubviewPosition;
    bool m_displayCenter;
    bool m_displayableCenter;
    bool m_rightIsStrictlyEqual;
    float m_horizontalAlignment;
  };
  virtual ContentCell* contentCell() = 0;
  virtual const ContentCell* contentCell() const = 0;
};

class ScrollableTwoLayoutsView : public AbstractScrollableMultipleLayoutsView {
 public:
  ScrollableTwoLayoutsView(Escher::Responder* parentResponder,
                           float horizontalAlignment = KDGlyph::k_alignLeft,
                           KDFont::Size font = KDFont::Size::Large)
      : AbstractScrollableMultipleLayoutsView(parentResponder, &m_contentCell),
        m_contentCell(horizontalAlignment, font) {
    setMargins(
        Escher::Metric::CommonSmallMargin, Escher::Metric::CommonLargeMargin,
        Escher::Metric::CommonSmallMargin, Escher::Metric::CommonLargeMargin);
  }
  KDSize minimalSizeForOptimalDisplayFullSize() const;
  KDFont::Size font() const { return m_contentCell.font(); }

 private:
  ContentCell* contentCell() override { return &m_contentCell; };
  const ContentCell* contentCell() const override { return &m_contentCell; };
  ContentCell m_contentCell;
};

}  // namespace Shared

#endif
