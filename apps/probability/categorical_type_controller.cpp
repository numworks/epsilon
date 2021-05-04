#include "categorical_type_controller.h"
#include <apps/i18n.h>

CategoricalTypeController::CategoricalTypeController(Escher::Responder * parent)
  : SelectableListViewController(parent)
{
  m_cells[k_indexOfGoodness].setMessage(I18n::Message::GoodnessOfFit);
  m_cells[k_indexOfHomogeneity].setMessage(I18n::Message::Homogeneity);
}

Escher::HighlightCell * CategoricalTypeController::reusableCell(int i, int type) {
  assert(type == 0);
  assert(i >= 0 && i < k_numberOfCells);
  return &m_cells[i];
}
