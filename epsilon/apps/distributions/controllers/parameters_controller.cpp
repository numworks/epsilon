#include "parameters_controller.h"

#include <assert.h>

#include "distributions/app.h"

using namespace Escher;

namespace Distributions {

/* Parameters Controller */

ParametersController::ParametersController(
    Escher::StackViewController* parentResponder, Distribution* distribution,
    CalculationController* calculationController)
    : FloatParameterController<double>(parentResponder, &m_headerView),
      m_headerView(I18n::Message::DefineParameters, k_messageFormat),
      m_bottomView(I18n::Message::LeaveAFieldEmpty, k_messageFormat),
      m_distribution(distribution),
      m_calculationController(calculationController) {
  assert(m_distribution != nullptr);
  m_okButton.setMessage(I18n::Message::Next);
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_menuListCell[i].setParentResponder(&m_selectableListView);
    m_menuListCell[i].setDelegate(this);
  }
}

const char* ParametersController::title() const {
  return I18n::translate(m_distribution->title());
}

bool ParametersController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

void ParametersController::reinitCalculation() {
  m_calculationController->reinitCalculation();
}

void ParametersController::viewWillAppear() {
  if (m_distribution->canHaveUninitializedParameter()) {
    setBottomView(&m_bottomView);
  } else {
    setBottomView(nullptr);
  }
  FloatParameterController::viewWillAppear();
}

int ParametersController::numberOfRows() const {
  return 1 + m_distribution->numberOfParameters();
}

void ParametersController::fillCellForRow(HighlightCell* cell, int row) {
  if (typeAtRow(row) == k_buttonCellType) {
    return;
  }
  assert(typeAtRow(row) == k_parameterCellType);
  MenuCellWithEditableText<LayoutView, MessageTextView>* myCell =
      static_cast<MenuCellWithEditableText<LayoutView, MessageTextView>*>(cell);
  myCell->label()->setLayout(m_distribution->parameterSymbolAtIndex(row));
  myCell->subLabel()->setMessage(
      m_distribution->parameterDefinitionAtIndex(row));
  if (m_distribution->uninitializedParameterIndex() == row) {
    textFieldOfCellAtIndex(cell, row)->setText("");
    return;
  }
  FloatParameterController::fillCellForRow(cell, row);
}

KDCoordinate ParametersController::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  if (type == k_parameterCellType) {
    MenuCellWithEditableText<LayoutView, MessageTextView> tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  }
  assert(type == k_buttonCellType);
  return Shared::FloatParameterController<double>::nonMemoizedRowHeight(row);
}

HighlightCell* ParametersController::reusableParameterCell(int index,
                                                           int type) {
  assert(index >= 0);
  assert(index < k_maxNumberOfCells);
  return &m_menuListCell[index];
}

TextField* ParametersController::textFieldOfCellAtIndex(HighlightCell* cell,
                                                        int index) {
  assert(typeAtRow(index) == k_parameterCellType);
  return static_cast<MenuCellWithEditableText<LayoutView, MessageTextView>*>(
             cell)
      ->textField();
}

int ParametersController::reusableParameterCellCount(int type) const {
  return m_distribution->numberOfParameters();
}

double ParametersController::parameterAtIndex(int index) {
  return m_distribution->parameterAtIndex(index);
}

bool ParametersController::setParameterAtIndex(int parameterIndex, double f) {
  if (!m_distribution->authorizedParameterAtIndex(f, parameterIndex)) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_distribution->setParameterAtIndex(f, parameterIndex);
  reinitCalculation();
  return true;
}

bool ParametersController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  if (FloatParameterController::textFieldDidFinishEditing(textField, event)) {
    m_selectableListView.reloadData();
    return true;
  }
  return false;
}

void ParametersController::buttonAction() {
  stackOpenPage(m_calculationController);
}

bool ParametersController::hasUndefinedValue(const char* text,
                                             double floatValue, int row) const {
  if (text[0] == 0) {
    // Accept empty inputs
    return false;
  }
  return Shared::FloatParameterController<double>::hasUndefinedValue(
      text, floatValue, row);
}

}  // namespace Distributions
