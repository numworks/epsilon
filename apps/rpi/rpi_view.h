#ifndef APPS_RPI_VIEW_H
#define APPS_RPI_VIEW_H

#include <escher.h>

namespace Rpi {

class RpiView : public View {
public:
  RpiView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}

#endif
