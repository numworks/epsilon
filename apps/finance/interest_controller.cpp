#include "interest_controller.h"
#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/stack_view_controller.h>
#include <poincare/print.h>
#include "app.h"

using namespace Finance;

InterestController::InterestController(Escher::StackViewController * parent, Escher::InputEventHandlerDelegate * handler, FinanceResultController * financeResultController, InterestData * data) :
    Shared::FloatParameterController<double>(parent),
    m_dropdownCell(&m_selectableTableView, &m_dropdownDataSource, this),
    m_financeResultController(financeResultController),
    m_data(data) {
  for (size_t i = 0; i < k_numberOfReusableInputs; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
    m_cells[i].setDelegates(handler, this);
  }
}

const char * InterestController::title() {
  uint8_t unknownParam = m_data->getUnknown();
  const char * label = I18n::translate(m_data->labelForParameter(unknownParam));
  int length = Poincare::Print::safeCustomPrintf(m_titleBuffer, k_titleBufferSize, I18n::translate(I18n::Message::FinanceSolving), label, I18n::translate(m_data->sublabelForParameter(unknownParam)));
  if (length >= k_titleBufferSize) {
    // Title did not fit, use a reduced pattern
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
  return popFromStackViewControllerOnLeftEvent(event);
}

void InterestController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  if (type == k_buttonCellType) {
    return;
  }
  uint8_t param = interestParamaterAtIndex(index);
  if (type == k_dropdownCellType) {
    assert(&m_dropdownCell == cell);
    m_dropdownCell.setMessage(m_data->labelForParameter(param));
    m_dropdownCell.setSubLabelMessage(m_data->sublabelForParameter(param));
    return;
  }
  Escher::MessageTableCellWithEditableTextWithMessage * myCell = static_cast<Escher::MessageTableCellWithEditableTextWithMessage *>(cell);
  myCell->setMessage(m_data->labelForParameter(param));
  myCell->setSubLabelMessage(m_data->sublabelForParameter(param));
  return Shared::FloatParameterController<double>::willDisplayCellForIndex(cell, index);
}

int InterestController::typeAtIndex(int index) {
  if (index < indexOfDropdown()) {
    return k_inputCellType;
  }
  return (index == indexOfDropdown()) ? k_dropdownCellType : k_buttonCellType;
}

KDCoordinate InterestController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  if (type == k_inputCellType) {
    Escher::MessageTableCellWithEditableTextWithMessage tempCell;
    return heightForCellAtIndexWithWidthInit(&tempCell, j);
  } else if (type == k_dropdownCellType) {
    return heightForCellAtIndex(&m_dropdownCell, j);
  }
  assert(type == k_buttonCellType);
  return Shared::FloatParameterController<double>::nonMemoizedRowHeight(j);
}

void InterestController::onDropdownSelected(int selectedRow) {
  m_data->m_booleanParam = (selectedRow == 0);
}

uint8_t InterestController::interestParamaterAtIndex(int index) const {
  uint8_t unknownParam = m_data->getUnknown();
  assert(unknownParam < m_data->numberOfUnknowns());
  if (unknownParam <= index) {
    index += 1;
  }
  assert(index < m_data->numberOfParameters());
  return index;
}

int InterestController::reusableParameterCellCount(int type) {
  if (type == k_inputCellType) {
    return k_numberOfReusableInputs;
  }
  return 1;
}

Escher::HighlightCell * InterestController::reusableParameterCell(int i, int type) {
  switch (type) {
    case k_inputCellType:
      assert(i < k_numberOfReusableInputs);
      return m_cells + i;
    default:
      assert(type == k_dropdownCellType && i == 0);
      return &m_dropdownCell;
  }
}

bool InterestController::setParameterAtIndex(int parameterIndex, double f) {
  uint8_t param = interestParamaterAtIndex(parameterIndex);
  if (!m_data->checkValue(param, f)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  m_data->setValue(param, f);
  return true;
}