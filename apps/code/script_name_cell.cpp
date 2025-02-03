#include "script_name_cell.h"

#include <assert.h>

#include "app.h"

using namespace Escher;

namespace Code {

bool ScriptNameCell::ScriptNameTextField::handleEvent(
    Ion::Events::Event event) {
  // Prevent using VarBox and ToolBox
  return event != Ion::Events::Var && event != Ion::Events::Toolbox &&
         Escher::TextField::handleEvent(event);
}

void ScriptNameCell::ScriptNameTextField::willSetCursorLocation(
    const char** location) {
  size_t textLength = strlen(text());
  assert(textLength >= k_extensionLength);
  const char* maxLocation =
      m_contentView.draftText() + (textLength - k_extensionLength);
  if (*location > maxLocation) {
    *location = maxLocation;
  }
}

bool ScriptNameCell::ScriptNameTextField::privateRemoveEndOfLine() {
  return removeTextBeforeExtension(false);
}

void ScriptNameCell::ScriptNameTextField::removeWholeText() {
  removeTextBeforeExtension(true);
  scrollToCursor();
}

bool ScriptNameCell::ScriptNameTextField::removeTextBeforeExtension(
    bool whole) {
  assert(isEditing());
  const char* extension =
      m_contentView.draftText() + (strlen(text()) - k_extensionLength);
  assert(extension >= m_contentView.draftText() &&
         extension < m_contentView.draftText() +
                         (ContentView::k_maxBufferSize - k_extensionLength));
  char* destination =
      const_cast<char*>(whole ? m_contentView.draftText() : cursorLocation());
  if (destination == extension) {
    return false;
  }
  assert(destination >= m_contentView.draftText());
  assert(destination < extension);
  m_contentView.willModifyTextBuffer();
  strlcpy(
      destination, extension,
      ContentView::k_maxBufferSize - (destination - m_contentView.draftText()));
  m_contentView.setCursorLocation(destination);
  m_contentView.didModifyTextBuffer();
  layoutSubviews();
  return true;
}

const char* ScriptNameCell::text() const {
  if (!m_textField.isEditing()) {
    return m_textField.text();
  }
  return nullptr;
}

KDSize ScriptNameCell::minimalSizeForOptimalDisplay() const {
  return m_textField.minimalSizeForOptimalDisplay();
}

void ScriptNameCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    App::app()->setFirstResponder(&m_textField);
  } else {
    Responder::handleResponderChainEvent(event);
  }
}

void ScriptNameCell::updateSubviewsBackgroundAfterChangingState() {
  m_textField.setBackgroundColor(backgroundColor());
}

void ScriptNameCell::layoutSubviews(bool force) {
  KDRect cellBounds = bounds();
  setChildFrame(&m_textField,
                KDRect(cellBounds.x() + k_leftMargin, cellBounds.y(),
                       cellBounds.width() - k_leftMargin, cellBounds.height()),
                force);
}

}  // namespace Code
