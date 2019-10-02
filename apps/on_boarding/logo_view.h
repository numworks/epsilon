#ifndef APPS_LOGO_VIEW_H
#define APPS_LOGO_VIEW_H

#include <escher.h>

namespace OnBoarding {

class LogoView : public View {
public:
  LogoView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ImageView m_logoView;
};

}

#endif

