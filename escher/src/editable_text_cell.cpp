#include <escher/editable_text_cell.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <assert.h>

EditableTextCell::EditableTextCell(Responder * parentResponder) :
  TableViewCell(),
  Responder(parentResponder),
  m_textField(TextField(this, m_textBody, 255, nullptr)),
  m_isEditing(false),
  m_successAction(Invocation(nullptr, nullptr)),
  m_bufferText(BufferTextView())
{
}

void EditableTextCell::reloadCell() {
  TableViewCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_bufferText.setBackgroundColor(backgroundColor);
}

const char * EditableTextCell::editedText() const {
  return m_textField.text();
}

void EditableTextCell::setText(const char * text) {
  m_bufferText.setText(text);
}

int EditableTextCell::numberOfSubviews() const {
  return 1;
}

View * EditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  if (m_isEditing) {
    return &m_textField;
  }
  return &m_bufferText;
}

void EditableTextCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_textField.setFrame(KDRect(k_separatorThickness, (height - k_textHeight)/2, width - k_separatorThickness, k_textHeight));
  m_bufferText.setFrame(KDRect(k_separatorThickness, (height - k_textHeight)/2, width - k_separatorThickness, k_textHeight));
}

void EditableTextCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backgroundColor = isHighlighted() ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  ctx->fillRect(rect, backgroundColor);
}

bool EditableTextCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    m_isEditing = false;
    m_successAction.perform(this);
    app()->setFirstResponder(parentResponder());
    return true;
  }
  m_isEditing = false;
  app()->setFirstResponder(parentResponder());
  return false;
}

void EditableTextCell::editValue(TextFieldDelegate * textFieldDelegate, const char * initialText, int cursorPosition, void * context, Invocation::Action successAction) {
  markRectAsDirty(bounds());
  m_textField.setTextFieldDelegate(textFieldDelegate);
  app()->setFirstResponder(&m_textField);
  m_isEditing = true;
  m_textField.setText(initialText);
  m_textField.setCursorLocation(cursorPosition);
  m_successAction = Invocation(successAction, context);
}
