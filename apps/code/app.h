#ifndef CODE_APP_H
#define CODE_APP_H

#include <escher.h>
#include <ion/events.h>
#include "../shared/message_controller.h"
#include "menu_controller.h"
#include "script_store.h"
#include "python_toolbox.h"
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
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    ScriptStore * scriptStore();
#if EPSILON_GETOPT
    bool lockOnConsole() const;
    void setOpt(const char * name, char * value) override;
#endif
  private:
#if EPSILON_GETOPT
    bool m_lockOnConsole;
#endif
    ScriptStore m_scriptStore;
  };
  StackViewController * stackViewController() { return &m_codeStackViewController; }
  PythonToolbox * pythonToolbox() { return &m_toolbox; }
  bool textInputDidReceiveEvent(TextInput * textInput, Ion::Events::Event event);
private:
  App(Container * container, Snapshot * snapshot);
  ButtonRowController m_listFooter;
  MenuController m_menuController;
  StackViewController m_codeStackViewController;
  PythonToolbox m_toolbox;
  VariableBoxController m_variableBoxController;
};

}

#endif
