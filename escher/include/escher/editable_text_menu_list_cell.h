#ifndef ESCHER_EDITABLE_TEXT_MENU_LIST_CELL_H
#define ESCHER_EDITABLE_TEXT_MENU_LIST_CELL_H

#include <escher/text_menu_list_cell.h>
#include <escher/text_field.h>
#include <escher/invocation.h>
#include <escher/responder.h>

class EditableTextMenuListCell : public Responder, public TextMenuListCell {
public:
  EditableTextMenuListCell(char * label = nullptr);
  View * accessoryView() const override;
  const char * editedText() const;
  void layoutSubviews() override;
  bool handleEvent(Ion::Events::Event event) override;
  void editValue(TextFieldDelegate * textFieldDelegate, const char * initialText, int cursorLocation, void * context, Invocation::Action successAction);
private:
  constexpr static KDCoordinate k_textWidth = 7*7;
  constexpr static KDCoordinate k_textHeight = 12;
  TextField m_textField;
  char m_textBody[255];
  bool m_isEditing;
  Invocation m_successAction;
};

#endif
