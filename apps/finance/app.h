#ifndef FINANCE_APP_H
#define FINANCE_APP_H

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>
#include <escher/input_view_controller.h>
#include "../shared/input_event_handler_delegate_app.h"
#include "../shared/shared_app.h"
#include "data.h"
#include "finance_menu_controller.h"
#include "interest_controller.h"
#include "interest_menu_controller.h"
#include "finance_result_controller.h"

namespace Finance {

class App : public Shared::InputEventHandlerDelegateApp {
public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
    public:
      I18n::Message name() const override { return I18n::Message::FinanceApp; };
      I18n::Message upperName() const override { return I18n::Message::FinanceAppCapital; };
      const Escher::Image * icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    App * unpack(Escher::Container * container) override;
    const Descriptor * descriptor() const override;
  };
  TELEMETRY_ID("Finance");

  static App * app() { return static_cast<App *>(Escher::Container::activeApp()); }

private:
  App(Snapshot * snapshot);

  // Controllers
  FinanceData m_financeData;
  FinanceResultController m_financeResultController;
  InterestController m_interestController;
  InterestMenuController m_interestMenuController;
  FinanceMenuController m_financeMenuController;
  Escher::StackViewController m_stackViewController;
  Escher::InputViewController m_inputViewController;
};

}

#endif
