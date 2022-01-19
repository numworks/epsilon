#include "compound_interest_controller.h"
#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/stack_view_controller.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/print.h>
#include "../app.h"

using namespace Solver;

CompoundInterestController::CompoundInterestController(Escher::StackViewController * parent, InputEventHandlerDelegate * handler, FinanceResultController * financeResultController, FinanceData * data) :
    SelectableListViewPage(parent),
    m_paymentDataSource(),
    m_payment(&m_selectableTableView, &m_paymentDataSource, this),
    m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()),
    m_financeResultController(financeResultController),
    m_data(data) {
  for (size_t i = 0; i < k_numberOfReusableInputs; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
    m_cells[i].setDelegates(handler, this);
  }
  m_payment.setMessage(I18n::Message::FinancePayment);
  m_payment.setSubLabelMessage(I18n::Message::BeginningEndPeriod);
}

const char * CompoundInterestController::title() {
  CompoundInterestParameter unknownParam = compoundInterestData()->getUnknown();
  const char * pattern = I18n::translate(I18n::Message::FinanceSolving);
  const char * label = I18n::translate(CompoundInterestData::LabelForParameter(unknownParam));
  const char * sublabel = I18n::translate(CompoundInterestData::SublabelForParameter(unknownParam));
  if (strlen(pattern) + strlen(label) + strlen(sublabel) - 2 * strlen("%s") <= k_titleBufferSize - 1) {
    Poincare::Print::customPrintf(m_titleBuffer, k_titleBufferSize, pattern, label, sublabel);
  } else {
    // If the title does not fit, use a reduced pattern
    Poincare::Print::customPrintf(m_titleBuffer, k_titleBufferSize, I18n::translate(I18n::Message::FinanceSolvingReduced), label);
  }
  return m_titleBuffer;
}

void CompoundInterestController::didBecomeFirstResponder() {
  // Init from data
  selectCellAtLocation(0, 0);
  m_payment.dropdown()->selectRow(compoundInterestData()->m_paymentIsBeginning ? 0 : 1);
  m_payment.dropdown()->init();
  m_payment.reload();
  resetMemoization();
  m_selectableTableView.reloadData(true);
}

bool CompoundInterestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackViewController()->pop();
    return true;
  }
  return false;
}

int CompoundInterestController::reusableCellCount(int type) {
  if (type == k_inputCellType) {
    return k_numberOfReusableInputs;
  }
  return 1;
}

void CompoundInterestController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  int type = typeAtIndex(index);
  assert(type != k_dropdownCellType || &m_payment == cell);
  if (type == k_inputCellType) {
    Escher::MessageTableCellWithEditableTextWithMessage * myCell = static_cast<Escher::MessageTableCellWithEditableTextWithMessage *>(cell);
    CompoundInterestParameter param = paramaterAtIndex(index);
    myCell->setMessage(CompoundInterestData::LabelForParameter(param));
    myCell->setSubLabelMessage(CompoundInterestData::SublabelForParameter(param));
    double value = compoundInterestData()->getValue(param);
    constexpr int precision = Poincare::Preferences::LargeNumberOfSignificantDigits;
    constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
    myCell->setAccessoryText(buffer);
  }
}

int CompoundInterestController::typeAtIndex(int index) {
  if (index < k_indexOfPayment) {
    return k_inputCellType;
  }
  return (index == k_indexOfPayment) ? k_dropdownCellType : k_confirmCellType;
}

KDCoordinate CompoundInterestController::nonMemoizedRowHeight(int j) {
  int type = typeAtIndex(j);
  if (type == k_inputCellType) {
    MessageTableCellWithEditableTextWithMessage tempCell;
    return heightForCellAtIndex(&tempCell, j);
  } else if (type == k_dropdownCellType) {
    return heightForCellAtIndex(&m_payment, j);
  }
  assert(type == k_confirmCellType);
  return heightForCellAtIndex(&m_next, j);
}

HighlightCell * CompoundInterestController::reusableCell(int i, int type) {
  switch (type) {
    case k_inputCellType:
      assert(i < k_numberOfReusableInputs);
      return m_cells+i;
    case k_dropdownCellType:
      assert(i == 0);
      return &m_payment;
    default:
      assert(type == k_confirmCellType && i == 0);
      return &m_next;
  }
}

bool CompoundInterestController::buttonAction() {
  openPage(m_financeResultController);
  return true;
}


bool CompoundInterestController::textFieldShouldFinishEditing(Escher::TextField * textField,
                                                                     Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool CompoundInterestController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                                  const char * text,
                                                                  Ion::Events::Event event) {
  CompoundInterestParameter param = paramaterAtIndex(selectedRow());
  double value = Shared::PoincareHelpers::ApproximateToScalar<double>(
      text,
      AppsContainer::sharedAppsContainer()->globalContext());
  if (!CompoundInterestData::CheckValue(param, value)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  compoundInterestData()->setValue(param, value);
  m_selectableTableView.reloadData();
  // Select next cell
  m_selectableTableView.selectCellAtLocation(0, selectedRow()+1);
  return true;
}

void CompoundInterestController::onDropdownSelected(int selectedRow) {
  compoundInterestData()->m_paymentIsBeginning = (selectedRow == 0);
}

CompoundInterestParameter CompoundInterestController::paramaterAtIndex(int index) const {
  if (index == k_indexOfPayment) {
    return CompoundInterestParameter::Payment;
  }
  CompoundInterestParameter unknownParam = compoundInterestData()->getUnknown();
  assert(unknownParam != CompoundInterestParameter::Payment);
  if (static_cast<int>(unknownParam) <= index) {
    index += 1;
  }
  assert(index < k_numberOfCompoundInterestParameters);
  return static_cast<CompoundInterestParameter>(index);
}
