#ifndef PERIODIC_APP_H
#define PERIODIC_APP_H

#include "elements_view_data_source.h"
#include "main_controller.h"
#include "../shared/text_field_delegate_app.h"
#include "../shared/shared_app.h"

namespace Periodic {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image * icon() const override;
  };

  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    App * unpack(Escher::Container * container) override;
    const Descriptor * descriptor() const override;
    ElementsViewDataSource * elementsViewDataSource() { return &m_dataSource; }

  private:
    ElementsViewDataSource m_dataSource;
  };
  TELEMETRY_ID("PeriodicTable");

private:
  App(Snapshot * snapshot);

  MainController m_mainController;
};

}

#endif
