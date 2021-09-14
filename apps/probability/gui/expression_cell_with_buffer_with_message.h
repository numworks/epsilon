#ifndef APPS_PROBABILITY_GUI_EXPRESSION_CELL_WITH_BUFFER_WITH_MESSAGE_H
#define APPS_PROBABILITY_GUI_EXPRESSION_CELL_WITH_BUFFER_WITH_MESSAGE_H

#include <escher/buffer_text_view.h>
#include <escher/expression_table_cell_with_message.h>

namespace Probability {

class ExpressionCellWithBufferWithMessage : public Escher::ExpressionTableCellWithMessage {
public:
  const View * accessoryView() const override { return &m_labelView; }
  void setAccessoryText(const char * textBody) { m_labelView.setText(textBody); }
  void setHighlighted(bool highlight) override {
    ExpressionTableCellWithMessage::setHighlighted(highlight);
    m_labelView.setBackgroundColor(highlight ? Escher::Palette::Select : backgroundColor());
  }
  bool shouldAlignLabelAndAccessory() const override { return true; }
  Escher::BufferTextView * bufferTextView() { return &m_labelView; }

private:
  Escher::BufferTextView m_labelView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_EXPRESSION_CELL_WITH_BUFFER_WITH_MESSAGE_H */
