#include "results_data_source.h"

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <kandinsky/color.h>

#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

ResultsDataSource::ResultsDataSource(Escher::Responder * parent, Statistic * statistic,
                                     ButtonDelegate * delegate) :
    MemoizedListViewDataSource(),
    m_statistic(statistic),
    m_next(parent, I18n::Message::Ok, delegate->buttonActionInvocation()) {
  Escher::BufferTextView * buffer;
  // Correct cell styles
  for (int i = 0; i < sizeof(m_resultCells) / sizeof(Escher::MessageTableCellWithBuffer); i++) {
    buffer = static_cast<Escher::BufferTextView *>(
        const_cast<Escher::View *>(m_resultCells[i].subLabelView()));
    buffer->setFont(KDFont::LargeFont);
    buffer->setTextColor(KDColorBlack);
  }
}

int ResultsDataSource::numberOfRows() const {
  Data::SubApp subapp = App::app()->subapp();
  int index = subapp == Data::SubApp::Tests ? 2 : 4;
  index += m_statistic->hasDegreeOfFreedom();
  return index + 1 /* button */;
}

void ResultsDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int i) {
  if (i < numberOfRows() - 1) {
    Escher::MessageTableCellWithBuffer * messageCell =
        static_cast<Escher::MessageTableCellWithBuffer *>(cell);
    I18n::Message message;
    float value;
    if (App::app()->subapp() == Data::SubApp::Tests) {
      switch (i) {
        case TestCellOrder::Z:
          message = I18n::Message::Z;  // TODO use testCriticalValueSymbol
          value = m_statistic->testCriticalValue();
          break;
        case TestCellOrder::PValue:
          message = I18n::Message::PValue;
          value = m_statistic->pValue();
          break;
        case TestCellOrder::TestDegree:
          message = I18n::Message::DegreesOfFreedom;
          value = m_statistic->degreeOfFreedom();
          break;
      }
    } else {
      switch (i) {
        case IntervalCellOrder::Estimate:
          message = I18n::Message::P;
          value = m_statistic->estimate();
          break;
        case IntervalCellOrder::Critical:
          message = I18n::Message::Z;  // TODO use m_statistic->intervalCriticalValueSymbol();
          value = m_statistic->intervalCriticalValue();
          break;
        case IntervalCellOrder::SE:
          message = I18n::Message::SE;
          value = m_statistic->standardError();
          break;
        case IntervalCellOrder::ME:
          message = I18n::Message::ME;
          value = m_statistic->marginOfError();
          break;
        case IntervalCellOrder::IntervalDegree:
          message = I18n::Message::DegreesOfFreedom;
          value = m_statistic->degreeOfFreedom();
          break;
      }
    }
    constexpr int bufferSize = 20;
    char buffer[bufferSize];
    defaultParseFloat(value, buffer, bufferSize);

    messageCell->setMessage(message);
    messageCell->setSubLabelText(buffer);
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
    return sizeof(m_resultCells) / sizeof(Escher::MessageTableCellWithBuffer);
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
