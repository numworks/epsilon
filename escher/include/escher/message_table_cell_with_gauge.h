#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_GAUGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_GAUGE_H

#include <escher/gauge_view.h>
#include <escher/message_table_cell.h>

namespace Escher {

class MessageTableCellWithGauge : public MessageTableCell {
 public:
  using MessageTableCell::MessageTableCell;
  const View* accessoryView() const override { return &m_accessoryView; }
  void setHighlighted(bool highlight) override;

 private:
  GaugeView m_accessoryView;
};

}  // namespace Escher

#endif
