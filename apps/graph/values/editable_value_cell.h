#ifndef GRAPH_EDITABLE_VALUE_CELL_H
#define GRAPH_EDITABLE_VALUE_CELL_H

#include <escher.h>
#include <poincare.h>

namespace Graph {
class EditableValueCell : public EvenOddCell, public Responder {
public:
  EditableValueCell(char * draftTextBuffer);
  void setDelegate(TextFieldDelegate * delegate);
  void reloadCell() override;
  const char * text() const;
  void setText(const char * textContent);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  void setEditing(bool isEditing);
  constexpr static int k_bufferLength = 255;
private:
  TextField m_textField;
  char m_textBody[k_bufferLength];
};

}

#endif
