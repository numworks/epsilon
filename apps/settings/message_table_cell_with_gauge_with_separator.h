#ifndef SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H

#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_gauge.h>
#include <escher/text_field_delegate.h>
#include "cell_with_separator.h"

namespace Settings {

class MessageTableCellWithGaugeWithSeparator : public CellWithSeparator {
public:
  MessageTableCellWithGaugeWithSeparator(I18n::Message message) :
    CellWithSeparator(),
    m_cell(message) {}
  const Escher::View * accessoryView() const { return m_cell.accessoryView(); }
  void setMessage(I18n::Message message) { return m_cell.setMessage(message); }
  /* Being const, we cannot set m_cell width to the expected width. It must then
   * be handled here. */
  KDSize minimalSizeForOptimalDisplay() const override {
    // Available width is necessary to compute it minimal height.
    KDCoordinate expectedWidth = m_frame.width();
    assert(expectedWidth > 0);
    return KDSize(expectedWidth, Escher::TableCell::minimalHeightForOptimalDisplay(m_cell.labelView(), m_cell.subLabelView(), m_cell.accessoryView(), expectedWidth) + k_margin + Escher::Metric::CellSeparatorThickness);
  }
private:
  Escher::HighlightCell * cell() override { return &m_cell; }
  Escher::MessageTableCellWithGauge m_cell;
};

}

#endif
