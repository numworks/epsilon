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

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_codeStackViewController, I18n::Message::Warning),
  m_listFooter(&m_codeStackViewController, &m_menuController, &m_menuController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey),
  m_menuController(&m_listFooter, snapshot->scriptStore(), &m_listFooter),
  m_codeStackViewController(&m_modalViewController, &m_listFooter),
  m_toolboxActionForTextArea([](void * sender, const char * text) {
      TextArea * textArea = static_cast<TextArea *>(sender);
      textArea->insertText(text);
      // insertText() also moves the cursor. We need to remove it to the
      // position we want (which is after the first parenthesis or before the
      // first point).
      int deltaCursorLocation = - (strlen(text) - Shared::ToolboxHelpers::CursorIndexInCommand(text));
      textArea->moveCursor(deltaCursorLocation);
      }),
  m_toolboxActionForTextField([](void * sender, const char * text) {
      TextField * textField = static_cast<TextField *>(sender);
      if (!textField->isEditing()) {
        textField->setEditing(true);
      }
      int newCursorLocation = textField->cursorLocation() + Shared::ToolboxHelpers::CursorIndexInCommand(text);
      textField->insertTextAtLocation(text, textField->cursorLocation());
      textField->setCursorLocation(newCursorLocation);
      })
{
}

}
