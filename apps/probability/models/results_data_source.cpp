#include "results_data_source.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/buffer_text_view.h>
#include <kandinsky/color.h>
#include <poincare/layout.h>

#include "probability/app.h"
#include "probability/constants.h"
#include "probability/helpers.h"
#include "probability/text_helpers.h"

namespace Probability {

ResultsDataSource::ResultsDataSource(Escher::Responder * parent,
                                     Statistic * statistic,
                                     ButtonDelegate * delegate) :
    MemoizedListViewDataSource(),
    m_statistic(statistic),
    m_next(parent, I18n::Message::Ok, delegate->buttonActionInvocation()) {
}

int ResultsDataSource::numberOfRows() const {
  Data::SubApp subapp = App::app()->subapp();
  int index = subapp == Data::SubApp::Tests ? 2 : 3;
  index += m_statistic->hasDegreeOfFreedom();
  index += subapp == Data::SubApp::Intervals &&
           !m_statistic->estimateLayout().isUninitialized();  // Add estimate cell
  return index + 1 /* button */;
}

void ResultsDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int i) {
  if (i < numberOfRows() - 1) {
    LayoutCellWithBufferWithMessage * messageCell = static_cast<LayoutCellWithBufferWithMessage *>(
        cell);
    Poincare::Layout message;
    I18n::Message subMessage = I18n::Message::Default;
    float value;
    if (App::app()->subapp() == Data::SubApp::Tests) {
      switch (i) {
        case TestCellOrder::Z:
          message = m_statistic->testCriticalValueSymbol();
          value = m_statistic->testCriticalValue();
          break;
        case TestCellOrder::PValue:
          message = layoutFromText(I18n::translate(I18n::Message::PValue));
          value = m_statistic->pValue();
          break;
        case TestCellOrder::TestDegree:
          message = layoutFromText(I18n::translate(I18n::Message::DegreesOfFreedom));
          value = m_statistic->degreeOfFreedom();
          break;
      }
    } else {
      if (m_statistic->estimateLayout().isUninitialized()) {
        // Estimate cell is not displayed -> shift i
        i++;
      }
      switch (i) {
        case IntervalCellOrder::Estimate:
          message = m_statistic->estimateLayout();
          subMessage = m_statistic->estimateDescription();
          value = m_statistic->estimate();
          break;
        case IntervalCellOrder::Critical:
          message = m_statistic->intervalCriticalValueSymbol();
          subMessage = I18n::Message::CriticalValue;
          value = m_statistic->intervalCriticalValue();
          break;
        case IntervalCellOrder::SE:
          message = layoutFromText(I18n::translate(I18n::Message::SE));
          subMessage = I18n::Message::StandardError;
          value = m_statistic->standardError();
          break;
        case IntervalCellOrder::ME:
          message = layoutFromText(I18n::translate(I18n::Message::ME));
          subMessage = I18n::Message::MarginOfError;
          value = m_statistic->marginOfError();
          break;
        case IntervalCellOrder::IntervalDegree:
          message = layoutFromText(I18n::translate(I18n::Message::DegreesOfFreedom));
          subMessage = I18n::Message::DegreesOfFreedom;
          value = m_statistic->degreeOfFreedom();
          break;
      }
    }
    constexpr int bufferSize = Constants::k_largeBufferSize;
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
        value,
        buffer,
        bufferSize,
        Poincare::Preferences::LargeNumberOfSignificantDigits,
        Poincare::Preferences::PrintFloatMode::Decimal);

    messageCell->setLayout(message);
    messageCell->setAccessoryText(buffer);
    messageCell->setSubLabelMessage(subMessage);
  }
}

Escher::HighlightCell * ResultsDataSource::reusableCell(int index, int type) {
  if (type == k_resultCellType) {
    return &m_resultCells[index];
  }
  return &m_next;
}

int ResultsDataSource::reusableCellCount(int type) {
  if (type == k_resultCellType) {
    return sizeof(m_resultCells) / sizeof(LayoutCellWithBufferWithMessage);
  }
  return 1;
}

int ResultsDataSource::typeAtIndex(int index) {
  if (index == numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return k_resultCellType;
}

}  // namespace Probability
