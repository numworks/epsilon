#include "interest_menu_controller.h"

#include "app.h"

namespace Finance {

InterestMenuController::InterestMenuController(
    Escher::StackViewController* parentResponder,
    ParametersController* interestController)
    : Escher::ListWithTopAndBottomController(parentResponder, &m_messageView),
      m_messageView(I18n::Message::ParameterChoose, k_messageFormat),
      m_parametersController(interestController) {
  selectRow(0);
}

void InterestMenuController::didBecomeFirstResponder() {
  int nRows = numberOfRows();
  for (int i = 0; i < nRows; i++) {
    m_cells[i].label()->setMessage(
        App::GetInterestData()->labelForParameter(paramaterAtIndex(i)));
    m_cells[i].subLabel()->setMessage(
        App::GetInterestData()->sublabelForParameter(paramaterAtIndex(i)));
  }
  resetMemoization();
  m_selectableListView.reloadData();
}

bool InterestMenuController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (m_cells[0].canBeActivatedByEvent(event)) {
    App::GetInterestData()->setUnknown(paramaterAtIndex(innerSelectedRow()));
    stackOpenPage(m_parametersController);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

const char* InterestMenuController::title() {
  return I18n::translate(App::GetInterestData()->menuTitle());
}

int InterestMenuController::numberOfRows() const {
  return App::GetInterestData()->numberOfUnknowns();
}

uint8_t InterestMenuController::paramaterAtIndex(int index) const {
  // Parameters are displayed in the same order as the enum order.
  assert(index >= 0 && index < App::GetInterestData()->numberOfUnknowns());
  return index;
}

}  // namespace Finance
