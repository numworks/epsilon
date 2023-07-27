#include "app.h"

#include <apps/apps_container.h>
#include <apps/elements/elements_icon.h>
#include <apps/i18n.h>
#include <escher/palette.h>

#include "elements_data_base.h"

namespace Elements {

// App::Descriptor

I18n::Message App::Descriptor::name() const {
  return I18n::Message::ElementsApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::ElementsAppCapital;
}

const Escher::Image* App::Descriptor::icon() const {
  return ImageStore::ElementsIcon;
}

// App::Snapshot

App* App::Snapshot::unpack(Escher::Container* container) {
  return new (container->currentAppBuffer()) App(this);
}

const App::Descriptor* App::Snapshot::descriptor() const {
  constexpr static Descriptor s_descriptor;
  return &s_descriptor;
}

void App::Snapshot::reset() {
  Shared::SharedApp::Snapshot::reset();
  init();
}

void App::Snapshot::init() {
  m_field = &ElementsDataBase::GroupField;
  m_selectedElement = 1;
  m_previousElement = ElementsDataBase::k_noElement;
}

// App
App::App(Snapshot* snapshot)
    : Shared::AppWithStoreMenu(snapshot, &m_stackController),
      m_stackController(&m_modalViewController, &m_mainController,
                        Escher::StackViewController::Style::WhiteUniform),
      m_mainController(&m_stackController),
      m_dataSource(&m_mainController) {}

}  // namespace Elements
