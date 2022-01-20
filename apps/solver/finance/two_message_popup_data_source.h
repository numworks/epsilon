#ifndef SOLVER_TWO_MESSAGE_POPUP_DATA_SOURCE_H
#define SOLVER_TWO_MESSAGE_POPUP_DATA_SOURCE_H

#include <escher/list_view_data_source.h>
#include <escher/buffer_text_highlight_cell.h>

namespace Solver {

class TwoMessagePopupDataSource : public Escher::ListViewDataSource {
public:
  TwoMessagePopupDataSource(I18n::Message message1, I18n::Message message2) :
    m_message1(message1), m_message2(message2) {}
  int numberOfRows() const override { return k_numberOfRows; }
  // Not needed because DropdownPopupController takes care of it
  KDCoordinate rowHeight(int r) override { assert(false); return 0; }
  int reusableCellCount(int type) override { return k_numberOfRows; }
  Escher::BufferTextHighlightCell * reusableCell(int i, int type) override {
    return &m_cells[i];
  }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override {
    static_cast<Escher::BufferTextHighlightCell *>(cell)->setText(
      I18n::translate(index == 0 ? m_message1 : m_message2));
  }

private:
  constexpr static int k_numberOfRows = 2;

  I18n::Message m_message1;
  I18n::Message m_message2;
  Escher::BufferTextHighlightCell m_cells[k_numberOfRows];
};

}  // namespace Solver

#endif /* SOLVER_TWO_MESSAGE_POPUP_DATA_SOURCE_H */
