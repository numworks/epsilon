#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>
#include <escher/scroll_view_indicator.h>

class ScrollView : public View {
public:
  ScrollView(View * contentView, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0,
    KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);

  void setContentOffset(KDPoint offset);
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  KDCoordinate maxContentWidthDisplayableWithoutScrolling();
  KDCoordinate maxContentHeightDisplayableWithoutScrolling();
  void layoutSubviews() override;
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
private:
  constexpr static KDCoordinate k_indicatorThickness = 10;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;

  KDPoint m_offset;
  View * m_contentView;
  ScrollViewIndicator m_verticalScrollIndicator;
  KDCoordinate m_topMargin;
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;
};

#endif
