#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>
#include <escher/metric.h>
#include <escher/scroll_view_data_source.h>
#include <escher/scroll_view_indicator.h>

namespace Escher {

class ScrollView : public View {

/* TODO: Should we add a reload method that forces the relayouting of the
 * subviews? Or should ScrollView::setFrame always force the layouting of the
 * subviews ? Because the scroll view frame might not change but its content
 * might need to be relayouted.
 * cf TableView, InputViewController, EditExpressionController. */

public:
  ScrollView(View * contentView, ScrollViewDataSource * dataSource);
  ScrollView(ScrollView&& other);
  KDSize minimalSizeForOptimalDisplay() const override;

  void setTopMargin(KDCoordinate m) { m_topMargin = m; }
  void setRightMargin(KDCoordinate m) { m_rightMargin = m; }
  void setBottomMargin(KDCoordinate m) { m_bottomMargin = m; }
  void setLeftMargin(KDCoordinate m) { m_leftMargin = m; }
  KDCoordinate topMargin() const { return m_topMargin; }
  KDCoordinate rightMargin() const { return m_rightMargin; }
  KDCoordinate bottomMargin() const { return m_bottomMargin; }
  KDCoordinate leftMargin() const { return m_leftMargin; }

  void setMargins(KDCoordinate top, KDCoordinate right, KDCoordinate bottom, KDCoordinate left);
  void setMargins(KDCoordinate m) { setMargins(m, m, m, m); }

  class Decorator {
  public:
    enum class Type {
      None,
      Bars,
      Arrows
    };
    /* We want (Decorator *)->~Decorator() to call ~BarDecorator() or ~ArrowDecorator()
     * when required. */
    virtual ~Decorator() = default;
    virtual int numberOfIndicators() const { return 0; }
    virtual View * indicatorAtIndex(int index) { assert(false); return nullptr; }
    virtual KDRect layoutIndicators(KDSize content, KDPoint offset, KDRect frame, KDRect * dirtyRect1, KDRect * dirtyRect2, bool force, ScrollViewDelegate * delegate = nullptr) { return frame; }
    virtual void setBackgroundColor(KDColor c) {}
    virtual void setVerticalMargins(KDCoordinate top, KDCoordinate bottom) {}
    virtual void setFont(KDFont::Size font) {}
  };

  class BarDecorator : public Decorator {
  public:
    constexpr static KDCoordinate k_barsFrameBreadth = Metric::CommonRightMargin;
    using Decorator::Decorator;
    void setVerticalMargins(KDCoordinate top, KDCoordinate bottom) override { m_verticalBar.setMargins(top, bottom); }
    int numberOfIndicators() const override { return 2; }
    View * indicatorAtIndex(int index) override;
    KDRect layoutIndicators(KDSize content, KDPoint offset, KDRect frame, KDRect * dirtyRect1, KDRect * dirtyRect2, bool force, ScrollViewDelegate * delegate = nullptr) override;
    ScrollViewVerticalBar * verticalBar() { return &m_verticalBar; }
    ScrollViewHorizontalBar * horizontalBar() { return &m_horizontalBar; }
  private:
    ScrollViewVerticalBar m_verticalBar;
    ScrollViewHorizontalBar m_horizontalBar;
  };

  class ArrowDecorator : public Decorator {
  public:
    ArrowDecorator() :
      m_topArrow(ScrollViewArrow::Side::Top),
      m_rightArrow(ScrollViewArrow::Side::Right),
      m_bottomArrow(ScrollViewArrow::Side::Bottom),
      m_leftArrow(ScrollViewArrow::Side::Left)
    {}
    int numberOfIndicators() const override { return 4; }
    View * indicatorAtIndex(int index) override;
    KDRect layoutIndicators(KDSize content, KDPoint offset, KDRect frame, KDRect * dirtyRect1, KDRect * dirtyRect2, bool force, ScrollViewDelegate * delegate = nullptr) override;
    void setBackgroundColor(KDColor c) override;
    void setFont(KDFont::Size font) override;
  private:
    ScrollViewArrow m_topArrow;
    ScrollViewArrow m_rightArrow;
    ScrollViewArrow m_bottomArrow;
    ScrollViewArrow m_leftArrow;
  };

  Decorator * decorator() { return m_decorators.activeDecorator(); }
  void setDecoratorType(Decorator::Type t) {
    m_decoratorType = t;
    m_decorators.setActiveDecorator(t);
  }
  void setDecoratorFont(KDFont::Size font) {
    m_decorators.activeDecorator()->setFont(font);
  }
  virtual void setBackgroundColor(KDColor c) {
    m_backgroundColor = c;
    decorator()->setBackgroundColor(m_backgroundColor);
  }
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
  void layoutSubviews(bool force = false) override;
  virtual KDSize contentSize() const { return m_contentView->minimalSizeForOptimalDisplay(); }
  virtual float marginPortionTolerance() const { return 0.8f; }
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
  void logAttributes(std::ostream &os) const override;
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
  mutable KDCoordinate m_excessWidth;
  mutable KDCoordinate m_excessHeight;

  InnerView m_innerView;
  Decorator::Type m_decoratorType;
  union Decorators {
  public:
    Decorators();
    ~Decorators();
    Decorator * activeDecorator() { return &m_none; }
    void setActiveDecorator(Decorator::Type t);
  private:
    Decorator m_none;
    BarDecorator m_bars;
    ArrowDecorator m_arrows;
  };
  Decorators m_decorators;
  KDColor m_backgroundColor;
};

}
#endif
