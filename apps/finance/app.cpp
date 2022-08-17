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
  Shared::InputEventHandlerDelegateApp(snapshot, &m_inputViewController),
  m_financeResultController(&m_stackViewController, m_financeData.interestData()),
  m_interestController(&m_stackViewController, &m_inputViewController, &m_financeResultController, m_financeData.interestData()),
  m_interestMenuController(&m_stackViewController, &m_interestController, m_financeData.interestData()),
  m_financeMenuController(&m_stackViewController, &m_interestMenuController, m_financeData.interestData()),
  m_stackViewController(&m_inputViewController, &m_financeMenuController, Escher::StackViewController::Style::GrayGradation),
  m_inputViewController(&m_modalViewController, &m_stackViewController, this, &m_interestController, nullptr)
{}

}