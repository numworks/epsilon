#include "app.h"
#include "code_icon.h"
#include <apps/i18n.h>
#include "helpers.h"
#include <ion/unicode/utf8_helper.h>

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

void App::Snapshot::loadScript(const char * scriptName, const char * scriptContent) {
  m_scriptStore.deleteAllScripts();
  Script::Create(scriptName, scriptContent);
}

App::App(Snapshot * snapshot) :
  Shared::InputEventHandlerDelegateApp(snapshot, &m_codeStackViewController),
  m_pythonHeap{},
  m_pythonUser(nullptr),
  m_consoleController(nullptr, this, snapshot->scriptStore()),
  m_listFooter(&m_codeStackViewController, &m_menuController, &m_menuController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey, ButtonRowController::Size::Large),
  m_menuController(&m_listFooter, this, snapshot->scriptStore(), &m_listFooter),
  m_codeStackViewController(&m_modalViewController, &m_listFooter),
  m_variableBoxController(snapshot->scriptStore())
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
