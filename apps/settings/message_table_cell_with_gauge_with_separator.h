#ifndef SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H

#include <escher/message_table_cell_with_gauge.h>
#include "cell_with_separator.h"

namespace Settings {

class MessageTableCellWithGaugeWithSeparator : public CellWithSeparator {
public:
  MessageTableCellWithGaugeWithSeparator(I18n::Message message);
  const Escher::View * accessoryView() const { return m_cell.accessoryView(); }
  void setMessage(I18n::Message message) { return m_cell.setMessage(message); }
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  Escher::HighlightCell * cell() override { return &m_cell; }
  Escher::MessageTableCellWithGauge m_cell;
};

}

#endif
