#include <escher/alternate_view_controller.h>
#include <escher/container.h>
#include <assert.h>

namespace Escher {

/* AlternateViewController */

AlternateViewController::AlternateViewController(Responder * parentResponder, AlternateViewDelegate * delegate, const char * title) :
  ViewController(parentResponder),
  m_delegate(delegate),
  m_title(title)
{}

void AlternateViewController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(m_delegate->activeViewController());
}

void AlternateViewController::viewWillAppear() {
  m_delegate->activeViewController()->viewWillAppear();
}

}
