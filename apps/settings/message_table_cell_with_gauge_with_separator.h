#ifndef SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H

#include "cell_with_separator.h"

namespace Settings {

class MessageTableCellWithGaugeWithSeparator : public CellWithSeparator {
public:
  MessageTableCellWithGaugeWithSeparator(I18n::Message message, const KDFont * font) :
    CellWithSeparator(),
    m_cell(message, font) {}
  View * accessoryView() const { return m_cell.accessoryView(); }
  void setMessage(I18n::Message message) { return m_cell.setMessage(message); }
private:
  HighlightCell * cell() override { return &m_cell; }
  MessageTableCellWithGauge m_cell;
};

}

#endif
