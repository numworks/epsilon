#include "results_data_source.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/layout.h>
#include <poincare/layout_helper.h>

#include "probability/app.h"
#include "probability/constants.h"
#include "probability/helpers.h"

namespace Probability {

ResultsDataSource::ResultsDataSource(Escher::Responder * parent,
                                     Statistic * statistic,
                                     Escher::Invocation invocation,
                                     DynamicCellsDataSourceDelegate<ExpressionCellWithBufferWithMessage> * dynamicCellsDataSourceDelegate) :
      MemoizedListViewDataSource(),
      DynamicCellsDataSource<ExpressionCellWithBufferWithMessage, k_maxNumberOfExpressionCellsWithBufferWithMessage>(dynamicCellsDataSourceDelegate),
      m_statistic(statistic),
      m_next(parent, I18n::Message::Next, invocation)
{
}

int ResultsDataSource::numberOfRows() const {
  Data::SubApp subapp = App::app()->subapp();
  int index = subapp == Data::SubApp::Tests ? 2 : 3;
  // Do not display degreeOfFreedom in Goodness test results.
  index += (m_statistic->hasDegreeOfFreedom() && App::app()->categoricalType() != Data::CategoricalType::Goodness);
  index += subapp == Data::SubApp::Intervals &&
           !m_statistic->estimateLayout().isUninitialized();  // Add estimate cell
  return index + 1 /* button */;
}

KDCoordinate ResultsDataSource::cellWidth() {
  return Ion::Display::Width - Escher::Metric::CommonLeftMargin - Escher::Metric::CommonRightMargin;
}

void ResultsDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int i) {
  if (i < numberOfRows() - 1) {
    ExpressionCellWithBufferWithMessage * messageCell =
        static_cast<ExpressionCellWithBufferWithMessage *>(cell);
    Poincare::Layout message;
    I18n::Message subMessage = I18n::Message::Default;
    double value;
    if (App::app()->subapp() == Data::SubApp::Tests) {
      switch (i) {
        case TestCellOrder::Z:
          message = m_statistic->testCriticalValueSymbol();
          value = m_statistic->testCriticalValue();
          subMessage = I18n::Message::TestStatistic;
          break;
        case TestCellOrder::PValue:
          message = Poincare::LayoutHelper::String(I18n::translate(I18n::Message::PValue));
          value = m_statistic->pValue();
          break;
        case TestCellOrder::TestDegree:
          message = Poincare::LayoutHelper::String(
              I18n::translate(I18n::Message::DegreesOfFreedom));
          value = m_statistic->degreeOfFreedom();
          break;
        default:
          assert(false);
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
          message = Poincare::LayoutHelper::String(I18n::translate(I18n::Message::SE));
          subMessage = I18n::Message::StandardError;
          value = m_statistic->standardError();
          break;
        case IntervalCellOrder::ME:
          message = Poincare::LayoutHelper::String(I18n::translate(I18n::Message::ME));
          subMessage = I18n::Message::MarginOfError;
          value = m_statistic->marginOfError();
          break;
        case IntervalCellOrder::IntervalDegree:
          message = Poincare::LayoutHelper::String(
              I18n::translate(I18n::Message::DegreesOfFreedom));
          value = m_statistic->degreeOfFreedom();
          break;
        default:
          assert(false);
      }
    }
    constexpr int bufferSize = Constants::k_largeBufferSize;
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
        value,
        buffer,
        bufferSize,
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits,
        Poincare::Preferences::PrintFloatMode::Decimal);

    messageCell->setLayout(message);
    messageCell->setAccessoryText(buffer);
    messageCell->setSubLabelMessage(subMessage);
  }
}

Escher::HighlightCell * ResultsDataSource::reusableCell(int index, int type) {
  if (type == k_resultCellType) {
    return cell(index);
  }
  return &m_next;
}

int ResultsDataSource::reusableCellCount(int type) {
  if (type == k_resultCellType) {
    return k_numberOfReusableCells;
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
