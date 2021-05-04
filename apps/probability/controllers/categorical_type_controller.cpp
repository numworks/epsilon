#include "categorical_type_controller.h"
#include <apps/i18n.h>
#include "../gui/selectable_cell_list_controller.h"

CategoricalTypeController::CategoricalTypeController(Escher::Responder * parent)
  : SelectableCellListController(parent)
{
  m_cells[k_indexOfGoodness].setMessage(I18n::Message::GoodnessOfFit);
  m_cells[k_indexOfHomogeneity].setMessage(I18n::Message::Homogeneity);
}
