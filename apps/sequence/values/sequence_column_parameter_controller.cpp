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
      m_valuesController(valuesController) {
  m_showSumCell.accessory()->setState(false);
  m_showSumCell.label()->setMessage(I18n::Message::ShowSumOfTerms);
  m_showSumCell.subLabel()->setMessage(I18n::Message::ShowSumOfTermsSublabel);
}

Shared::ColumnNameHelper*
SequenceColumnParameterController::columnNameHelper() {
  return m_valuesController;
}

bool SequenceColumnParameterController::handleEvent(Ion::Events::Event event) {
  assert(selectedRow() == 0);
  if (m_showSumCell.enterOnEvent(event)) {
    ExpiringPointer<Shared::Sequence> currentSequence =
        GlobalContext::sequenceStore->modelForRecord(m_record);
    bool currentState = currentSequence->displaySum();
    assert(m_showSumCell.accessory()->state() == currentState);
    m_showSumCell.accessory()->setState(!currentState);
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
  m_showSumCell.accessory()->setState(currentSequence->displaySum());
}

}  // namespace Sequence
