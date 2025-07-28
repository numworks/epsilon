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

void InterestMenuController::viewWillAppear() {
  int nRows = numberOfRows();
  for (int i = 0; i < nRows; i++) {
    m_cells[i].label()->setMessage(
        App::GetInterestData()->labelForParameter(parameterAtIndex(i)));
    m_cells[i].subLabel()->setMessage(
        App::GetInterestData()->sublabelForParameter(parameterAtIndex(i)));
  }
  m_selectableListView.reloadData(false);
  ViewController::viewWillAppear();
}

bool InterestMenuController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (m_cells[0].canBeActivatedByEvent(event)) {
    App::GetInterestData()->setUnknown(parameterAtIndex(innerSelectedRow()));
    stackOpenPage(m_parametersController);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

const char* InterestMenuController::title() const {
  return I18n::translate(App::GetInterestData()->menuTitle());
}

int InterestMenuController::numberOfRows() const {
  return App::GetInterestData()->numberOfUnknowns();
}

KDCoordinate InterestMenuController::nonMemoizedRowHeight(int row) {
  return protectedNonMemoizedRowHeight(&m_cells[row], row);
}

uint8_t InterestMenuController::parameterAtIndex(int index) const {
  // Parameters are displayed in the same order as the enum order.
  assert(index >= 0 && index < App::GetInterestData()->numberOfUnknowns());
  return index;
}

}  // namespace Finance
