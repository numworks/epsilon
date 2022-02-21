#include <escher/alternate_view_controller.h>
#include <escher/container.h>
#include <assert.h>

namespace Escher {

/* AlternateViewController */

AlternateViewController::AlternateViewController(Responder * parentResponder, AlternateViewDelegate * delegate, ViewController ** viewControllers, const char * title) :
  ViewController(parentResponder),
  m_delegate(delegate),
  m_viewControllers(viewControllers),
  m_title(title)
{}

void AlternateViewController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(activeViewController());
}

void AlternateViewController::viewWillAppear() {
  activeViewController()->viewWillAppear();
}

}
