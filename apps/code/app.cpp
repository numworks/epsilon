#include "app.h"

#include <apps/i18n.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/pool.h>

#include "clipboard.h"
#include "code_icon.h"
#include "helpers.h"

using namespace Escher;

namespace Code {

I18n::Message App::Descriptor::name() const { return I18n::Message::CodeApp; }

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::CodeAppCapital;
}

const Image *App::Descriptor::icon() const { return ImageStore::CodeIcon; }

App::Snapshot::Snapshot()
#if EPSILON_GETOPT
    : m_lockOnConsole(false)
#endif
{
  ScriptStore::InitTemplates();
}

App *App::Snapshot::unpack(Container *container) {
  return new (container->currentAppBuffer()) App(this);
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor *App::Snapshot::descriptor() const {
  return &sDescriptor;
}

#if EPSILON_GETOPT
bool App::Snapshot::lockOnConsole() const { return m_lockOnConsole; }

void App::Snapshot::setOpt(const char *name, const char *value) {
  if (strcmp(name, "script") == 0) {
    ScriptStore::DeleteAllScripts();
    char *separator =
        const_cast<char *>(UTF8Helper::CodePointSearch(value, ':'));
    if (*separator == 0) {
      return;
    }
    *separator = 0;
    const char *scriptName = value;
    const char *scriptContent = separator;
    Script::Create(scriptName, scriptContent + 1);
    return;
  }
  if (strcmp(name, "lock-on-console") == 0) {
    m_lockOnConsole = true;
    return;
  }
}
#endif

App::App(Snapshot *snapshot)
    : Shared::SharedApp(snapshot, &m_codeStackViewController),
      m_pythonUser(nullptr),
      m_consoleController(nullptr, this
#if EPSILON_GETOPT
                          ,
                          snapshot->lockOnConsole()
#endif
                              ),
      m_listFooter(&m_codeStackViewController, &m_menuController,
                   &m_menuController, ButtonRowController::Position::Bottom,
                   ButtonRowController::Style::EmbossedGray,
                   ButtonRowController::Size::Large),
      m_menuController(&m_listFooter, this, &m_listFooter),
      m_codeStackViewController(
          &m_modalViewController, &m_listFooter,
          Escher::StackViewController::Style::WhiteUniform),
      m_variableBox() {
  Clipboard::sharedClipboard()->enterPython();
}

bool App::quitInputRunLoop() {
  if (m_consoleController.inputRunLoopActive()) {
    m_consoleController.terminateInputLoop();
    m_modalViewController.dismissPotentialModal();
    Ion::USB::clearEnumerationInterrupt();
    return true;
  }
  return false;
}

App::~App() {
  quitInputRunLoop();
  deinitPython();
  Clipboard::sharedClipboard()->exitPython();
}

bool App::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::USBEnumeration
#if !PLATFORM_DEVICE
      /* On the simulator, pressing Home does not interrupt the execution,
       * so we must quit the run loop. */
      || event == Ion::Events::Home
#endif
  ) {
    if (quitInputRunLoop()) {
      /* We need to return true here because we want to actually exit from the
       * input run loop, which requires ending a dispatchEvent cycle. */
      return true;
    }
  }
  return false;
}

void App::willExitResponderChain(Responder *nextFirstResponder) {
  m_menuController.willExitApp();
}

bool App::textInputDidReceiveEvent(EditableField *textInput,
                                   Ion::Events::Event event) {
  const char *pythonText = Helpers::PythonTextForEvent(event);
  if (pythonText != nullptr) {
    textInput->handleEventWithText(pythonText);
    return true;
  }
  return false;
}

void App::initPythonWithUser(const void *pythonUser) {
  if (!m_pythonUser) {
    /* Tree pool will be used as an extension of the heap. */
    assert(Poincare::Pool::sharedPool->numberOfNodes() == 0);
    Poincare::Pool::sharedPool.deinit();

    char *heap = pythonHeap();
    MicroPython::init(heap, heap + k_pythonHeapSize);
  }
  m_pythonUser = pythonUser;
}

void App::deinitPython() {
  if (m_pythonUser) {
    MicroPython::deinit();
    m_pythonUser = nullptr;
    /* Re-construct the tree pool, which might have been ovewritten by the heap.
     */
    Poincare::Pool::sharedPool.init();
  }
}

}  // namespace Code
