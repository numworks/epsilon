#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>
#include <escher/scroll_view_indicator.h>

class ScrollView : public View {
public:
  ScrollView(View * contentView);

  void setContentOffset(KDPoint offset);
protected:
  KDCoordinate maxContentWidth();
  void layoutSubviews() override;
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;

  KDPoint m_offset;
  View * m_contentView;
  ScrollViewIndicator m_verticalScrollIndicator;
};

#endif
