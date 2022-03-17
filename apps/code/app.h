#ifndef CODE_APP_H
#define CODE_APP_H

#include <escher.h>
#include <ion/events.h>
#include "../shared/input_event_handler_delegate_app.h"
#include "console_controller.h"
#include "menu_controller.h"
#include "script_store.h"
#include "python_toolbox.h"
#include "variable_box_controller.h"
#include "../shared/shared_app.h"

namespace Code {

class App : public Shared::InputEventHandlerDelegateApp {
public:
  class Descriptor : public Shared::InputEventHandlerDelegateApp::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    App::Descriptor::ExaminationLevel examinationLevel() override;
    const Image * icon() override;
  };
  class Snapshot : public SharedApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
    ScriptStore * scriptStore();
#if EPSILON_GETOPT
    bool lockOnConsole() const;
    void setOpt(const char * name, const char * value) override;
#endif
  private:
#if EPSILON_GETOPT
    bool m_lockOnConsole;
    bool m_hasBeenWiped;
#endif
    ScriptStore m_scriptStore;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  ~App();
  TELEMETRY_ID("Code");
  bool prepareForExit() override {
    if (m_consoleController.inputRunLoopActive()) {
      m_consoleController.terminateInputLoop();
      return false;
    }
    return true;
  }
  StackViewController * stackViewController() { return &m_codeStackViewController; }
  ConsoleController * consoleController() { return &m_consoleController; }
  MenuController * menuController() { return &m_menuController; }

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void willExitResponderChain(Responder * nextFirstResponder) override;

  /* InputEventHandlerDelegate */
  Toolbox * toolboxForInputEventHandler(InputEventHandler * textInput) override;
  VariableBoxController * variableBoxForInputEventHandler(InputEventHandler * textInput) override;

  /* TextInputDelegate */
  bool textInputDidReceiveEvent(InputEventHandler * textInput, Ion::Events::Event event);

  /* Code::App */
  // Python delegate
  bool pythonIsInited() { return m_pythonUser != nullptr; }
  bool isPythonUser(const void * pythonUser) { return m_pythonUser == pythonUser; }
  void initPythonWithUser(const void * pythonUser);
  void deinitPython();

  VariableBoxController * variableBoxController() { return &m_variableBoxController; }

  static constexpr int k_pythonHeapSize = 70000;

private:
  /* Python delegate:
   * MicroPython requires a heap. To avoid dynamic allocation, we keep a working
   * buffer here and we give to controllers that load Python environment. We
   * also memoize the last Python user to avoid re-initiating MicroPython when
   * unneeded. */
  char m_pythonHeap[k_pythonHeapSize];
  const void * m_pythonUser;

  App(Snapshot * snapshot);
  ConsoleController m_consoleController;
  ButtonRowController m_listFooter;
  MenuController m_menuController;
  StackViewController m_codeStackViewController;
  PythonToolbox m_toolbox;
  VariableBoxController m_variableBoxController;
};

}

#endif
