#ifndef HOME_APP_H
#define HOME_APP_H

#include <escher.h>
#include "controller.h"
#include "../shared/shared_app.h"

namespace Home {

class App : public Escher::App {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot, public Escher::SelectableTableViewDataSource {
  public:
    App * unpack(Escher::Container * container) override;
    Descriptor * descriptor() override;
  };
  static App * app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }
  TELEMETRY_ID("Home");
private:
  App(Snapshot * snapshot);
  Controller m_controller;
};

}

#endif
