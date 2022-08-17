#ifndef FINANCE_APP_H
#define FINANCE_APP_H

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>
#include "../shared/text_field_delegate_app.h"
#include "../shared/shared_app.h"
#include "data.h"
#include "menu_controller.h"
#include "interest_controller.h"
#include "interest_menu_controller.h"
#include "result_controller.h"

namespace Finance {

class App : public Shared::TextFieldDelegateApp {
public:
  // Descriptor
  class Descriptor : public Shared::TextFieldDelegateApp::Descriptor {
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
  Data m_data;
  ResultController m_resultController;
  InterestController m_interestController;
  InterestMenuController m_interestMenuController;
  MenuController m_menuController;
  Escher::StackViewController m_stackViewController;
};

}

#endif
