#ifndef APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_BUFFER_WITH_MESSAGE_H
#define APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_BUFFER_WITH_MESSAGE_H

#include <escher/buffer_text_view.h>

#include "layout_cell_with_sub_message.h"

namespace Probability {

class LayoutCellWithBufferWithMessage : public LayoutCellWithSubMessage {
public:
  const View * accessoryView() const override { return &m_labelView; }
  void setAccessoryText(const char * textBody) { m_labelView.setText(textBody); }
  void setHighlighted(bool highlight) override {
    LayoutCellWithSubMessage::setHighlighted(highlight);
    m_labelView.setBackgroundColor(highlight ? Escher::Palette::Select : backgroundColor());
  }
  Escher::BufferTextView * bufferTextView() { return &m_labelView; }

private:
  Escher::BufferTextView m_labelView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_BUFFER_WITH_MESSAGE_H */
