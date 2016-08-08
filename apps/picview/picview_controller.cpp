#include "picview_controller.h"

PicViewController::PicViewController() :
  ViewController(),
  m_view(PicView())
{
}

View * PicViewController::view() {
  return &m_view;
}

/*

const char * PicViewController::title() const {
  return "PicView";
}
*/
