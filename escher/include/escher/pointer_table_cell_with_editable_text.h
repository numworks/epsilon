#ifndef ESCHER_POINTER_TABLE_CELL_WITH_EDITABLE_TEXT_H
#define ESCHER_POINTER_TABLE_CELL_WITH_EDITABLE_TEXT_H

#include <escher/editable_text_cell.h>
#include <escher/pointer_table_cell.h>
#include <escher/responder.h>

class PointerTableCellWithEditableText : public Responder, public PointerTableCell {
public:
  PointerTableCellWithEditableText(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, char * draftTextBuffer, char * label = nullptr);
  View * accessoryView() const override;
  const char * editedText() const;
  void didBecomeFirstResponder() override;
  void setEditing(bool isEditing);
  void setHighlighted(bool highlight) override;
  void setAccessoryText(const char * text);
  void setTextColor(KDColor color) override;
  constexpr static int k_bufferLength = 255;
private:
  constexpr static int k_maxNumberOfEditableCharacters = 14;
  TextField m_textField;
  char m_textBody[k_bufferLength];
};

#endif
