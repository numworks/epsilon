#include "interest_controller.h"
#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/stack_view_controller.h>
#include <poincare/print.h>
#include "../app.h"

using namespace Solver;

InterestController::InterestController(Escher::StackViewController * parent, Escher::InputEventHandlerDelegate * handler, FinanceResultController * financeResultController, InterestData * data) :
    Escher::SelectableListViewController(parent),
    m_dropdownCell(&m_selectableTableView, &m_dropdownDataSource, this),
    m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()),
    m_financeResultController(financeResultController),
    m_data(data) {
  for (size_t i = 0; i < k_numberOfReusableInputs; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
    m_cells[i].setDelegates(handler, this);
  }
}

const char * InterestController::title() {
  uint8_t unknownParam = m_data->getUnknown();
  const char * pattern = I18n::translate(I18n::Message::FinanceSolving);
  const char * label = I18n::translate(m_data->labelForParameter(unknownParam));
  const char * sublabel = I18n::translate(m_data->sublabelForParameter(unknownParam));
  if (strlen(pattern) + strlen(label) + strlen(sublabel) - 2 * strlen("%s") < k_titleBufferSize) {
    Poincare::Print::customPrintf(m_titleBuffer, k_titleBufferSize, pattern, label, sublabel);
  } else {
    // If the title does not fit, use a reduced pattern
    Poincare::Print::customPrintf(m_titleBuffer, k_titleBufferSize, I18n::translate(I18n::Message::FinanceSolvingReduced), label);
  }
  return m_titleBuffer;
}

void InterestController::didBecomeFirstResponder() {
  // Init from data
  m_dropdownDataSource.setMessages(m_data->dropdownMessageAtIndex(0),
                                   m_data->dropdownMessageAtIndex(1));
  selectCellAtLocation(0, 0);
  m_dropdownCell.dropdown()->selectRow(m_data->m_booleanParam ? 0 : 1);
  m_dropdownCell.dropdown()->init();
  m_dropdownCell.reload();
  resetMemoization();
  m_selectableTableView.reloadData(true);
}

bool InterestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    popStackViewControllerParentResponder();
    return true;
  }
  return false;
}

int InterestController::reusableCellCount(int type) {
  if (type == k_inputCellType) {
    return k_numberOfReusableInputs;
  }
  return 1;
}

void InterestController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  if (type == k_confirmCellType) {
    return;
  }
  uint8_t param = paramaterAtIndex(index);
  if (type == k_dropdownCellType) {
    assert(&m_dropdownCell == cell);
    m_dropdownCell.setMessage(m_data->labelForParameter(param));
    m_dropdownCell.setSubLabelMessage(m_data->sublabelForParameter(param));
    return;
  }
  Escher::MessageTableCellWithEditableTextWithMessage * myCell = static_cast<Escher::MessageTableCellWithEditableTextWithMessage *>(cell);
  myCell->setMessage(m_data->labelForParameter(param));
  myCell->setSubLabelMessage(m_data->sublabelForParameter(param));
  double value = m_data->getValue(param);
  constexpr int precision = Poincare::Preferences::LargeNumberOfSignificantDigits;
  constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
  myCell->setAccessoryText(buffer);
}

int InterestController::typeAtIndex(int index) {
  if (index < indexOfDropdown()) {
    return k_inputCellType;
  }
  return (index == indexOfDropdown()) ? k_dropdownCellType : k_confirmCellType;
}

KDCoordinate InterestController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  if (type == k_inputCellType) {
    Escher::MessageTableCellWithEditableTextWithMessage tempCell;
    return heightForCellAtIndex(&tempCell, j);
  } else if (type == k_dropdownCellType) {
    return heightForCellAtIndex(&m_dropdownCell, j);
  }
  assert(type == k_confirmCellType);
  return heightForCellAtIndex(&m_next, j);
}

Escher::HighlightCell * InterestController::reusableCell(int i, int type) {
  switch (type) {
    case k_inputCellType:
      assert(i < k_numberOfReusableInputs);
      return m_cells + i;
    case k_dropdownCellType:
      assert(i == 0);
      return &m_dropdownCell;
    default:
      assert(type == k_confirmCellType && i == 0);
      return &m_next;
  }
}

bool InterestController::buttonAction() {
  m_financeResultController->setData(m_data);
  stackOpenPage(m_financeResultController, 1);
  return true;
}

bool InterestController::textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool InterestController::textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) {
  uint8_t param = paramaterAtIndex(selectedRow());
  double value = Shared::PoincareHelpers::ApproximateToScalar<double>(text, AppsContainer::sharedAppsContainer()->globalContext());
  if (!m_data->checkValue(param, value)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  m_data->setValue(param, value);
  m_selectableTableView.reloadData();
  // Select next cell
  m_selectableTableView.selectCellAtLocation(0, selectedRow() + 1);
  return true;
}

void InterestController::onDropdownSelected(int selectedRow) {
  m_data->m_booleanParam = (selectedRow == 0);
}

uint8_t InterestController::paramaterAtIndex(int index) const {
  uint8_t unknownParam = m_data->getUnknown();
  assert(unknownParam < m_data->numberOfUnknowns());
  if (unknownParam <= index) {
    index += 1;
  }
  assert(index < m_data->numberOfParameters());
  return index;
}
