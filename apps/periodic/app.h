#ifndef PERIODIC_APP_H
#define PERIODIC_APP_H

#include "elements_view_data_source.h"
#include "main_controller.h"
#include <apps/shared/text_field_delegate_app.h>
#include <apps/shared/shared_app.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>

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

  static App * app() { return static_cast<App *>(Escher::Container::activeApp()); }

  ElementsViewDataSource * elementsViewDataSource() const { return static_cast<Snapshot *>(snapshot())->elementsViewDataSource(); }

private:
  App(Snapshot * snapshot);

  Escher::StackViewController m_stackController;
  MainController m_mainController;
};

}

#endif
