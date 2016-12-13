#ifndef ESCHER_EDITABLE_TEXT_MENU_LIST_CELL_H
#define ESCHER_EDITABLE_TEXT_MENU_LIST_CELL_H

#include <escher/editable_text_cell.h>
#include <escher/menu_list_cell.h>
#include <escher/responder.h>

class EditableTextMenuListCell : public Responder, public MenuListCell {
public:
  EditableTextMenuListCell(char * label = nullptr, Responder * parentResponder = nullptr, TextFieldDelegate * textFieldDelegate = nullptr);
  void setDelegate(TextFieldDelegate * delegate);
  View * accessoryView() const override;
  const char * editedText() const;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  void setEditing(bool isEditing);
  void reloadCell() override;
  void setAccessoryText(const char * text);
  void setTextColor(KDColor color) override;
private:
  constexpr static KDCoordinate k_textWidth = 7*7;
  constexpr static KDCoordinate k_textHeight = 12;
  TextField m_textField;
  char m_textBody[255];
  char m_draftTextBody[255];
};

#endif
