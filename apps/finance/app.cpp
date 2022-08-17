#include "app.h"
#include "../apps_container.h"
#include "finance_icon.h"
#include <apps/i18n.h>
#include <escher/palette.h>

namespace Finance {

// App::Descriptor

const Escher::Image * App::Descriptor::icon() const {
  // TODO
  return ImageStore::FinanceIcon;
}

// App::Snapshot

App * App::Snapshot::unpack(Escher::Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

const App::Descriptor * App::Snapshot::descriptor() const {
  constexpr static Descriptor s_descriptor;
  return &s_descriptor;
}

// App
App::App(Snapshot * snapshot) :
  Shared::TextFieldDelegateApp(snapshot, &m_stackViewController),
  m_resultController(&m_stackViewController, m_data.interestData()),
  m_interestController(&m_stackViewController, this, &m_resultController, m_data.interestData()),
  m_interestMenuController(&m_stackViewController, &m_interestController, m_data.interestData()),
  m_menuController(&m_stackViewController, &m_interestMenuController, m_data.interestData()),
  m_stackViewController(&m_modalViewController, &m_menuController, Escher::StackViewController::Style::GrayGradation)
{}

}