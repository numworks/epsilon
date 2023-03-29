#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/metric.h>
#include <escher/scroll_view_data_source.h>
#include <escher/scroll_view_indicator.h>
#include <escher/view.h>

namespace Escher {

class ScrollView : public View {
  /* TODO: Should we add a reload method that forces the relayouting of the
   * subviews? Or should ScrollView::setFrame always force the layouting of the
   * subviews ? Because the scroll view frame might not change but its content
   * might need to be relayouted.
   * cf TableView, InputViewController, EditExpressionController. */

 public:
  ScrollView(View *contentView, ScrollViewDataSource *dataSource);
  KDSize minimalSizeForOptimalDisplay() const override;

  void setTopMargin(KDCoordinate m) { m_topMargin = m; }
  void setRightMargin(KDCoordinate m) { m_rightMargin = m; }
  void setBottomMargin(KDCoordinate m) { m_bottomMargin = m; }
  void setLeftMargin(KDCoordinate m) { m_leftMargin = m; }
  KDCoordinate topMargin() const { return m_topMargin; }
  KDCoordinate rightMargin() const { return m_rightMargin; }
  KDCoordinate bottomMargin() const { return m_bottomMargin; }
  KDCoordinate leftMargin() const { return m_leftMargin; }

  void setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom,
                  KDCoordinate left);
  void setMargins(KDCoordinate m) { setMargins(m, m, m, m); }

  class Decorator {
   public:
    virtual int numberOfIndicators() const { return 0; }
    virtual View *indicatorAtIndex(int index) {
      assert(false);
      return nullptr;
    }
    virtual KDRect layoutIndicators(View *parent, KDSize content,
                                    KDPoint offset, KDRect frame,
                                    KDRect *dirtyRect1, KDRect *dirtyRect2,
                                    bool force,
                                    ScrollViewDelegate *delegate = nullptr) {
      return frame;
    }
    virtual void setBackgroundColor(KDColor c) {}
  };

  // Decorator is the base class and an empty decorator
  using NoDecorator = Decorator;

  class BarDecorator : public Decorator {
   public:
    constexpr static KDCoordinate k_barsFrameBreadth =
        Metric::CommonRightMargin;
    void setVerticalMargins(KDCoordinate top, KDCoordinate bottom) {
      m_verticalBar.setMargins(top, bottom);
    }
    int numberOfIndicators() const override { return m_visible ? 2 : 0; }
    View *indicatorAtIndex(int index) override;
    KDRect layoutIndicators(View *parent, KDSize content, KDPoint offset,
                            KDRect frame, KDRect *dirtyRect1,
                            KDRect *dirtyRect2, bool force,
                            ScrollViewDelegate *delegate = nullptr) override;
    ScrollViewVerticalBar *verticalBar() { return &m_verticalBar; }
    ScrollViewHorizontalBar *horizontalBar() { return &m_horizontalBar; }
    void setVisibility(bool visible) { m_visible = visible; }

   private:
    ScrollViewVerticalBar m_verticalBar;
    ScrollViewHorizontalBar m_horizontalBar;
    bool m_visible;
  };

  class ArrowDecorator : public Decorator {
   public:
    int numberOfIndicators() const override { return 2; }
    View *indicatorAtIndex(int index) override;
    KDRect layoutIndicators(View *parent, KDSize content, KDPoint offset,
                            KDRect frame, KDRect *dirtyRect1,
                            KDRect *dirtyRect2, bool force,
                            ScrollViewDelegate *delegate = nullptr) override;
    void setBackgroundColor(KDColor c) override;
    void setFont(KDFont::Size font);

   private:
    ScrollViewRightArrow m_rightArrow;
    ScrollViewLeftArrow m_leftArrow;
  };

  virtual Decorator *decorator() = 0;
  virtual void setBackgroundColor(KDColor c) {
    m_backgroundColor = c;
    decorator()->setBackgroundColor(m_backgroundColor);
  }
  KDColor backgroundColor() const { return m_backgroundColor; }

  void setContentOffset(KDPoint offset);
  KDPoint contentOffset() const { return m_dataSource->offset(); }

  void scrollToContentPoint(KDPoint point);
  // Minimal scrolling to make this rect visible
  void scrollToContentRect(KDRect rect);

 protected:
  KDCoordinate maxContentWidthDisplayableWithoutScrolling() const {
    return bounds().width() - m_leftMargin - m_rightMargin;
  }
  KDCoordinate maxContentHeightDisplayableWithoutScrolling() const {
    return bounds().height() - m_topMargin - m_bottomMargin;
  }
  KDRect visibleContentRect();
  void layoutSubviews(bool force = false) override;
  virtual KDSize contentSize() const {
    return m_contentView->minimalSizeForOptimalDisplay();
  }
  virtual float marginPortionTolerance() const { return 0.8f; }
#if ESCHER_VIEW_LOGGING
  const char *className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
  int numberOfSubviews() const override {
    return 1 +
           const_cast<ScrollView *>(this)->decorator()->numberOfIndicators();
  }
  View *subviewAtIndex(int index) override {
    return (index == 0) ? &m_innerView : decorator()->indicatorAtIndex(index);
  }

 private:
  class InnerView : public View {
   public:
    InnerView(ScrollView *scrollView) : View(), m_scrollView(scrollView) {}
    void drawRect(KDContext *ctx, KDRect rect) const override;

   private:
    int numberOfSubviews() const override { return 1; }
    View *subviewAtIndex(int index) override {
      assert(index == 0);
      return m_scrollView->m_contentView;
    }
    const ScrollView *m_scrollView;
  };

  ScrollViewDataSource *m_dataSource;
  View *m_contentView;
  InnerView m_innerView;

  KDCoordinate m_topMargin;
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;
  mutable KDCoordinate m_excessWidth;
  mutable KDCoordinate m_excessHeight;

  KDColor m_backgroundColor;
};

}  // namespace Escher
#endif
