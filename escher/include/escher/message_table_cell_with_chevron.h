#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_H

#include <escher/message_table_cell.h>
#include <escher/chevron_view.h>

namespace Escher {

class MessageTableCellWithChevron : public MessageTableCell {
public:
  using MessageTableCell::MessageTableCell;
  const View * accessoryView() const override { return &m_accessoryView; }

  static bool ShouldEnterOnEvent(Ion::Events::Event event) {
    return (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right);
  }
  bool cellTypeIsStorable() const override { return false; }

private:
  ChevronView m_accessoryView;
};

}

#endif
