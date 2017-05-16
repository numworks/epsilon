#ifndef HOME_APP_H
#define HOME_APP_H

#include <escher.h>
#include "controller.h"

class AppsContainer;

namespace Home {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    App * build(Container * container) override;
    I18n::Message name() override;
    I18n::Message upperName() override;
  };
  static Descriptor * buildDescriptor();
private:
  App(Container * container, Descriptor * descriptor);
  Controller m_controller;
};

}

#endif
