#include "picview_controller.h"

PicViewController::PicViewController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(PicView())
{
}

View * PicViewController::view() {
  return &m_view;
}

/*

const char * PicViewController::title() {
  return "PicView";
}
*/
