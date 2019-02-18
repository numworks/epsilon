#include "parameters_controller.h"
#include "../constant.h"
#include "app.h"
#include <assert.h>
#include <string.h>

namespace Probability {

ParametersController::ContentView::ContentView(Responder * parentResponder, SelectableTableView * selectableTableView) :
  m_numberOfParameters(1),
  m_titleView(KDFont::SmallFont, I18n::Message::ChooseParameters, 0.5f, 0.5f, Palette::GreyDark, Palette::WallScreen),
  m_firstParameterDefinition(KDFont::SmallFont, (I18n::Message)0, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen),
  m_secondParameterDefinition(KDFont::SmallFont, (I18n::Message)0, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen),
  m_selectableTableView(selectableTableView)
{
}

void ParametersController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  int tableHeight = m_selectableTableView->minimalSizeForOptimalDisplay().height();
  ctx->fillRect(KDRect(0, tableHeight, bounds().width(), bounds().height() - tableHeight), Palette::WallScreen);
}

MessageTextView * ParametersController::ContentView::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_firstParameterDefinition;
  }
  return &m_secondParameterDefinition;
}

void ParametersController::ContentView::setNumberOfParameters(int numberOfParameters) {
  m_numberOfParameters = numberOfParameters;
}

int ParametersController::ContentView::numberOfSubviews() const {
  return m_numberOfParameters+2;
}

View * ParametersController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 5);
  if (index == 0) {
    return &m_titleView;
  }
  if (index == 1) {
    return m_selectableTableView;
  }
  if (index == 2) {
    return &m_firstParameterDefinition;
  }
  return &m_secondParameterDefinition;
}

void ParametersController::ContentView::layoutSubviews() {
  KDCoordinate titleHeight = KDFont::SmallFont->glyphSize().height()+k_titleMargin;
  m_titleView.setFrame(KDRect(0, 0, bounds().width(), titleHeight));
  KDCoordinate tableHeight = m_selectableTableView->minimalSizeForOptimalDisplay().height();
  m_selectableTableView->setFrame(KDRect(0, titleHeight, bounds().width(),  tableHeight));
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  KDCoordinate defOrigin = (titleHeight+tableHeight)/2+(bounds().height()-textHeight)/2;
  m_secondParameterDefinition.setFrame(KDRectZero);
  if (m_numberOfParameters == 2) {
    defOrigin = (titleHeight+tableHeight)/2+(bounds().height()-2*textHeight-k_textMargin)/2;
    m_secondParameterDefinition.setFrame(KDRect(0, defOrigin+textHeight+k_textMargin, bounds().width(), textHeight));
  }
  m_firstParameterDefinition.setFrame(KDRect(0, defOrigin, bounds().width(), textHeight));
}

/* Parameters Controller */

ParametersController::ParametersController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Law * law, CalculationController * calculationController) :
  FloatParameterController(parentResponder),
  m_contentView(this, &m_selectableTableView),
  m_menuListCell{},
  m_law(law),
  m_calculationController(calculationController)
{
  assert(m_law != nullptr);
  m_okButton.setMessage(I18n::Message::Next);
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_menuListCell[i].setParentResponder(&m_selectableTableView);
    m_menuListCell[i].textField()->setDelegates(inputEventHandlerDelegate, this);
    m_menuListCell[i].textField()->setDraftTextBuffer(m_draftTextBuffer);
  }
}

const char * ParametersController::title() {
  return I18n::translate(m_law->title());
}

bool ParametersController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}
void ParametersController::reinitCalculation() {
  m_calculationController->setCalculationAccordingToIndex(0, true);
}

void ParametersController::didBecomeFirstResponder() {
  App::Snapshot * snapshot = (App::Snapshot *)app()->snapshot();
  snapshot->setActivePage(App::Snapshot::Page::Parameters);
  FloatParameterController::didBecomeFirstResponder();
}

void ParametersController::viewWillAppear() {
  m_contentView.setNumberOfParameters(m_law->numberOfParameter());
  for (int i = 0; i < m_law->numberOfParameter(); i++) {
    m_contentView.parameterDefinitionAtIndex(i)->setMessage(m_law->parameterDefinitionAtIndex(i));
  }
  m_contentView.layoutSubviews();
  FloatParameterController::viewWillAppear();
}

int ParametersController::numberOfRows() {
  return 1+m_law->numberOfParameter();
}

void ParametersController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    if (selectedRow() != numberOfRows()-1) {
      cell->setHighlighted(false);
    }
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *) cell;
  myCell->setMessage(m_law->parameterNameAtIndex(index));
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

HighlightCell * ParametersController::reusableParameterCell(int index, int type) {
  assert(index >= 0);
  assert(index < 2);
  return &m_menuListCell[index];
}

int ParametersController::reusableParameterCellCount(int type) {
  return m_law->numberOfParameter();
}

double ParametersController::parameterAtIndex(int index) {
  return m_law->parameterValueAtIndex(index);
}

bool ParametersController::setParameterAtIndex(int parameterIndex, double f) {
  if (!m_law->authorizedValueAtIndex(f, parameterIndex)) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_law->setParameterAtIndex(f, parameterIndex);
  m_calculationController->setCalculationAccordingToIndex(0, true);
  return true;
}

bool ParametersController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  if (FloatParameterController::textFieldDidFinishEditing(textField, text, event)) {
    m_selectableTableView.reloadData();
    return true;
  }
  return false;
}

void ParametersController::buttonAction() {
  StackViewController * stack = stackController();
  stack->push(m_calculationController, KDColorWhite, Palette::SubTab, Palette::SubTab);
}

}
