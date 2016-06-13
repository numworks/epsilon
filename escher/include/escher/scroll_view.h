#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>
#include <escher/scroll_view_indicator.h>

class ScrollView : public View {
public:
  ScrollView(View * contentView);

  int numberOfSubviews() const override;
  View * subview(int index) override;
  void storeSubviewAtIndex(View * view, int index) override;
  void layoutSubviews() override;

  void setContentOffset(KDPoint offset);
protected:
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const override;
  virtual void logAttributes(std::ostream &os) const override;
#endif
private:
  void setContentViewOrigin();
  KDPoint m_offset;
  View * m_contentView;
  ScrollViewIndicator m_verticalScrollIndicator;
};

#endif
