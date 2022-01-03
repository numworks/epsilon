#include "simple_interest_controller.h"
#include <apps/apps_container.h>
#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/stack_view_controller.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/print.h>
#include "../app.h"

using namespace Solver;

SimpleInterestController::SimpleInterestController(Escher::StackViewController * parent, InputEventHandlerDelegate * handler, FinanceResultController * financeResultController, FinanceData * data) :
    SelectableListViewPage(parent),
    m_operatorDataSource(),
    m_year(&m_selectableTableView, &m_operatorDataSource, this),
    m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()),
    m_financeResultController(financeResultController),
    m_data(data) {
  for (size_t i = 0; i < k_numberOfReusableInputs; i++) {
    m_cells[i].setParentResponder(&m_selectableTableView);
    m_cells[i].setDelegates(handler, this);
  }
  m_year.setMessage(I18n::Message::FinanceYear);
  m_year.setSubLabelMessage(I18n::Message::NumberOfDaysConvention);
}

const char * SimpleInterestController::title() {
  SimpleInterestParameter unknownParam = simpleInterestData()->getUnknown();
  Poincare::Print::customPrintf(m_titleBuffer, k_titleBufferSize,
    I18n::translate(I18n::Message::FinanceSolving),
    I18n::translate(SimpleInterestData::LabelForParameter(unknownParam)),
    I18n::translate(SimpleInterestData::SublabelForParameter(unknownParam)));
  return m_titleBuffer;
}

void SimpleInterestController::didBecomeFirstResponder() {
  // Init from data
  selectCellAtLocation(0, 0);
  m_year.dropdown()->selectRow(simpleInterestData()->m_yearConventionIs360 ? 0 : 1);
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
    SimpleInterestParameter param = paramaterAtIndex(index);
    myCell->setMessage(SimpleInterestData::LabelForParameter(param));
    myCell->setSubLabelMessage(SimpleInterestData::SublabelForParameter(param));
    double value = simpleInterestData()->getValue(param);
    constexpr int precision = Poincare::Preferences::LargeNumberOfSignificantDigits;
    constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
    myCell->setAccessoryText(buffer);
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
  openPage(m_financeResultController);
  return true;
}


bool SimpleInterestController::textFieldShouldFinishEditing(Escher::TextField * textField,
                                                                     Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool SimpleInterestController::textFieldDidFinishEditing(Escher::TextField * textField,
                                                                  const char * text,
                                                                  Ion::Events::Event event) {
  SimpleInterestParameter param = paramaterAtIndex(selectedRow());
  double value = Shared::PoincareHelpers::ApproximateToScalar<double>(
      text,
      AppsContainer::sharedAppsContainer()->globalContext());
  if (!SimpleInterestData::CheckValue(param, value)) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  simpleInterestData()->setValue(param, value);
  m_selectableTableView.reloadData();
  // Select next cell
  m_selectableTableView.selectCellAtLocation(0, selectedRow()+1);
  return true;
}

void SimpleInterestController::onDropdownSelected(int selectedRow) {
  simpleInterestData()->m_yearConventionIs360 = (selectedRow == 0);
}

SimpleInterestParameter SimpleInterestController::paramaterAtIndex(int index) const {
  if (index == k_indexOfYear) {
    return SimpleInterestParameter::YearConvention;
  }
  SimpleInterestParameter unknownParam = simpleInterestData()->getUnknown();
  assert(unknownParam != SimpleInterestParameter::YearConvention);
  if (static_cast<int>(unknownParam) <= index) {
    index += 1;
  }
  assert(index < k_numberOfSimpleInterestParameters);
  return static_cast<SimpleInterestParameter>(index);
}
