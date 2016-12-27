#ifndef ESCHER_EVEN_ODD_EDITABLE_TEXT_CELL_H
#define ESCHER_EVEN_ODD_EDITABLE_TEXT_CELL_H

#include <escher/responder.h>
#include <escher/even_odd_cell.h>
#include <escher/editable_text_cell.h>

class EvenOddEditableTextCell : public EvenOddCell, public Responder {
public:
  EvenOddEditableTextCell(Responder * parentResponder, TextFieldDelegate * delegate, char * draftTextBuffer);
  void reloadCell() override;
  const char * text() const;
  void setText(const char * textContent);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  void setEditing(bool isEditing);
private:
  EditableTextCell m_editableCell;
};

#endif
