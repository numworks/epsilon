#ifndef PERDIODIC_TABLE_CELLS_H
#define PERDIODIC_TABLE_CELLS_H

#include <apps/shared/cell_with_separator.h>
#include <apps/i18n.h>
#include <escher/expression_view.h>
#include <escher/message_table_cell_with_message_with_buffer.h>

namespace Periodic {

class MessageTableCellWithMessageWithBufferWithSeparator : public Shared::CellWithSeparator {
public:
  MessageTableCellWithMessageWithBufferWithSeparator(I18n::Message message = I18n::Message::Default) : m_cell(message) {}

  Escher::MessageTableCellWithMessageWithBuffer * innerCell() { return &m_cell; }

private:
  const Escher::TableCell * constCell() const override { return &m_cell; }

  Escher::MessageTableCellWithMessageWithBuffer m_cell;
};

class MessageTableCellWithMessageWithExpression : public Escher::MessageTableCellWithMessage {
public:
  MessageTableCellWithMessageWithExpression(I18n::Message message = I18n::Message::Default);

  const Escher::View * accessoryView() const override { return &m_accessoryView; }
  void setHighlighted(bool highlight) override;
  void setAccessoryLayout(Poincare::Layout layout);

private:
  bool shouldAlignLabelAndAccessory() const override { return true; }
  bool shouldAlignSublabelRight() const override { return false; }

  Escher::ExpressionView m_accessoryView;
};

class MessageTableCellWithMessageWithExpressionWithSeparator : public Shared::CellWithSeparator {
public:
  MessageTableCellWithMessageWithExpressionWithSeparator(I18n::Message message = I18n::Message::Default) : m_cell(message) {}

  void setSeparatorPosition(bool above) { m_separatorAbove = above; }
  MessageTableCellWithMessageWithExpression * innerCell() { return &m_cell; }

private:
  const Escher::TableCell * constCell() const override { return &m_cell; }
  bool separatorAboveCell() const override { return m_separatorAbove; }

  MessageTableCellWithMessageWithExpression m_cell;
  bool m_separatorAbove;
};

}

#endif
