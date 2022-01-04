#include "finance_result_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/print.h>
#include <escher/clipboard.h>

using namespace Solver;

FinanceResultController::FinanceResultController(Escher::StackViewController * parentResponder, FinanceData * data) :
      SelectableCellListPage(parentResponder), m_data(data) {
}

void FinanceResultController::didBecomeFirstResponder() {
  selectRow(-1);
  double value;
  if (m_data->isSimpleInterest) {
    SimpleInterestParameter unknownParam = simpleInterestData()->getUnknown();
    m_cells[0].setMessage(SimpleInterestData::LabelForParameter(unknownParam));
    m_cells[0].setSubLabelMessage(SimpleInterestData::SublabelForParameter(unknownParam));
    value = simpleInterestData()->computeUnknownValue();
  } else {
    CompoundInterestParameter unknownParam = compoundInterestData()->getUnknown();
    m_cells[0].setMessage(CompoundInterestData::LabelForParameter(unknownParam));
    m_cells[0].setSubLabelMessage(CompoundInterestData::SublabelForParameter(unknownParam));
    value = compoundInterestData()->computeUnknownValue();
  }
  constexpr int precision = Poincare::Preferences::LargeNumberOfSignificantDigits;
  constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
  m_cells[0].setAccessoryText(buffer);
  m_selectableTableView.reloadData();
}

bool FinanceResultController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    Escher::Clipboard::sharedClipboard()->store(m_cells[0].text());
  } else if (event == Ion::Events::Left) {
    stackViewController()->pop();
  } else {
    return false;
  }
  return true;
}

const char * FinanceResultController::title() {
  // SimpleInterestParameter unknownParam = simpleInterestData()->getUnknown();
  // Poincare::Print::customPrintf(m_titleBuffer, k_titleBufferSize,
  //   I18n::translate(I18n::Message::FinanceSolving),
  //   I18n::translate(SimpleInterestData::LabelForParameter(unknownParam)),
  //   I18n::translate(SimpleInterestData::SublabelForParameter(unknownParam)));
  // return m_titleBuffer;
  return "TODO";
}
