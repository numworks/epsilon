#include "app.h"
#include "code_icon.h"
#include <apps/i18n.h>
#include "helpers.h"
#include <ion/unicode/utf8_helper.h>
#include <apps/apps_container.h>

namespace Code {

I18n::Message App::Descriptor::name() {
  return I18n::Message::CodeApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::CodeAppCapital;
}

App::Descriptor::ExaminationLevel App::Descriptor::examinationLevel() {
  return App::Descriptor::ExaminationLevel::Basic;
}

const Image * App::Descriptor::icon() {
  return ImageStore::CodeIcon;
}

App::Snapshot::Snapshot() :
#if EPSILON_GETOPT
  m_lockOnConsole(false),
  m_hasBeenWiped(false),
#endif
  m_scriptStore()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
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
    if (!m_hasBeenWiped) {
      m_hasBeenWiped = true;
      m_scriptStore.deleteAllScripts();
    }
  
    char * separator = const_cast<char *>(UTF8Helper::CodePointSearch(value, ':'));
    if (*separator == 0) {
      return;
    }
    *separator = 0;
    const char * scriptName = value;
    /* We include the 0 in the scriptContent to represent the importation
     * status. It is set to 1 after addScriptFromTemplate. Indeed, this '/0'
     * char has two goals: ending the scriptName and representing the
     * importation status; we cannot set it to 1 before adding the script to
     * storage. */
    const char * scriptContent = separator;
    Code::ScriptTemplate script(scriptName, scriptContent);
    m_scriptStore.addScriptFromTemplate(&script);
    ScriptStore::ScriptNamed(scriptName).toggleAutoimportationStatus(); // set Importation Status to 1
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
  m_pythonHeap{},
  m_pythonUser(nullptr),
  m_consoleController(nullptr, this, snapshot->scriptStore()
#if EPSILON_GETOPT
      , snapshot->lockOnConsole()
#endif
      ),
  m_listFooter(&m_codeStackViewController, &m_menuController, &m_menuController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGray, ButtonRowController::Size::Large),
  m_menuController(&m_listFooter, this, snapshot->scriptStore(), &m_listFooter),
  m_codeStackViewController(&m_modalViewController, &m_listFooter),
  m_variableBoxController(snapshot->scriptStore())
{
  Clipboard::sharedClipboard()->enterPython();
}

App::~App() {
  assert(!m_consoleController.inputRunLoopActive());
  deinitPython();
  Clipboard::sharedClipboard()->exitPython();
}

bool App::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Home && m_consoleController.inputRunLoopActive()) {
    /* We need to return true here because we want to actually exit from the
     * input run loop, which requires ending a dispatchEvent cycle. */
    m_consoleController.terminateInputLoop();
    if (m_modalViewController.isDisplayingModal()) {
      m_modalViewController.dismissModalViewController();
    }
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
  if (event == Ion::Events::XNT) {
    int bufferSize = CodePoint::MaxCodePointCharLength + 1;
    char buffer[bufferSize];
    bool shouldRemoveLastCharacter = false;
    CodePoint codePoint = AppsContainer::sharedAppsContainer()->XNT('x', &shouldRemoveLastCharacter);
    UTF8Decoder::CodePointToChars(codePoint, buffer, bufferSize);
    buffer[UTF8Decoder::CharSizeOfCodePoint(codePoint)] = 0;
    textInput->handleEventWithText(const_cast<char *>(buffer), false, false, shouldRemoveLastCharacter);
    return true;
  }
  const char * pythonText = Helpers::PythonTextForEvent(event);
  if (pythonText != nullptr) {
    textInput->handleEventWithText(pythonText);
    return true;
  }
  return false;
}

void App::initPythonWithUser(const void * pythonUser) {
  if (!m_pythonUser) {
    MicroPython::init(m_pythonHeap, m_pythonHeap + k_pythonHeapSize);
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
