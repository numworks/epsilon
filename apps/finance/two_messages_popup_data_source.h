#ifndef FINANCE_TWO_MESSAGE_POPUP_DATA_SOURCE_H
#define FINANCE_TWO_MESSAGE_POPUP_DATA_SOURCE_H

#include <apps/i18n.h>
#include <escher/buffer_text_highlight_cell.h>
#include <escher/explicit_list_view_data_source.h>

namespace Finance {

class TwoMessagesPopupDataSource : public Escher::ExplicitListViewDataSource {
 public:
  TwoMessagesPopupDataSource() {}
  int numberOfRows() const override { return k_numberOfRows; }
  Escher::HighlightCell* cell(int row) override { return &m_cells[row]; }
  void setMessages(I18n::Message message1, I18n::Message message2) {
    m_cells[0].setText(I18n::translate(message1));
    m_cells[1].setText(I18n::translate(message2));
  }

 private:
  constexpr static int k_numberOfRows = 2;
  Escher::SmallBufferTextHighlightCell m_cells[k_numberOfRows];
};

}  // namespace Finance

#endif
