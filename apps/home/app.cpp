#include "app.h"
#include "../i18n.h"
#include "../apps_container.h"

extern "C" {
#include <assert.h>
}

namespace Home {

I18n::Message App::Descriptor::name() {
  return I18n::Message::Apps;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::AppsCapital;
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_controller, I18n::Message::Warning),
  m_controller(&m_modalViewController, (AppsContainer *)container, snapshot)
{
}

}
