#ifndef CODE_APP_H
#define CODE_APP_H

#include <escher.h>
#include <ion/events.h>
#include "code_icon.h"
#include "../shared/toolbox_helpers.h"
#include "../i18n.h"
#include "menu_controller.h"
#include "script_store.h"
#include "python_toolbox.h"
#include "variable_box_controller.h"

namespace Code {

class App final : public ::App {
public:
  class Descriptor final : public ::App::Descriptor {
  public:
    I18n::Message name() override {
      return I18n::Message::CodeApp;
    }
    I18n::Message upperName() override {
      return I18n::Message::CodeAppCapital;
    }
    const Image * icon() override {
      return ImageStore::CodeIcon;
    }
  };
  class Snapshot final : public ::App::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override { return new App(container, this); }
    void reset() override { m_scriptStore.deleteAllScripts(); }
    Descriptor * descriptor() override { return &s_descriptor; }
    ScriptStore * scriptStore() { return &m_scriptStore; }
#if EPSILON_GETOPT
    bool lockOnConsole() const;
    void setOpt(const char * name, char * value) override;
#endif
  private:
#if EPSILON_GETOPT
    bool m_lockOnConsole;
#endif
    ScriptStore m_scriptStore;
    static Descriptor s_descriptor;
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
