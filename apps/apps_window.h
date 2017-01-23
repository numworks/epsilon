#ifndef APPS_WINDOW_H
#define APPS_WINDOW_H

#include <escher.h>

class AppsWindow : public Window {
public:
  AppsWindow();
private:
  class TitleBarView : public View {
    void drawRect(KDContext * ctx, KDRect rect) const override;
  };
  constexpr static KDCoordinate k_titleBarHeight = 18;
  int numberOfSubviews() const override;
  void layoutSubviews() override;
  View * subviewAtIndex(int index) override;
  TitleBarView m_titleBarView;
};

#endif
