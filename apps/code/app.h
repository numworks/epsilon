#ifndef CODE_APP_H
#define CODE_APP_H

#include <escher.h>
#include <ion/events.h>
#include "../shared/message_controller.h"
#include "menu_controller.h"
#include "script_store.h"
#include "toolbox.h"
#include "variable_box_controller.h"

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
  VariableBoxController * scriptsVariableBoxController() { return &m_variableBoxController; }
  Toolbox * pythonToolbox() { return &m_toolbox; }
  Toolbox::Action toolboxActionForTextArea() { return m_toolboxActionForTextArea; }
  Toolbox::Action toolboxActionForTextField() { return m_toolboxActionForTextField; }
private:
  App(Container * container, Snapshot * snapshot);
  ButtonRowController m_listFooter;
  MenuController m_menuController;
  StackViewController m_codeStackViewController;
  Toolbox m_toolbox;
  Toolbox::Action m_toolboxActionForTextArea;
  Toolbox::Action m_toolboxActionForTextField;
  VariableBoxController m_variableBoxController;
};

}

#endif
