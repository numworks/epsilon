#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include <escher.h>
#include "main_controller.h"

namespace Settings {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    const char * uriName() override;
    const I18n::Message *name() override;
    const I18n::Message *upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
private:
  App(Container * container, Snapshot * snapshot);
  MainController m_mainController;
  StackViewController m_stackViewController;
};

}

#endif
