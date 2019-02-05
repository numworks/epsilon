#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>
#include <escher/scroll_view_data_source.h>
#include <escher/scroll_view_indicator.h>

class ScrollView : public View {
public:
  ScrollView(View * contentView, ScrollViewDataSource * dataSource);
  KDSize minimalSizeForOptimalDisplay() const override;

  void setTopMargin(KDCoordinate m) { m_topMargin = m; }
  KDCoordinate topMargin() const { return m_topMargin; }
  void setRightMargin(KDCoordinate m) { m_rightMargin = m; }
  KDCoordinate rightMargin() const { return m_rightMargin; }
  void setBottomMargin(KDCoordinate m) { m_bottomMargin = m; }
  KDCoordinate bottomMargin() const { return m_bottomMargin; }
  void setLeftMargin(KDCoordinate m) { m_leftMargin = m; }
  KDCoordinate leftMargin() const { return m_leftMargin; }

  void setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left) {
    setTopMargin(top); setRightMargin(right); setBottomMargin(bottom); setLeftMargin(left);
  }
  void setMargins(KDCoordinate m) { setMargins(m, m, m, m); }
  void setCommonMargins();

  class Decorator {
  public:
    enum class Type {
      None,
      Bars,
      Arrows
    };
    virtual int numberOfIndicators() { return 0; }
    virtual View * indicatorAtIndex(int index) { assert(false); return nullptr; }
    virtual void layoutIndicators(KDSize content, KDPoint offset, KDSize frame) {}
  };

  class BarDecorator : public Decorator {
  public:
    BarDecorator();
    int numberOfIndicators() override { return 2; }
    View * indicatorAtIndex(int index) override {
      assert(0 < index && index <= numberOfIndicators());
      return &m_verticalBar + (index-1);
    }
    void layoutIndicators(KDSize content, KDPoint offset, KDSize frame) override;
    ScrollViewVerticalBar * verticalBar() { return &m_verticalBar; }
    ScrollViewHorizontalBar * horizontalBar() { return &m_horizontalBar; }
    void setBarsFrameBreadth(KDCoordinate t) { m_barsFrameBreadth = t; }
  private:
    ScrollViewVerticalBar m_verticalBar;
    ScrollViewHorizontalBar m_horizontalBar;
    KDCoordinate m_barsFrameBreadth;
  };

  class ArrowDecorator : public Decorator {
  public:
    ArrowDecorator();
    int numberOfIndicators() override { return 4; }
    View * indicatorAtIndex(int index) override {
      assert(0 < index && index <= numberOfIndicators());
      return &m_topArrow + (index-1);
    }
    void layoutIndicators(KDSize content, KDPoint offset, KDSize frame) override;
  private:
    ScrollViewArrow m_topArrow;
    ScrollViewArrow m_rightArrow;
    ScrollViewArrow m_bottomArrow;
    ScrollViewArrow m_leftArrow;
  };

  Decorator * decorator();
  void setDecoratorType(Decorator::Type t) { m_decoratorType = t; }
  virtual void setBackgroundColor(KDColor c) { m_backgroundColor = c; }
  KDColor backgroundColor() const { return m_backgroundColor; }

  void setContentOffset(KDPoint offset, bool forceRelayout = false);
  KDPoint contentOffset() const { return m_dataSource->offset(); }

  void scrollToContentPoint(KDPoint p, bool allowOverscroll = false);
  void scrollToContentRect(KDRect rect, bool allowOverscroll = false); // Minimal scrolling to make this rect visible
protected:
  KDCoordinate maxContentWidthDisplayableWithoutScrolling() const {
    return m_frame.width() - m_leftMargin - m_rightMargin;
  }
  KDCoordinate maxContentHeightDisplayableWithoutScrolling() const {
    return m_frame.height() - m_topMargin - m_bottomMargin;
  }
  KDRect visibleContentRect();
  void layoutSubviews() override;
  virtual KDSize contentSize() const { return m_contentView->minimalSizeForOptimalDisplay(); }
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
  View * m_contentView;
private:
  ScrollViewDataSource * m_dataSource;
  int numberOfSubviews() const override { return 1 + const_cast<ScrollView *>(this)->decorator()->numberOfIndicators(); }
  View * subviewAtIndex(int index) override { return (index == 0) ? &m_innerView : decorator()->indicatorAtIndex(index); }

  class InnerView : public View {
  public:
    InnerView(ScrollView * scrollView) : View(), m_scrollView(scrollView) {}
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override {
      assert(index == 0);
      return m_scrollView->m_contentView;
    }
    const ScrollView * m_scrollView;
  };

  KDCoordinate m_topMargin;
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;

  InnerView m_innerView;
  Decorator::Type m_decoratorType;
  Decorator m_decorator;
  BarDecorator m_barDecorator;
  ArrowDecorator m_arrowDecorator;
  KDColor m_backgroundColor;
};

#endif
