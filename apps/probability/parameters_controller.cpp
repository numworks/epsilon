#include "parameters_controller.h"
#include "../constant.h"
#include "app.h"
#include <assert.h>
#include <string.h>

namespace Probability {

ParametersController::ContentView::ContentView(Responder * parentResponder, SelectableTableView * selectableTableView) :
  m_nextButton(Button(parentResponder, "Suivant", Invocation([](void * context, void * sender) {
    ParametersController * parameterController = (ParametersController *) context;
    CalculationController * calculationController = parameterController->calculationController();
    calculationController->selectSubview(1);
    Calculation * calculation = calculationController->calculation();
    calculation->setType(Calculation::Type::LeftIntegral);
    StackViewController * stack = parameterController->stackController();
    stack->updateTitle();
    stack->push(calculationController);
  }, parentResponder))),
  m_firstParameterDefinition(PointerTextView(nullptr, 0.5f, 0.5f, KDColorBlack, Palette::BackgroundColor)),
  m_secondParameterDefinition(PointerTextView(nullptr, 0.5f, 0.5f, KDColorBlack, Palette::BackgroundColor)),
  m_selectableTableView(selectableTableView)
{
}

Button * ParametersController::ContentView::button() {
  return &m_nextButton;
}

PointerTextView * ParametersController::ContentView::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_firstParameterDefinition;
  }
  return &m_secondParameterDefinition;
}

void ParametersController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  int tableHeight = m_selectableTableView->size().height()+ Metric::TopMargin + Metric::BottomMargin;
  ctx->fillRect(KDRect(0, tableHeight, bounds().width(), bounds().height() - tableHeight), Palette::BackgroundColor);
}

int ParametersController::ContentView::numberOfSubviews() const {
  return 4;
}

View * ParametersController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 4);
  if (index == 0) {
    return m_selectableTableView;
  }
  if (index == 1) {
    return &m_nextButton;
  }
  if (index == 2) {
    return &m_firstParameterDefinition;
  }
  return &m_secondParameterDefinition;
}

void ParametersController::ContentView::layoutSubviews() {
  int tableHeight = m_selectableTableView->size().height() + Metric::TopMargin + Metric::BottomMargin;
  m_selectableTableView->setFrame(KDRect(0, 0, bounds().width(),  tableHeight));
  m_nextButton.setFrame(KDRect(Metric::LeftMargin, tableHeight, bounds().width() - Metric::RightMargin - Metric::LeftMargin, k_buttonHeight));
  m_firstParameterDefinition.setFrame(KDRect(0, tableHeight + k_buttonHeight, bounds().width(), k_textHeight));
  m_secondParameterDefinition.setFrame(KDRect(0, tableHeight + k_buttonHeight + k_textHeight, bounds().width(), k_textHeight));
}

/* Parameters Controller */

ParametersController::ParametersController(Responder * parentResponder) :
  FloatParameterController(parentResponder),
  m_contentView(ContentView(this, &m_selectableTableView)),
  m_law(nullptr),
  m_buttonSelected(false),
  m_calculationController(CalculationController(nullptr))
{
  for (int k = 0; k < k_maxNumberOfCells; k++) {
    m_menuListCell[k].setParentResponder(&m_selectableTableView);
    m_menuListCell[k].setDelegate(this);
  }
}

ExpressionTextFieldDelegate * ParametersController::textFieldDelegate() {
  ExpressionTextFieldDelegate * myApp = (ExpressionTextFieldDelegate *)app();
  return myApp;
}

View * ParametersController::view() {
  return &m_contentView;
}

const char * ParametersController::title() const {
  if (!m_buttonSelected) {
    return "Choisir les parametres";
  }
  return m_titleBuffer;
}

void ParametersController::setLaw(Law * law) {
  m_law = law;
  m_calculationController.setLaw(law);
}

void ParametersController::updateTitle() {
  int currentChar = 0;
  for (int index = 0; index < m_law->numberOfParameter(); index++) {
    m_titleBuffer[currentChar++] = m_law->parameterNameAtIndex(index)[0];
    strlcpy(m_titleBuffer+currentChar, " = ", 4);
    currentChar += 3;
    char buffer[Constant::FloatBufferSizeInScientificMode];
    Float(m_law->parameterValueAtIndex(index)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    strlcpy(m_titleBuffer+currentChar, buffer, strlen(buffer)+1);
    currentChar += strlen(buffer);
    m_titleBuffer[currentChar++] = ' ';
  }
  m_titleBuffer[currentChar-1] = 0;
}

bool ParametersController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down && !m_buttonSelected) {
    m_buttonSelected = true;
    m_contentView.button()->setBackgroundColor(Palette::FocusCellBackgroundColor);
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(m_contentView.button());
    updateTitle();
    return true;
  }
  if (event == Ion::Events::Up && m_buttonSelected) {
    m_buttonSelected = false;
    m_contentView.button()->setBackgroundColor(KDColorWhite);
    m_selectableTableView.selectCellAtLocation(0, numberOfRows()-1);
    app()->setFirstResponder(&m_selectableTableView);
    return true;
  }
  return FloatParameterController::handleEvent(event);
}

void ParametersController::didBecomeFirstResponder() {
  for (int i = 0; i < numberOfRows(); i++) {
    m_contentView.parameterDefinitionAtIndex(i)->setText(m_law->parameterDefinitionAtIndex(i));
  }
  m_contentView.layoutSubviews();
  m_buttonSelected = false;
  stackController()->updateTitle();
  m_contentView.button()->setBackgroundColor(KDColorWhite);
  FloatParameterController::didBecomeFirstResponder();
}

StackViewController * ParametersController::stackController() {
  return (StackViewController * )parentResponder();
}

CalculationController * ParametersController::calculationController() {
  return &m_calculationController;
}

int ParametersController::numberOfRows() {
  return m_law->numberOfParameter();
};

void ParametersController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  EditableTextMenuListCell * myCell = (EditableTextMenuListCell *) cell;
  myCell->setText(m_law->parameterNameAtIndex(index));
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

TableViewCell * ParametersController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < 2);
  return &m_menuListCell[index];
}

int ParametersController::reusableCellCount() {
  return m_law->numberOfParameter();
}

float ParametersController::parameterAtIndex(int index) {
  return m_law->parameterValueAtIndex(index);
}

void ParametersController::setParameterAtIndex(int parameterIndex, float f) {
  m_law->setParameterAtIndex(f, parameterIndex);
}

bool ParametersController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  App * myApp = (App *)app();
  return myApp->textFieldDidReceiveEvent(textField, event);
}

}

