#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>
#include <escher/palette.h>
#include <escher/scroll_view_data_source.h>
#include <escher/scroll_view_indicator.h>

class ScrollView : public View {
public:
  ScrollView(View * contentView, ScrollViewDataSource * dataSource, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0, bool showIndicators = true,
    bool colorBackground = true, KDColor backgroundColor = Palette::WallScreen, KDCoordinate indicatorThickness = 20,
    KDColor indicatorColor = Palette::GreyDark, KDColor backgroundIndicatorColor = Palette::GreyMiddle,
    KDCoordinate indicatorMargin = 14);

  void setContentOffset(KDPoint offset, bool forceRelayout = false);
  KDPoint contentOffset() const;
  KDCoordinate topMargin() const;
  KDCoordinate leftMargin() const;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void scrollToContentPoint(KDPoint p, bool allowOverscroll = false);
  void scrollToContentRect(KDRect rect, bool allowOverscroll = false); // Minimal scrolling to make this rect visible
protected:
  KDCoordinate maxContentWidthDisplayableWithoutScrolling();
  KDCoordinate maxContentHeightDisplayableWithoutScrolling();
  KDRect visibleContentRect();
  void layoutSubviews() override;
  void updateScrollIndicator();
  KDSize contentSize();
  KDCoordinate m_topMargin;
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
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;
  KDCoordinate m_indicatorThickness;
  bool m_showIndicators;
  bool m_colorBackground;
  KDColor m_backgroundColor;
};

#endif
