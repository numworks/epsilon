#include "rf_column_parameter_controller.h"

#include <escher/app.h>

namespace Statistics::Categorical {

bool RFColumnParameterController::handleEvent(Ion::Events::Event event) {
  Escher::AbstractMenuCell* cell =
      static_cast<Escher::AbstractMenuCell*>(selectedCell());
  assert(m_group >= 0);
  if (!cell->canBeActivatedByEvent(event)) {
    return false;
  }
  if (cell == &m_clearColumnCell) {
    m_store->setRelativeFrequencyColumn(m_group, false);
    m_stackViewController->pop();
    return true;
  }
  return false;
}

void RFColumnParameterController::setGroup(int group) {
  m_group = group;
  char buffer[20];
  m_store->getGroupName(group, buffer, sizeof(buffer));
  m_selectableListView.selectRow(0);
}
const char* RFColumnParameterController::title() const {
  char buffer[20];
  m_store->getGroupName(m_group, buffer, sizeof(buffer));
  Poincare::Print::CustomPrintf(
      m_titleBuffer, sizeof(m_titleBuffer),
      I18n::translate(I18n::Message::ColumnOptions),
      I18n::translate(I18n::Message::RelativeFrequencyColumnName));
  return m_titleBuffer;
}

}  // namespace Statistics::Categorical
