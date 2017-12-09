#ifndef HOME_APP_H
#define HOME_APP_H

#include <escher.h>
#include "../i18n.h"
#include "controller.h"

class AppsContainer;

namespace Home {

class App final : public ::App {
public:
  class Descriptor final : public ::App::Descriptor {
  public:
    I18n::Message name() override {
      return I18n::Message::Apps;
    }
    I18n::Message upperName() override {
      return I18n::Message::AppsCapital;
    }
  };
  class Snapshot final : public ::App::Snapshot, public SelectableTableViewDataSource {
  public:
    App * unpack(Container * container) override {
      return new App(container, this);
    }
    Descriptor * descriptor() override {
      return &s_descriptor;
    }
  private:
    static Descriptor s_descriptor;
  };
private:
  App(Container * container, Snapshot * snapshot) :
    ::App(container, snapshot, &m_controller, I18n::Message::Warning),
    m_controller(&m_modalViewController, (AppsContainer *)container, snapshot) {}
  Controller m_controller;
};

}

#endif
