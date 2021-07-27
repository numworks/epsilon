#ifndef APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_SUB_MESSAGE_H
#define APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_SUB_MESSAGE_H

#include <escher/message_text_view.h>

#include "layout_cell.h"

namespace Probability {

class LayoutCellWithSubMessage : public LayoutCell {
public:
  LayoutCellWithSubMessage();
  const View * subLabelView() const override { return &m_messageTextView; }
  void setSubLabelMessage(I18n::Message message);
  void setHighlighted(bool highlight) override;

private:
  Escher::MessageTextView m_messageTextView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_SUB_MESSAGE_H */
