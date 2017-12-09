#ifndef PICVIEW_PICVIEW_CONTROLLER_H
#define PICVIEW_PICVIEW_CONTROLLER_H

#include <escher.h>
#include "pic_view.h"

class PicViewController final : public ViewController {
public:
  PicViewController(Responder * parentResponder) : ViewController(parentResponder), m_view(PicView()) {}
  View * view() override {
    return &m_view;
  }
private:
  PicView m_view;
};

#endif
