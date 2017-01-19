#ifndef ESCHER_EDITABLE_TEXT_MENU_LIST_CELL_H
#define ESCHER_EDITABLE_TEXT_MENU_LIST_CELL_H

#include <escher/editable_text_cell.h>
#include <escher/menu_list_cell.h>
#include <escher/responder.h>

class EditableTextMenuListCell : public Responder, public MenuListCell {
public:
  EditableTextMenuListCell(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, char * draftTextBuffer, char * label = nullptr);
  View * accessoryView() const override;
  const char * editedText() const;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  void setEditing(bool isEditing);
  void reloadCell() override;
  void setAccessoryText(const char * text);
  void setTextColor(KDColor color) override;
  constexpr static int k_bufferLength = 255;
private:
  constexpr static int k_maxNumberOfEditableCharacters = 14;
  TextField m_textField;
  char m_textBody[k_bufferLength];
};

#endif
