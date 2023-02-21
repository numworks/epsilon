#ifndef SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H

#include <escher/message_table_cell_with_gauge.h>

#include "../shared/cell_with_separator.h"

namespace Settings {

class MessageTableCellWithGaugeWithSeparator
    : public Shared::CellWithSeparator {
 public:
  MessageTableCellWithGaugeWithSeparator(I18n::Message message)
      : CellWithSeparator(), m_cell(message) {}
  const Escher::View* accessoryView() const { return m_cell.accessoryView(); }
  void setMessage(I18n::Message message) { return m_cell.setMessage(message); }

 private:
  const Escher::TableCell* constCell() const override { return &m_cell; }
  Escher::MessageTableCellWithGauge m_cell;
};

}  // namespace Settings

#endif
