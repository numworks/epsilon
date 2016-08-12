#ifndef PICVIEW_PICVIEW_CONTROLLER_H
#define PICVIEW_PICVIEW_CONTROLLER_H

#include <escher.h>
#include "pic_view.h"

class PicViewController : public ViewController {
public:
  PicViewController(Responder * parentResponder);
  View * view() override;
private:
  PicView m_view;
};

#endif
