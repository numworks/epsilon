#ifndef SETTINGS_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_SEPARATOR_H

#include "cell_with_separator.h"

namespace Settings {

class MessageTableCellWithEditableTextWithSeparator : public CellWithSeparator {
public:
  MessageTableCellWithEditableTextWithSeparator(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * textFieldDelegate = nullptr, I18n::Message message = (I18n::Message)0) :
    CellWithSeparator(),
    m_cell(parentResponder, inputEventHandlerDelegate, textFieldDelegate, message) {}
  const char * text() const override { return m_cell.text(); }
  Poincare::Layout layout() const override{ return m_cell.layout(); }
  MessageTableCellWithEditableText * messageTableCellWithEditableText() { return &m_cell; }
private:
  HighlightCell * cell() override { return &m_cell; }
  MessageTableCellWithEditableText m_cell;
};

}

#endif
