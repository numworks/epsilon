#ifndef GRAPH_EDITABLE_VALUE_CELL_H
#define GRAPH_EDITABLE_VALUE_CELL_H

#include <escher.h>
#include <poincare.h>

namespace Graph {
class EditableValueCell : public EvenOddCell, public Responder {
public:
  EditableValueCell();
  void setDelegate(TextFieldDelegate * delegate);
  void reloadCell() override;
  const char * text() const;
  void setText(const char * textContent);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  void setEditing(bool isEditing);
private:
  TextField m_textField;
  char m_textBody[255];
  char m_draftTextBody[255];
};

}

#endif
