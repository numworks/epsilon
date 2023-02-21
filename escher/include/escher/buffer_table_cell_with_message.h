#ifndef ESCHER_BUFFER_TABLE_CELL_WITH_MESSAGE_H
#define ESCHER_BUFFER_TABLE_CELL_WITH_MESSAGE_H

#include <escher/buffer_table_cell.h>

#include "message_text_view.h"

namespace Escher {

class BufferTableCellWithMessage : public BufferTableCell {
 public:
  BufferTableCellWithMessage(I18n::Message message = (I18n::Message)0);
  const View* subLabelView() const override { return &m_subLabelView; }
  void setHighlighted(bool highlight) override;
  void setSubLabelMessage(I18n::Message textBody);

 protected:
  MessageTextView m_subLabelView;
};

}  // namespace Escher

#endif
