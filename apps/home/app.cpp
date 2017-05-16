#include "app.h"
#include "../apps_container.h"
#include "../i18n.h"

extern "C" {
#include <assert.h>
}

namespace Home {

App * App::Descriptor::build(Container * container) {
  return new App(container, this);
}

I18n::Message App::Descriptor::name() {
  return I18n::Message::Apps;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::AppsCapital;
}

App::App(Container * container, Descriptor * descriptor) :
  ::App(container, &m_controller, descriptor, I18n::Message::Warning),
  m_controller(&m_modalViewController, (AppsContainer *)container)
{
}

App::Descriptor * App::buildDescriptor() {
  return new App::Descriptor();
}

}
