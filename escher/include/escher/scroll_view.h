#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>
#include <escher/scroll_view_indicator.h>
#include <escher/palette.h>

class ScrollView : public View {
public:
  ScrollView(View * contentView, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0, bool showIndicators = true,
    bool colorBackground = true, KDColor backgroundColor = Palette::WallScreen, KDCoordinate indicatorThickness = 10,
    KDColor indicatorColor = Palette::GreyDark, KDColor backgroundIndicatorColor = Palette::GreyMiddle,
    KDCoordinate indicatorMargin = 14);

  void setContentOffset(KDPoint offset);
  KDPoint contentOffset();
  KDCoordinate topMargin() const;
  KDCoordinate leftMargin() const;
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  KDCoordinate maxContentWidthDisplayableWithoutScrolling();
  KDCoordinate maxContentHeightDisplayableWithoutScrolling();
  void layoutSubviews() override;
  void updateScrollIndicator();
  KDCoordinate m_topMargin;
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
private:
  KDPoint m_offset;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;

  View * m_contentView;
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
