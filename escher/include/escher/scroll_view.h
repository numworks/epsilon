#ifndef ESCHER_SCROLL_VIEW_H
#define ESCHER_SCROLL_VIEW_H

#include <escher/view.h>

class ScrollView : public View {
public:
  ScrollView(View * contentView);

  int numberOfSubviews() const override;
  const View * subview(int index) const override;
  void storeSubviewAtIndex(View * view, int index) override;
  void layoutSubviews() override;

  void setContentOffset(KDPoint offset);
protected:
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const;
  virtual void logAttributes(std::ostream &os) const;
#endif
private:
  KDPoint m_offset;
  View * m_contentView;
  //ScollIndicator m_verticalScrollIndicator;
};

#endif
