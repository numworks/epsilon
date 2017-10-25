#ifndef CODE_APP_H
#define CODE_APP_H

#include <escher.h>
#include "../shared/message_controller.h"
#include "menu_controller.h"
#include "script_store.h"

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
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    ScriptStore * scriptStore();
  private:
    ScriptStore m_scriptStore;
  };
  StackViewController * stackViewController() { return &m_codeStackViewController; }
private:
  App(Container * container, Snapshot * snapshot);
  ButtonRowController m_listFooter;
  MenuController m_menuController;
  StackViewController m_codeStackViewController;
};

}

#endif
