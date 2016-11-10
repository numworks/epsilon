#ifndef PICVIEW_PICVIEW_APP_H
#define PICVIEW_PICVIEW_APP_H

#include <escher.h>
#include "picview_controller.h"

class PicViewApp : public App {
public:
  PicViewApp(Container * container);
private:
  PicViewController m_picViewController;
};

#endif
