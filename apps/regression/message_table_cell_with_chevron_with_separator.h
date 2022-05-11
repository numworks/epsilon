#ifndef REGRESSION_MESSAGE_TABLE_CELL_WITH_CHEVRON_WITH_SEPARATOR_H
#define REGRESSION_MESSAGE_TABLE_CELL_WITH_CHEVRON_WITH_SEPARATOR_H

#include <escher/message_table_cell_with_chevron.h>
#include <apps/shared/cell_with_separator.h>

namespace Regression {

class MessageTableCellWithChevronWithSeparator : public Shared::CellWithSeparator {
public:
  MessageTableCellWithChevronWithSeparator(I18n::Message message = (I18n::Message)0) :
    CellWithSeparator(),
    m_cell(message) {}
  const char * text() const override { return m_cell.text(); }
  Poincare::Layout layout() const override { return m_cell.layout(); }
private:
  const Escher::TableCell * constCell() const override { return &m_cell; }
  Escher::MessageTableCellWithChevron m_cell;
};

}

#endif
