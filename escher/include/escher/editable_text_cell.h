#ifndef ESCHER_EDITABLE_TEXT_CELL_H
#define ESCHER_EDITABLE_TEXT_CELL_H

#include <escher/responder.h>
#include <escher/table_view_cell.h>
#include <escher/text_field_delegate.h>
#include <escher/text_field.h>

class EditableTextCell : public TableViewCell, public Responder {
public:
  EditableTextCell(Responder * parentResponder, TextFieldDelegate * delegate, char * draftTextBuffer, KDText::FontSize size = KDText::FontSize::Large,
     float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
  TextField * textField();
  void setHighlighted(bool highlight) override;
  const char * text() const;
  void setText(const char * textContent);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  void setEditing(bool isEditing);
  constexpr static int k_bufferLength = 255;
private:
  constexpr static KDCoordinate k_textHeight = 12;
  constexpr static KDCoordinate k_separatorThickness = 1;
  TextField m_textField;
  char m_textBody[k_bufferLength];
};

#endif
