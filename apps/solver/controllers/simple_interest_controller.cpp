#include "simple_interest_controller.h"
#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/stack_view_controller.h>
#include <apps/shared/poincare_helpers.h>
#include "../app.h"

using namespace Solver;

// TODO Hugo : Hide the selected parameter (depends on m_data)

SimpleInterestController::SimpleInterestController(Escher::StackViewController * parent, InputEventHandlerDelegate * handler, FinanceData * data) :
    SelectableListViewPage(parent),
    m_operatorDataSource(),
    m_year(&m_selectableTableView, &m_operatorDataSource, this),
    m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()),
    m_data(data) {
  for (size_t i = 0; i < k_numberOfReusableInputs; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
    m_cells[i].setDelegates(handler, this);
  }
  m_year.setMessage(I18n::Message::FinanceYear);
  m_year.setSubLabelMessage(I18n::Message::NumberOfDaysConvention);
}

const char * SimpleInterestController::title() {
  // TODO : I18n::Message::FinanceSolving (%s depends on m_data)
  return "Title todo";
}

void SimpleInterestController::didBecomeFirstResponder() {
  // Init from data
  selectCellAtLocation(0, 0);
  // TODO Hugo : Dropdown selected row depends on m_data
  m_year.dropdown()->selectRow(0);
  m_year.dropdown()->init();
  m_year.reload();
  resetMemoization();
  m_selectableTableView.reloadData(true);
}

bool SimpleInterestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackViewController()->pop();
    return true;
  }
  return false;
}

int SimpleInterestController::reusableCellCount(int type) {
  if (type == k_inputCellType) {
    return k_numberOfReusableInputs;
  }
  return 1;
}

void SimpleInterestController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  assert(type != k_dropdownCellType || &m_year == cell);
  if (type == k_inputCellType) {
    Escher::MessageTableCellWithEditableTextWithMessage * myCell = static_cast<Escher::MessageTableCellWithEditableTextWithMessage *>(cell);
    I18n::Message label;
    I18n::Message sublabel;
    switch (index) {
    case k_indexOfN:
      label = I18n::Message::FinanceLowerN;
      sublabel = I18n::Message::NumberOfDays;
      break;
    case k_indexOfRPct:
      label = I18n::Message::FinanceRPct;
      sublabel = I18n::Message::AnnualInterestRate;
      break;
    case k_indexOfP:
      label = I18n::Message::FinanceP;
      sublabel = I18n::Message::InitialPrincipal;
      break;
    default:
      assert(index == k_indexOfI);
      label = I18n::Message::FinanceI;
      sublabel = I18n::Message::FinalInterestAmount;
    }
    myCell->setMessage(label);
    myCell->setMessage(sublabel);
    // TODO Hugo : Depends on m_data
    myCell->setAccessoryText("0");
  }
}

int SimpleInterestController::typeAtIndex(int index) {
  if (index < k_indexOfYear) {
    return k_inputCellType;
  }
  return (index == k_indexOfYear) ? k_dropdownCellType : k_confirmCellType;
}

KDCoordinate SimpleInterestController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  if (type == k_inputCellType) {
    MessageTableCellWithEditableTextWithMessage tempCell;
    return heightForCellAtIndex(&tempCell, j);
  } else if (type == k_dropdownCellType) {
    return heightForCellAtIndex(&m_year, j);
  }
  assert(type == k_confirmCellType);
  return heightForCellAtIndex(&m_next, j);
}

HighlightCell * SimpleInterestController::reusableCell(int i, int type) {
  switch (type) {
    case k_inputCellType:
      assert(i < k_numberOfReusableInputs);
      return m_cells+i;
    case k_dropdownCellType:
      assert(i == 0);
      return &m_year;
    default:
      assert(type == k_confirmCellType && i == 0);
      return &m_next;
  }
}

bool SimpleInterestController::buttonAction() {
  // TODO Hugo : Load result controller
  assert(false);
  openPage(nullptr);
  return true;
}


bool SimpleInterestController::textFieldShouldFinishEditing(Escher::TextField * textField,
                                                                     Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool SimpleInterestController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                                  const char * text,
                                                                  Ion::Events::Event event) {
  double h0 = Shared::PoincareHelpers::ApproximateToScalar<double>(
      text,
      AppsContainer::sharedAppsContainer()->globalContext());
  // TODO Hugo : Check each parameter's validity
  if (std::isnan(h0)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  // TODO Hugo : Set param in m_data
  m_selectableTableView.selectCellAtLocation(0, selectedRow()+1);
  return true;
}

void SimpleInterestController::onDropdownSelected(int selectedRow) {
  // TODO Hugo : Set param in m_data
  switch (selectedRow) {
    case 0:
      // 360
      break;
    case 1:
      // 365
      break;
  }
}
