#include "parameters_controller.h"

#include <assert.h>

#include "distributions/app.h"

using namespace Escher;

namespace Distributions {

/* Parameters Controller */

ParametersController::ParametersController(Escher::StackViewController * parentResponder,
                                           InputEventHandlerDelegate * inputEventHandlerDelegate,
                                           Distribution * distribution,
                                           CalculationController * calculationController) :
      FloatParameterController<double>(parentResponder),
      m_contentView(&m_selectableTableView, I18n::Message::DefineParameters),
      m_distribution(distribution),
      m_calculationController(calculationController) {
  assert(m_distribution != nullptr);
  m_okButton.setMessage(I18n::Message::Next);
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_menuListCell[i].setParentResponder(&m_selectableTableView);
    m_menuListCell[i].setDelegates(inputEventHandlerDelegate, this);
  }
}

const char * ParametersController::title() {
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
  resetMemoization();
  m_contentView.layoutSubviews();
  FloatParameterController::viewWillAppear();
}

void ParametersController::viewDidDisappear() {
  /* Prevent initSize to be bypassed when reopening the ParametersController.
   * Indeed, number of cells might increase so their width need to be computed
   * */
  m_contentView.resetSize();
}

int ParametersController::numberOfRows() const {
  return 1 + m_distribution->numberOfParameters();
}

void ParametersController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows() - 1) {
    return;
  }
  ExpressionCellWithEditableTextWithMessage * myCell =
      static_cast<ExpressionCellWithEditableTextWithMessage *>(cell);
  myCell->setLayout(m_distribution->parameterSymbolAtIndex(index));
  myCell->setSubLabelMessage(m_distribution->parameterDefinitionAtIndex(index));
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool ParametersController::isCellEditing(Escher::HighlightCell * cell, int index) {
  return static_cast<ExpressionCellWithEditableTextWithMessage *>(cell)->textField()->isEditing();
}

void ParametersController::setTextInCell(Escher::HighlightCell * cell, const char * text, int index) {
  static_cast<ExpressionCellWithEditableTextWithMessage *>(cell)->textField()->setText(text);
}

HighlightCell * ParametersController::reusableParameterCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_maxNumberOfCells);
  return &m_menuListCell[index];
}

int ParametersController::reusableParameterCellCount(int type) {
  return m_distribution->numberOfParameters();
}

double ParametersController::parameterAtIndex(int index) {
  return m_distribution->parameterAtIndex(index);
}

bool ParametersController::setParameterAtIndex(int parameterIndex, double f) {
  if (!m_distribution->authorizedParameterAtIndex(f, parameterIndex)) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_distribution->setParameterAtIndex(f, parameterIndex);
  reinitCalculation();
  return true;
}

bool ParametersController::textFieldDidFinishEditing(AbstractTextField * textField,
                                                     const char * text,
                                                     Ion::Events::Event event) {
  if (FloatParameterController::textFieldDidFinishEditing(textField, text, event)) {
    resetMemoization();
    m_selectableTableView.reloadData();
    return true;
  }
  return false;
}

void ParametersController::buttonAction() {
  stackOpenPage(m_calculationController);
}

}
