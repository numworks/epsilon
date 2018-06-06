#include "store_parameter_controller.h"
#include "store_controller.h"
#include <assert.h>

namespace Regression {

StoreParameterController::StoreParameterController(Responder * parentResponder, Store * store, StoreController * storeController) :
  Shared::StoreParameterController(parentResponder, store, storeController),
  m_changeRegression(I18n::Message::ChangeRegression),
  m_regressionController(this, store)
{
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == numberOfRows() - 1) {
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->push(&m_regressionController);
    return true;
  }
  return Shared::StoreParameterController::handleEvent(event);
}

HighlightCell * StoreParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < reusableCellCount());
  if (index == reusableCellCount() - 1) {
    return &m_changeRegression;
  }
  return Shared::StoreParameterController::reusableCell(index);
}

}
