#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_BUFFER_WITH_MESSAGE_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_BUFFER_WITH_MESSAGE_H

#include <escher/buffer_text_view.h>
#include <escher/expression_table_cell_with_message.h>

namespace Escher {

class ExpressionTableCellWithMessageWithBuffer : public ExpressionTableCellWithMessage {
public:
  using ExpressionTableCellWithMessage::ExpressionTableCellWithMessage;
  const View * accessoryView() const override { return &m_accessoryView; }
  void setAccessoryText(const char * textBody) { m_accessoryView.setText(textBody); }
  void setHighlighted(bool highlight) override {
    ExpressionTableCellWithMessage::setHighlighted(highlight);
    m_accessoryView.setBackgroundColor(highlight ? Palette::Select : backgroundColor());
  }
  BufferTextView * bufferTextView() { return &m_accessoryView; }
  const char * text() const override { return m_accessoryView.text(); }

private:
  BufferTextView m_accessoryView;
};

}

#endif
