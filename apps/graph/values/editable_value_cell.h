#ifndef GRAPH_EDITABLE_VALUE_CELL_H
#define GRAPH_EDITABLE_VALUE_CELL_H

#include <escher.h>
#include <poincare.h>

namespace Graph {
class EditableValueCell : public EvenOddCell, public Responder {
public:
  EditableValueCell();
  void reloadCell() override;
  void setText(const char * textContent);
  const char * editedText() const;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  bool handleEvent(Ion::Events::Event event) override;
  void editValue(const char * initialText, int cursorLocation, void * context, Invocation::Action successAction);
protected:
  BufferTextView m_bufferTextView;
  TextField m_textField;
  char m_textBody[255];
  bool m_isEditing;
  Invocation m_successAction;
};

}

#endif
