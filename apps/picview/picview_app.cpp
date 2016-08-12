#include "picview_app.h"

PicViewApp::PicViewApp() :
  App(),
  m_picViewController(PicViewController(this))
{
}

ViewController * PicViewApp::rootViewController() {
  return &m_picViewController;
}
