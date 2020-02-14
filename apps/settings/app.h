#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include "main_controller.h"
#include "../shared/text_field_delegate_app.h"

namespace Settings {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
  TELEMETRY_ID("Settings");
private:
  App(Snapshot * snapshot);
  MainController m_mainController;
  StackViewController m_stackViewController;
};

}

#endif
