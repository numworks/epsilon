#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_H

#include <escher/message_table_cell.h>
#include <escher/chevron_view.h>

namespace Escher {

class MessageTableCellWithChevron : public MessageTableCell {
public:
  using MessageTableCell::MessageTableCell;
  const View * accessoryView() const override { return &m_accessoryView; }

  template<class T>
  bool handleEvent(Ion::Events::Event event, T * object, void (T::*callback)()) {
    if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
      (object->*callback)();
      return true;
    }
    return false;
  }
private:
  ChevronView m_accessoryView;
};

}

#endif
