#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/metric.h>
#include <escher/scroll_view_data_source.h>
#include <escher/scroll_view_indicator.h>
#include <escher/view.h>
#include <kandinsky/margins.h>

namespace Escher {

class ScrollView;

class ScrollViewDelegate {
 public:
  virtual KDRect visibleRectInBounds(ScrollView* scrollView) = 0;
};

class ScrollView : public View {
  /* TODO: Should we add a reload method that forces the relayouting of the
   * subviews? Or should ScrollView::setFrame always force the layouting of the
   * subviews ? Because the scroll view frame might not change but its content
   * might need to be relayouted.
   * cf TableView, InputViewController, EditExpressionController. */

 public:
  ScrollView(View* contentView, ScrollViewDataSource* dataSource,
             Escher::ScrollViewDelegate* scrollViewDelegate = nullptr);
  KDSize contentSizeWithMargins() const;
  KDSize minimalSizeForOptimalDisplay() const override;

  KDMargins* margins() { return &m_margins; }
  KDMargins constMargins() const { return m_margins; }
  void setMargins(KDMargins m) { m_margins = m; }
  void resetMargins() { setMargins({}); }

  class Decorator {
   public:
    virtual int numberOfIndicators() const { return 0; }
    virtual View* indicatorAtIndex(int index) {
      assert(false);
      return nullptr;
    }
    virtual KDRect layoutIndicators(
        View* parent, KDSize content, KDPoint offset, KDRect frame,
        KDRect* dirtyRect1, KDRect* dirtyRect2, bool force,
        ScrollViewDataSourceDelegate* delegate = nullptr) {
      return frame;
    }
    virtual void setBackgroundColor(KDColor c) {}
    virtual bool layoutBeforeInnerView() const { return true; }
  };

  // Decorator is the base class and an empty decorator
  using NoDecorator = Decorator;

  class BarDecorator : public Decorator {
   public:
    constexpr static KDCoordinate k_barsFrameBreadth =
        Metric::CommonMargins.right();
    void setVerticalMargins(KDVerticalMargins margins) {
      m_verticalBar.setMargins(margins);
    }
    int numberOfIndicators() const override { return m_visible ? 2 : 0; }
    View* indicatorAtIndex(int index) override;
    KDRect layoutIndicators(
        View* parent, KDSize content, KDPoint offset, KDRect frame,
        KDRect* dirtyRect1, KDRect* dirtyRect2, bool force,
        ScrollViewDataSourceDelegate* delegate = nullptr) override;
    ScrollViewVerticalBar* verticalBar() { return &m_verticalBar; }
    ScrollViewHorizontalBar* horizontalBar() { return &m_horizontalBar; }
    void setVisibility(bool visible) { m_visible = visible; }
    // Draw the bar after the table to know its size
    bool layoutBeforeInnerView() const override { return false; }

   private:
    ScrollViewVerticalBar m_verticalBar;
    ScrollViewHorizontalBar m_horizontalBar;
    bool m_visible;
  };

  class ArrowDecorator : public Decorator {
   public:
    int numberOfIndicators() const override { return 2; }
    View* indicatorAtIndex(int index) override;
    KDRect layoutIndicators(
        View* parent, KDSize content, KDPoint offset, KDRect frame,
        KDRect* dirtyRect1, KDRect* dirtyRect2, bool force,
        ScrollViewDataSourceDelegate* delegate = nullptr) override;
    void setBackgroundColor(KDColor c) override;
    void setFont(KDFont::Size font);

   private:
    ScrollViewRightArrow m_rightArrow;
    ScrollViewLeftArrow m_leftArrow;
  };

  virtual Decorator* decorator() = 0;
  virtual void setBackgroundColor(KDColor c) {
    m_backgroundColor = c;
    decorator()->setBackgroundColor(m_backgroundColor);
  }
  KDColor backgroundColor() const { return m_backgroundColor; }

  void setContentOffset(KDPoint offset);
  KDPoint contentOffset() const { return m_dataSource->offset(); }
  void resetScroll() { setContentOffset(KDPointZero); }
  void translateContentOffsetBy(KDPoint translation) {
    setContentOffset(contentOffset().translatedBy(translation));
  }

  // TODO: replace setContentOffset by setClippedContentOffset ?
  void setClippedContentOffset(KDPoint offset);
  void scrollToContentPoint(KDPoint point);
  // Minimal scrolling to make this rect visible
  void scrollToContentRect(KDRect rect);

  KDCoordinate maxContentWidthDisplayableWithoutScrolling() const {
    return (bounds().size() - m_margins).width();
  }
  KDCoordinate maxContentHeightDisplayableWithoutScrolling() const {
    return (bounds().size() - m_margins).height();
  }

 protected:
  void layoutSubviews(bool force = false) override;
  virtual KDSize contentSize() const {
    return m_contentView->minimalSizeForOptimalDisplay();
  }
  virtual bool alwaysForceRelayoutOfContentView() const { return false; }
  virtual float marginPortionTolerance() const { return 0.8f; }
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
  void logAttributes(std::ostream& os) const override;
#endif
  int numberOfSubviews() const override {
    return 1 + const_cast<ScrollView*>(this)->decorator()->numberOfIndicators();
  }
  View* subviewAtIndex(int index) override {
    return (index == 0) ? &m_innerView : decorator()->indicatorAtIndex(index);
  }

 private:
  class InnerView : public View {
   public:
    InnerView(ScrollView* scrollView) : View(), m_scrollView(scrollView) {}
    void drawRect(KDContext* ctx, KDRect rect) const override;

   private:
    int numberOfSubviews() const override { return 1; }
    View* subviewAtIndex(int index) override {
      assert(index == 0);
      return m_scrollView->m_contentView;
    }
    const ScrollView* m_scrollView;
  };

  KDRect layoutDecorator(bool force);
  KDRect visibleContentRect();
  KDRect visibleRectInBounds() {
    return m_scrollViewDelegate
               ? m_scrollViewDelegate->visibleRectInBounds(this)
               : bounds();
  }

  ScrollViewDataSource* m_dataSource;
  View* m_contentView;
  InnerView m_innerView;
  Escher::ScrollViewDelegate* m_scrollViewDelegate;

  KDMargins m_margins;
  mutable KDCoordinate m_excessWidth;
  mutable KDCoordinate m_excessHeight;

  KDColor m_backgroundColor;
};

}  // namespace Escher
#endif
