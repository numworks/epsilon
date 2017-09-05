#include "picview_app.h"

PicViewApp::PicViewApp(Container * container) :
  ::App(container, &m_picViewController),
  m_picViewController(PicViewController(&m_modalViewController))
{
}
