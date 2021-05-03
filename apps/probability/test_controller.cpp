#include "test_controller.h"
#include <assert.h>
#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/container.h>

TestController::TestController(Escher::Responder * parentResponder)
  : SelectableListViewController(parentResponder) {
  // Create cells
  m_cells[k_indexOfOneProp].setMessage(I18n::Message::TestOneProp);
  m_cells[k_indexOfOneProp].setSubtitle(I18n::Message::TestOnePropSub);
  m_cells[k_indexOfOneMean].setMessage(I18n::Message::TestOneMean);
  m_cells[k_indexOfOneMean].setSubtitle(I18n::Message::TestOneMeanSub);
  m_cells[k_indexOfTwoProps].setMessage(I18n::Message::TestTwoProps);
  m_cells[k_indexOfTwoProps].setSubtitle(I18n::Message::TestTwoPropsSub);
  m_cells[k_indexOfTwoMeans].setMessage(I18n::Message::TestTwoMeans);
  m_cells[k_indexOfTwoMeans].setSubtitle(I18n::Message::TestTwoMeansSub);
  m_cells[k_indexOfCategorical].setMessage(I18n::Message::TestCategorical);
  m_cells[k_indexOfCategorical].setSubtitle(I18n::Message::TestCategoricalSub);
}

Escher::HighlightCell * TestController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index <= k_numberOfCells);
  return &m_cells[index];
}

void TestController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}
