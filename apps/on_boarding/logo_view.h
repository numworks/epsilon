#ifndef APPS_LOGO_VIEW_H
#define APPS_LOGO_VIEW_H

#include <escher/image_view.h>
#include <escher/view.h>
#include <ion.h>

namespace OnBoarding {

class LogoView : public Escher::View {
 public:
  LogoView();
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  int numberOfSubviews() const override;
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  Escher::ImageView m_logoView;
};

}  // namespace OnBoarding

#endif
