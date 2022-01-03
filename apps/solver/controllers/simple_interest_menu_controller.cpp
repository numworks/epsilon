#include "simple_interest_menu_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>

using namespace Solver;

SimpleInterestMenuController::SimpleInterestMenuController(Escher::StackViewController * parentResponder, SimpleInterestController * simpleInterestController, FinanceData * data) :
      SelectableCellListPage(parentResponder), m_simpleInterestController(simpleInterestController), m_data(data) {
  selectRow(0);
  for (size_t i = 0; i < numberOfRows(); i++) {
    m_cells[i].setMessage(SimpleInterestData::LabelForParameter(paramaterAtIndex(i)));
    m_cells[i].setSubtitle(SimpleInterestData::SublabelForParameter(paramaterAtIndex(i)));
  }
}

void SimpleInterestMenuController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
}

bool SimpleInterestMenuController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_data->m_data.m_simpleInterestData.m_unknown = paramaterAtIndex(selectedRow());
    openPage(m_simpleInterestController);
  } else if (event == Ion::Events::Left) {
    stackViewController()->pop();
  } else {
    return false;
  }
  return true;
}

SimpleInterestParameter SimpleInterestMenuController::paramaterAtIndex(int index) const {
  // Parameters are displayed in the same order as the enum order.
  assert(index >= 0 && index < k_numberOfSimpleInterestParameters);
  // Year convention parameter insn't displayed in this menu.
  assert(static_cast<SimpleInterestParameter>(index) != SimpleInterestParameter::YearConvention);
  return static_cast<SimpleInterestParameter>(index);
}
