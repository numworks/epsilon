#ifndef CODE_APP_H
#define CODE_APP_H

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
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image * icon() const override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Escher::Container * container) override;
    const Descriptor * descriptor() const override;
    ScriptStore * scriptStore();
#if EPSILON_GETOPT
    bool lockOnConsole() const;
    void setOpt(const char * name, const char * value) override;
#endif
  private:
#if EPSILON_GETOPT
    bool m_lockOnConsole;
#endif
    ScriptStore m_scriptStore;
  };
  static App * app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  ~App();
  TELEMETRY_ID("Code");
  Escher::StackViewController * stackViewController() { return &m_codeStackViewController; }
  ConsoleController * consoleController() { return &m_consoleController; }
  MenuController * menuController() { return &m_menuController; }

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;

  /* InputEventHandlerDelegate */
  PythonToolbox * toolbox() override { return &m_toolbox; }
  VariableBoxController * variableBox() override { return &m_variableBoxController; }

  /* TextInputDelegate */
  bool textInputDidReceiveEvent(Escher::InputEventHandler * textInput, Ion::Events::Event event);

  /* Code::App */
  // Python delegate
  bool pythonIsInited() { return m_pythonUser != nullptr; }
  bool isPythonUser(const void * pythonUser) { return m_pythonUser == pythonUser; }
  void initPythonWithUser(const void * pythonUser);
  void deinitPython();

  constexpr static size_t k_pythonHeapSize = 65536; // 64KiB

private:
  App(Snapshot * snapshot);

  /* Python delegate:
   * MicroPython requires a heap. To avoid dynamic allocation, we keep a working
   * buffer here and we give to controllers that load Python environment. We
   * also memoize the last Python user to avoid re-initiating MicroPython when
   * unneeded. */
  const void * m_pythonUser;
  ConsoleController m_consoleController;
  Escher::ButtonRowController m_listFooter;
  MenuController m_menuController;
  Escher::StackViewController m_codeStackViewController;
  PythonToolbox m_toolbox;
  VariableBoxController m_variableBoxController;
#if PLATFORM_DEVICE
  /* On the device, we reach 64K of heap by repurposing the unused tree pool.
   * The linker must make sure that the pool and the apps buffer are
   * contiguous. */
  char * pythonHeap() {
    /* The python heap will be made of the pool plus an additional buffer to
     * reach 64KiB. We declare the buffer with the length necessary to reach
     * this size, but in practice the heap will use more space in the Apps
     * buffer if Code is not the largest app.
     * We must also make sure in the linker script that the pool is located
     * right after the Apps buffer. */
    assert(Poincare::TreePool::sharedPool()->numberOfNodes() == 0);
    assert(static_cast<void *>(m_pythonHeap + k_pythonHeapSize) <= static_cast<void *>(Poincare::TreePool::sharedPool() + sizeof(Poincare::TreePool)));
    return m_pythonHeap;
  }
  constexpr static int k_pythonHeapExtensionSize = k_pythonHeapSize - sizeof(Poincare::TreePool);
  char m_pythonHeap[k_pythonHeapExtensionSize];
#else
  char * pythonHeap() { return m_pythonHeap; }
  char m_pythonHeap[k_pythonHeapSize];
#endif
};

}

#endif
