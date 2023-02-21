#ifndef HOME_APP_H
#define HOME_APP_H

#include <escher/app.h>
#include <escher/container.h>
#include <escher/selectable_table_view_data_source.h>

#include "../shared/shared_app.h"
#include "controller.h"

namespace Home {

class App : public Escher::App {
 public:
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot,
                   public Escher::SelectableTableViewDataSource {
   public:
    App *unpack(Escher::Container *container) override;
    const Descriptor *descriptor() const override;
  };
  static App *app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Snapshot *snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }
  TELEMETRY_ID("Home");

 private:
  App(Snapshot *snapshot);
  Controller m_controller;
};

}  // namespace Home

#endif
