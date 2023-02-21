#include "sequence_column_parameter_controller.h"

#include <assert.h>

#include "../app.h"
#include "values_controller.h"

using namespace Escher;
using namespace Shared;

namespace Sequence {

SequenceColumnParameterController::SequenceColumnParameterController(
    ValuesController* valuesController)
    : ColumnParameterController(valuesController),
      m_showSumCell(I18n::Message::ShowSumOfTerms,
                    I18n::Message::ShowSumOfTermsSublabel),
      m_valuesController(valuesController) {
  m_showSumCell.setState(false);
}

Shared::ColumnNameHelper*
SequenceColumnParameterController::columnNameHelper() {
  return m_valuesController;
}

bool SequenceColumnParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedRow() == 0);
    ExpiringPointer<Shared::Sequence> currentSequence =
        GlobalContext::sequenceStore->modelForRecord(m_record);
    bool currentState = currentSequence->displaySum();
    assert(m_showSumCell.state() == currentState);
    m_showSumCell.setState(!currentState);
    currentSequence->setDisplaySum(!currentState);
    return true;
  }
  return false;
}

void SequenceColumnParameterController::willDisplayCellForIndex(
    Escher::HighlightCell* cell, int index) {
  Shared::ColumnParameterController::willDisplayCellForIndex(cell, index);
  assert(index == 0 && cell == &m_showSumCell);
  ExpiringPointer<Shared::Sequence> currentSequence =
      GlobalContext::sequenceStore->modelForRecord(m_record);
  m_showSumCell.setState(currentSequence->displaySum());
}

}  // namespace Sequence
