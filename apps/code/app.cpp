#include "app.h"
#include "../apps_container.h"
#include "code_icon.h"
#include "../shared/toolbox_helpers.h"
#include "../i18n.h"

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
  return new App(container, this);
}

void App::Snapshot::reset() {
  m_scriptStore.deleteAllScripts();
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
      const char * scriptContent = separator+1;
      Code::ScriptTemplate script(scriptName, scriptContent);
      m_scriptStore.addScriptFromTemplate(&script);
      return;
  }
  if (strcmp(name, "lock-on-console") == 0) {
    m_lockOnConsole = true;
    return;
  }
}
#endif

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_codeStackViewController, I18n::Message::Warning),
  m_listFooter(&m_codeStackViewController, &m_menuController, &m_menuController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey, ButtonRowController::Size::Large),
  m_menuController(&m_listFooter, snapshot->scriptStore(), &m_listFooter
#if EPSILON_GETOPT
      , snapshot->lockOnConsole()
#endif
      ),
  m_codeStackViewController(&m_modalViewController, &m_listFooter),
  m_toolboxActionForTextArea([](void * sender, const char * text) {
      TextArea * textArea = static_cast<TextArea *>(sender);
      int previousCursorLocation = textArea->cursorLocation();
      if (textArea->insertTextWithIndentation(text)) {
      // insertText() also moves the cursor. We need to re-move it to the
      // position we want (which is after the first parenthesis or before the
      // first point).
        int deltaCursorLocation = - textArea->cursorLocation() + previousCursorLocation + Shared::ToolboxHelpers::CursorIndexInCommand(text);
      // WARNING: This is a dirty and only works because the cursor location we
      // want is always on the first line of the text we insert. Because of the
      // auto indentation, it would be difficult to compute the wanted cursor
      // location on other lines of the text.
        textArea->moveCursor(deltaCursorLocation);
      }}),
  m_toolboxActionForTextField([](void * sender, const char * text) {
      TextField * textField = static_cast<TextField *>(sender);
      if (!textField->isEditing()) {
        textField->setEditing(true);
      }
      int newCursorLocation = textField->cursorLocation() + Shared::ToolboxHelpers::CursorIndexInCommand(text);
      if (textField->insertTextAtLocation(text, textField->cursorLocation())) {
        textField->setCursorLocation(newCursorLocation);
      }}),
  m_variableBoxController(&m_menuController, snapshot->scriptStore())
{
}

}
