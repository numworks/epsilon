#include "categorical_type_controller.h"

#include <apps/i18n.h>
#include <escher/stack_view_controller.h>

#include "../gui/selectable_cell_list_controller.h"

CategoricalTypeController::CategoricalTypeController(Escher::Responder * parent,
                                                     InputGoodnessController * inputGoodnessController)
    : SelectableCellListController(parent), m_inputGoodnessController(inputGoodnessController) {
  m_cells[k_indexOfGoodness].setMessage(I18n::Message::GoodnessOfFit);
  m_cells[k_indexOfHomogeneity].setMessage(I18n::Message::Homogeneity);
}

bool CategoricalTypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Escher::ViewController * view;
    switch (selectedRow()) {
      case k_indexOfGoodness:
      case k_indexOfHomogeneity:
        view = m_inputGoodnessController;
        break;
    }
    assert(view != nullptr);
    Escher::StackViewController * stack = (Escher::StackViewController *)parentResponder();
    stack->pop();  // Pop self
    stack->push(view);
    return true;
  }
  return false;
}
