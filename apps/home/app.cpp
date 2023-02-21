#include "app.h"

#include <apps/i18n.h>

extern "C" {
#include <assert.h>
}

namespace Home {

I18n::Message App::Descriptor::name() const { return I18n::Message::Apps; }

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::AppsCapital;
}

App* App::Snapshot::unpack(Escher::Container* container) {
  return new (container->currentAppBuffer()) App(this);
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot* snapshot)
    : Escher::App(snapshot, &m_controller, I18n::Message::Warning),
      m_controller(&m_modalViewController, snapshot) {}

}  // namespace Home
