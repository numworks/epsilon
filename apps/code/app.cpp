#include "app.h"
#include "clipboard.h"
#include "code_icon.h"
#include <apps/i18n.h>
#include "helpers.h"
#include <ion/unicode/utf8_helper.h>

using namespace Escher;

namespace Code {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::CodeApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::CodeAppCapital;
}

const Image * App::Descriptor::icon() const {
  return ImageStore::CodeIcon;
}

App::Snapshot::Snapshot()
#if EPSILON_GETOPT
  : m_lockOnConsole(false)
#endif
{
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

static constexpr App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

ScriptStore * App::Snapshot::scriptStore() {
  return &m_scriptStore;
}

#if EPSILON_GETOPT
bool App::Snapshot::lockOnConsole() const {
  return m_lockOnConsole;
}

void App::Snapshot::setOpt(const char * name, const char * value) {
  if (strcmp(name, "script") == 0) {
    m_scriptStore.deleteAllScripts();
    char * separator = const_cast<char *>(UTF8Helper::CodePointSearch(value, ':'));
    if (*separator == 0) {
      return;
    }
    *separator = 0;
    const char * scriptName = value;
    const char * scriptContent = separator;
    Script::Create(scriptName, scriptContent + 1);
    return;
  }
  if (strcmp(name, "lock-on-console") == 0) {
    m_lockOnConsole = true;
    return;
  }
}
#endif

App::App(Snapshot * snapshot) :
  Shared::InputEventHandlerDelegateApp(snapshot, &m_codeStackViewController),
  m_pythonUser(nullptr),
  m_consoleController(nullptr, this, snapshot->scriptStore()
#if EPSILON_GETOPT
      , snapshot->lockOnConsole()
#endif
      ),
  m_listFooter(&m_codeStackViewController, &m_menuController, &m_menuController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGray, ButtonRowController::Size::Large),
  m_menuController(&m_listFooter, this, snapshot->scriptStore(), &m_listFooter),
  m_codeStackViewController(&m_modalViewController, &m_listFooter, Escher::StackViewController::Style::WhiteUniform),
  m_variableBoxController(snapshot->scriptStore())
{
  Clipboard::sharedClipboard()->enterPython();
}

App::~App() {
  deinitPython();
  Clipboard::sharedClipboard()->exitPython();
}

bool App::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::USBEnumeration || event == Ion::Events::Home) && m_consoleController.inputRunLoopActive()) {
    /* We need to return true here because we want to actually exit from the
     * input run loop, which requires ending a dispatchEvent cycle. */
    m_consoleController.terminateInputLoop();
    if (m_modalViewController.isDisplayingModal()) {
      m_modalViewController.dismissModalViewController();
    }
    Ion::USB::clearEnumerationInterrupt();
    return true;
  }
  return false;
}

void App::willExitResponderChain(Responder * nextFirstResponder) {
  m_menuController.willExitApp();
}

Toolbox * App::toolboxForInputEventHandler(InputEventHandler * textInput) {
  return &m_toolbox;
}

VariableBoxController * App::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  return &m_variableBoxController;
}

bool App::textInputDidReceiveEvent(InputEventHandler * textInput, Ion::Events::Event event) {
  const char * pythonText = Helpers::PythonTextForEvent(event);
  if (pythonText != nullptr) {
    textInput->handleEventWithText(pythonText);
    return true;
  }
  return false;
}

void App::initPythonWithUser(const void * pythonUser) {
  if (!m_pythonUser) {
    char * heap = pythonHeap();
    MicroPython::init(heap, heap + k_pythonHeapSize);
  }
  m_pythonUser = pythonUser;
}

void App::deinitPython() {
  if (m_pythonUser) {
    MicroPython::deinit();
    m_pythonUser = nullptr;
  }
}

}
