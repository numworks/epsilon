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
    I18n::Message name() override;
    I18n::Message upperName() override;
  };
  class Snapshot : public ::App::Snapshot, public SelectableTableViewDataSource {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
private:
  App(Container * container, Snapshot * snapshot);
  Controller m_controller;
};

}

#endif
