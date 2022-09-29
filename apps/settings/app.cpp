#include "app.h"
#include "settings_icon.h"
#include <apps/i18n.h>

using namespace Escher;

namespace Settings {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::SettingsApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::SettingsAppCapital;
}

const Image * App::Descriptor::icon() const {
  return ImageStore::SettingsIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot * snapshot) :
  Shared::TextFieldDelegateApp(snapshot, &m_stackViewController),
  m_mainController(&m_stackViewController, this),
  m_stackViewController(&m_modalViewController, &m_mainController, Escher::StackViewController::Style::WhiteUniform)
{
}

bool App::handleEvent(Ion::Events::Event event) {
  // We don't want InputEventHandlerDelegateApp to open the store menu in here
  return false;
}

}
