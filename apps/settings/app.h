#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include <escher.h>
#include "main_controller.h"

namespace Settings {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    App * build(Container * container) override;
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  static Descriptor * buildDescriptor();
private:
  App(Container * container, Descriptor * descriptor);
  MainController m_mainController;
  StackViewController m_stackViewController;
};

}

#endif
