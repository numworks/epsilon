#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>
#include <escher/scroll_view_data_source.h>
#include <escher/scroll_view_indicator.h>

class ScrollView : public View {
public:
  ScrollView(View * contentView, ScrollViewDataSource * dataSource);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  virtual void setTopMargin(KDCoordinate m) { m_topMargin = m; }
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

  void setShowsIndicators(bool s) { m_showsIndicators = s; }
  bool showsIndicators() const { return m_showsIndicators; }
  void setColorsBackground(bool c) { m_colorsBackground = c; }
  bool colorsBackground() const { return m_colorsBackground; }
  virtual void setBackgroundColor(KDColor c) { m_backgroundColor = c; }
  KDColor backgroundColor() const { return m_backgroundColor; }

  ScrollViewIndicator * verticalScrollIndicator() { return &m_verticalScrollIndicator; }
  ScrollViewIndicator * horizontalScrollIndicator() { return &m_horizontalScrollIndicator; }
  void setIndicatorThickness(KDCoordinate t) { m_indicatorThickness = t; }
  KDCoordinate indicatorThickness() const { return m_indicatorThickness; }

  void setContentOffset(KDPoint offset, bool forceRelayout = false);
  KDPoint contentOffset() const { return m_dataSource->offset(); }

  void scrollToContentPoint(KDPoint p, bool allowOverscroll = false);
  void scrollToContentRect(KDRect rect, bool allowOverscroll = false); // Minimal scrolling to make this rect visible
protected:
  KDCoordinate maxContentWidthDisplayableWithoutScrolling();
  KDCoordinate maxContentHeightDisplayableWithoutScrolling();
  KDRect visibleContentRect();
  void layoutSubviews() override;
  void updateScrollIndicator();
  KDSize contentSize();
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
  View * m_contentView;
private:
  ScrollViewDataSource * m_dataSource;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;

  ScrollViewIndicator m_verticalScrollIndicator;
  ScrollViewIndicator m_horizontalScrollIndicator;
  bool hasVerticalIndicator() const;
  bool hasHorizontalIndicator() const;
  KDCoordinate m_topMargin;
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;
  KDCoordinate m_indicatorThickness;
  bool m_showsIndicators;
  bool m_colorsBackground;
  KDColor m_backgroundColor;
};

#endif
