
#include "column_parameter_controller.h"

#include <escher/app.h>

namespace Statistics::Categorical {

bool ColumnParameterController::handleEvent(Ion::Events::Event event) {
  Escher::AbstractMenuCell* cell =
      static_cast<Escher::AbstractMenuCell*>(selectedCell());
  assert(m_column >= 0);
  if (!cell->canBeActivatedByEvent(event)) {
    return false;
  }
  if (cell == &m_clearColumnCell) {
    m_tableData->clearColumn(m_column);
    m_stackViewController->pop();
    return true;
  }
  if (cell == &m_showInGraphCell) {
    if (m_tableData->isGroupEmpty(m_column)) {
      Escher::App::app()->displayWarning(I18n::Message::DataNotSuitable);
    } else {
      bool newState = !m_tableData->isGroupActive(m_column);
      m_tableData->setGroupActive(newState, m_column);
      m_showInGraphCell.accessory()->setState(newState);
      m_selectableListView.reloadSelectedCell();
    }
    return true;
  }
  if (cell == &m_columnNameCell) {
    // TODO
  }
  if (cell == &m_relativeFreqCell) {
    // TODO
  }
  return false;
}

}  // namespace Statistics::Categorical
