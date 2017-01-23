#ifndef APPS_TITLE_BAR_VIEW_H
#define APPS_TITLE_BAR_VIEW_H

#include <escher.h>

class TitleBarView : public View {
public:
  TitleBarView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setTitle(const char * title);
private:
  int numberOfSubviews() const override;
  void layoutSubviews() override;
  View * subviewAtIndex(int index) override;
  PointerTextView m_titleView;
};

#endif
