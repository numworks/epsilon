#ifndef FINANCE_TWO_MESSAGE_POPUP_DATA_SOURCE_H
#define FINANCE_TWO_MESSAGE_POPUP_DATA_SOURCE_H

#include <apps/i18n.h>
#include <escher/list_view_data_source.h>
#include <escher/buffer_text_highlight_cell.h>

namespace Finance {

class TwoMessagesPopupDataSource : public Escher::ListViewDataSource {
public:
  TwoMessagesPopupDataSource() :
      m_message1(I18n::Message::Default), m_message2(I18n::Message::Default) {}
  int numberOfRows() const override { return k_numberOfRows; }
  int reusableCellCount(int type) override { return k_numberOfRows; }
  Escher::BufferTextHighlightCell * reusableCell(int i, int type) override {
    return &m_cells[i];
  }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override {
    static_cast<Escher::BufferTextHighlightCell *>(cell)->setText(
      I18n::translate(index == 0 ? m_message1 : m_message2));
  }
  void setMessages(I18n::Message message1, I18n::Message message2) {
    m_message1 = message1;
    m_message2 = message2;
  }

private:
  constexpr static int k_numberOfRows = 2;
  // Not needed because DropdownPopupController takes care of it
  KDCoordinate nonMemoizedRowHeight(int r) override { assert(false); return 0; }

  I18n::Message m_message1;
  I18n::Message m_message2;
  Escher::BufferTextHighlightCell m_cells[k_numberOfRows];
};

}  // namespace Finance

#endif /* FINANCE_TWO_MESSAGE_POPUP_DATA_SOURCE_H */
