#ifndef CODE_APP_H
#define CODE_APP_H

#include <escher.h>
#include "menu_controller.h"
#include "program.h"
#include "../shared/message_controller.h"

namespace Code {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    Program * program();
  private:
    Program m_program;
  };
private:
  App(Container * container, Snapshot * snapshot);
  MenuController m_menuController;
};

}

#endif
