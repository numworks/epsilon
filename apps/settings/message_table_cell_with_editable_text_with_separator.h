#ifndef SETTINGS_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_SEPARATOR_H

#include <escher/message_table_cell_with_editable_text.h>
#include "cell_with_separator.h"

namespace Settings {

class MessageTableCellWithEditableTextWithSeparator : public CellWithSeparator {
public:
  MessageTableCellWithEditableTextWithSeparator(Escher::Responder * parentResponder = nullptr, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, Escher::TextFieldDelegate * textFieldDelegate = nullptr, I18n::Message message = (I18n::Message)0) :
    CellWithSeparator(),
    m_cell(parentResponder, inputEventHandlerDelegate, textFieldDelegate, message) {}
  const char * text() const override { return m_cell.text(); }
  Poincare::Layout layout() const override{ return m_cell.layout(); }
  Escher::MessageTableCellWithEditableText * messageTableCellWithEditableText() { return &m_cell; }
  KDSize minimalSizeForOptimalDisplay() const override { return m_cell.minimalSizeForOptimalDisplay() + CellWithSeparator::minimalSizeForOptimalDisplay(); }
private:
  Escher::HighlightCell * cell() override { return &m_cell; }
  Escher::MessageTableCellWithEditableText m_cell;
};

}

#endif
