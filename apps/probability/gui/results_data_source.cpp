#include "results_data_source.h"

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <kandinsky/color.h>

#include "probability/app.h"

namespace Probability {

ResultsDataSource::ResultsDataSource(Escher::Responder * parent,
                                     Statistic * statistic,
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

void ResultsDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int i) {
  if (!m_statistic->hasDegreeOfFreedom() && i == k_indexOfDegrees) {
    i++;  // offset to match button index
  }
  if (i < k_indexOfButton) {
    Escher::MessageTableCellWithBuffer * messageCell =
        static_cast<Escher::MessageTableCellWithBuffer *>(cell);
    I18n::Message message;
    float value;

    switch (i) {
      case k_indexOfZ:
        message = I18n::Message::Z;
        value = m_statistic->testCriticalValue();
        break;
      case k_indexOfP:
        message = I18n::Message::PValue;
        value = m_statistic->pValue();
        break;
      case k_indexOfDegrees:
        message = I18n::Message::DegreesOfFreedomDefinition;
        value = m_statistic->degreeOfFreedom();
        break;
    }
    // Parse float
    // TODO do that better
    constexpr int bufferSize = 20;
    char buffer[bufferSize];
    Poincare::PrintFloat::ConvertFloatToText(value, buffer, bufferSize, 10, 5,
                                             Poincare::Preferences::PrintFloatMode::Decimal);
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
