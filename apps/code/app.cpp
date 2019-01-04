#include "app.h"
#include "../apps_container.h"
#include "code_icon.h"
#include "../i18n.h"
#include "helpers.h"

namespace Code {

I18n::Message App::Descriptor::name() {
  return I18n::Message::CodeApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::CodeAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::CodeIcon;
}

App::Snapshot::Snapshot() :
#if EPSILON_GETOPT
  m_lockOnConsole(false),
#endif
  m_scriptStore()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(container, this);
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

void App::Snapshot::setOpt(const char * name, char * value) {
  if (strcmp(name, "script") == 0) {
    m_scriptStore.deleteAllScripts();
    char * separator = strchr(value, ':');
    if (!separator) {
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
    m_scriptStore.scriptNamed(scriptName).toggleImportationStatus(); // set Importation Status to 1
    return;
  }
  if (strcmp(name, "lock-on-console") == 0) {
    m_lockOnConsole = true;
    return;
  }
}
#endif

App::App(Container * container, Snapshot * snapshot) :
  Shared::InputEventHandlerDelegateApp(container, snapshot, &m_codeStackViewController),
  m_pythonHeap{},
  m_pythonUser(nullptr),
  m_consoleController(nullptr, this, snapshot->scriptStore()
#if EPSILON_GETOPT
      , snapshot->lockOnConsole()
#endif
      ),
  m_listFooter(&m_codeStackViewController, &m_menuController, &m_menuController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey, ButtonRowController::Size::Large),
  m_menuController(&m_listFooter, this, snapshot->scriptStore(), &m_listFooter),
  m_codeStackViewController(&m_modalViewController, &m_listFooter),
  m_variableBoxController(this, snapshot->scriptStore())
{
}

App::~App() {
  assert(!m_consoleController.inputRunLoopActive());
  deinitPython();
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
