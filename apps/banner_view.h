#ifndef APPS_BANNER_VIEW_H
#define APPS_BANNER_VIEW_H

#include <escher.h>

class BannerView : public View {
public:
  virtual void reload() = 0;
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

#endif
