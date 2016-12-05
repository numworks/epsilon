#ifndef GRAPH_EDITABLE_VALUE_CELL_H
#define GRAPH_EDITABLE_VALUE_CELL_H

#include <escher.h>
#include <poincare.h>
#include "value_cell.h"

namespace Graph {
class EditableValueCell : public ValueCell, public Responder {
public:
  EditableValueCell();
  const char * editedText() const;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  bool handleEvent(Ion::Events::Event event) override;
  void editValue(const char * initialText, int cursorLocation, void * context, Invocation::Action successAction);
private:
  TextField m_textField;
  char m_textBody[255];
  bool m_isEditing;
  Invocation m_successAction;
};

}

#endif
