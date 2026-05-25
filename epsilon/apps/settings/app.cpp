#include "app.h"

#include <apps/i18n.h>
#include <apps/icons/settings_icon.h>
#include <escher/transparent_themed_image_view.h>

using namespace Escher;

namespace Settings {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::SettingsApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::SettingsAppCapital;
}

const Image* App::Descriptor::icon() const {
  return Escher::TransparentThemedImageView(*ImageStore::SettingsIcon)
      .currentImage();
}

App* App::Snapshot::unpack(Container* container) {
  return new (container->currentAppBuffer()) App(this);
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot* snapshot)
    : Shared::SharedApp(snapshot, &m_stackViewController),
      m_mainController(&m_stackViewController),
      m_stackViewController(&m_modalViewController, &m_mainController,
                            Escher::StackViewController::Style::WhiteUniform) {}

}  // namespace Settings
