#include <escher/editable_text_menu_list_cell.h>
#include <escher/app.h>

EditableTextMenuListCell::EditableTextMenuListCell(char * label) :
  Responder(nullptr),
  TextMenuListCell(label),
  m_textField(TextField(this, m_textBody, 255, nullptr)),
  m_isEditing(false),
  m_successAction(Invocation(nullptr, nullptr))
{
}

View * EditableTextMenuListCell::accessoryView() const {
  if (m_isEditing) {
    return (View *)&m_textField;
  }
  return (View *)&m_accessoryView;
}

const char * EditableTextMenuListCell::editedText() const {
  return m_textField.text();
}

void EditableTextMenuListCell::layoutSubviews() {
  TextMenuListCell::layoutSubviews();
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_textField.setFrame(KDRect(width - k_textWidth - k_separatorThickness, (height - k_textHeight)/2, k_textWidth - k_separatorThickness, k_textHeight));
  m_accessoryView.setFrame(KDRect(width - k_textWidth - k_separatorThickness, (height - k_textHeight)/2, k_textWidth - k_separatorThickness, k_textHeight));
}

bool EditableTextMenuListCell::handleEvent(Ion::Events::Event event) {
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


void EditableTextMenuListCell::editValue(TextFieldDelegate * textFieldDelegate, const char * initialText, int cursorPosition, void * context, Invocation::Action successAction) {
  markRectAsDirty(bounds());
  m_textField.setTextFieldDelegate(textFieldDelegate);
  app()->setFirstResponder(&m_textField);
  m_isEditing = true;
  m_textField.setText(initialText);
  m_textField.setCursorLocation(cursorPosition);
  m_successAction = Invocation(successAction, context);
}