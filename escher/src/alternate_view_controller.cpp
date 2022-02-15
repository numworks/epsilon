#include <escher/alternate_view_controller.h>
#include <escher/container.h>
#include <assert.h>

namespace Escher {

/* AlternateViewController */

AlternateViewController::AlternateViewController(Responder * parentResponder, AlternateViewDelegate * delegate, const char * title) :
  ViewController(parentResponder),
  m_contentView(delegate),
  m_title(title)
{}

void AlternateViewController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(m_contentView.mainViewController());
}

void AlternateViewController::viewWillAppear() {
  m_contentView.layoutSubviews();
  m_contentView.mainViewController()->viewWillAppear();
}

}
