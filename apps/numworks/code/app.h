#ifndef CODE_APP_H
#define CODE_APP_H

#include <escher.h>
#include "menu_controller.h"
#include "program.h"
#include "apps/shared/message_controller.h"

namespace Code {

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
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    Program * program();
  private:
    Program m_program;
  };
  void didBecomeActive(Window * window) override;
private:
  App(Container * container, Snapshot * snapshot);
  MessageController m_betaVersionController;
  MenuController m_menuController;
};

}

#endif
