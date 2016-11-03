#include "picview_app.h"

PicViewApp::PicViewApp() :
  ::App(&m_picViewController),
  m_picViewController(PicViewController(&m_modalViewController))
{
}
