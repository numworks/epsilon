#ifndef ESCHER_EDITABLE_TEXT_CELL_H
#define ESCHER_EDITABLE_TEXT_CELL_H

#include <escher/buffer_text_view.h>
#include <escher/invocation.h>
#include <escher/responder.h>
#include <escher/table_view_cell.h>
#include <escher/text_field_delegate.h>
#include <escher/text_field.h>

class EditableTextCell : public TableViewCell, public Responder {
public:
  EditableTextCell(Responder * parentResponder = nullptr);
  void reloadCell() override;
  const char * editedText() const;
  const char * text() const;
  void setText(const char * textContent);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void editValue(TextFieldDelegate * textFieldDelegate, const char * initialText, int cursorLocation, void * context, Invocation::Action successAction);
  bool handleEvent(Ion::Events::Event event) override;
private:
  constexpr static KDCoordinate k_textHeight = 12;
  constexpr static KDCoordinate k_separatorThickness = 1;
  TextField m_textField;
  char m_textBody[255];
  bool m_isEditing;
  Invocation m_successAction;
  BufferTextView m_bufferText;
};

#endif
