#include "compound_interest_menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

using namespace Solver;

CompoundInterestMenuController::CompoundInterestMenuController(Escher::StackViewController * parentResponder, CompoundInterestController * compoundInterestController, FinanceData * data) :
      Shared::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfCompoundInterestCells>(parentResponder, &m_contentView), m_contentView(&m_selectableTableView, this, I18n::Message::ParameterChoose), m_compoundInterestController(compoundInterestController), m_data(data) {
  selectRow(0);
  for (size_t i = 0; i < numberOfRows(); i++) {
    m_cells[i].setMessage(CompoundInterestData::LabelForParameter(paramaterAtIndex(i)));
    m_cells[i].setSubtitle(CompoundInterestData::SublabelForParameter(paramaterAtIndex(i)));
  }
}

void CompoundInterestMenuController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

bool CompoundInterestMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_data->m_data.m_compoundInterestData.setUnknown(paramaterAtIndex(selectedRow()));
    openPage(m_compoundInterestController);
  } else if (event == Ion::Events::Left) {
    stackViewController()->pop();
  } else {
    return false;
  }
  return true;
}

CompoundInterestParameter CompoundInterestMenuController::paramaterAtIndex(int index) const {
  // Parameters are displayed in the same order as the enum order.
  assert(index >= 0 && index < k_numberOfCompoundInterestParameters);
  // P/Y, C/Y and Payment parameters aren't displayed in this menu.
  assert(static_cast<CompoundInterestParameter>(index) < CompoundInterestParameter::PY);
  return static_cast<CompoundInterestParameter>(index);
}
