#include "parameters_controller.h"
#include "../constant.h"
#include "app.h"
#include <assert.h>
#include <string.h>

namespace Probability {

ParametersController::ContentView::ContentView(Responder * parentResponder, SelectableTableView * selectableTableView) :
  m_numberOfParameters(1),
  m_nextButton(Button(parentResponder, "Suivant", Invocation([](void * context, void * sender) {
    ParametersController * parameterController = (ParametersController *) context;
    CalculationController * calculationController = parameterController->calculationController();
    calculationController->setCalculationAccordingToIndex(0);
    calculationController->selectSubview(1);
    calculationController->reload();
    StackViewController * stack = parameterController->stackController();
    stack->push(calculationController);
  }, parentResponder), KDText::FontSize::Large)),
  m_titleView(PointerTextView(KDText::FontSize::Small, "Choisir les parametres", 0.5f, 0.5f, Palette::GreyDark, Palette::WallScreen)),
  m_firstParameterDefinition(PointerTextView(KDText::FontSize::Small, nullptr, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen)),
  m_secondParameterDefinition(PointerTextView(KDText::FontSize::Small, nullptr, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen)),
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
  ctx->fillRect(KDRect(0, tableHeight, bounds().width(), bounds().height() - tableHeight), Palette::WallScreen);
}

void ParametersController::ContentView::setNumberOfParameters(int numberOfParameters) {
  m_numberOfParameters = numberOfParameters;
}

int ParametersController::ContentView::numberOfSubviews() const {
  return m_numberOfParameters+3;
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
    return &m_nextButton;
  }
  if (index == 3) {
    return &m_firstParameterDefinition;
  }
  return &m_secondParameterDefinition;
}

void ParametersController::ContentView::layoutSubviews() {
  KDCoordinate titleHeight = KDText::stringSize("", KDText::FontSize::Small).height()+k_titleMargin;
  m_titleView.setFrame(KDRect(0, 0, bounds().width(), titleHeight));
  KDCoordinate tableHeight = m_selectableTableView->size().height() + Metric::TopMargin + Metric::BottomMargin;
  m_selectableTableView->setFrame(KDRect(0, titleHeight, bounds().width(),  tableHeight));
  m_nextButton.setFrame(KDRect(Metric::LeftMargin, titleHeight+tableHeight, bounds().width() - Metric::RightMargin - Metric::LeftMargin, k_buttonHeight));
  KDCoordinate textHeight = KDText::stringSize("", KDText::FontSize::Small).height();
  KDCoordinate defOrigin = (titleHeight+tableHeight+k_buttonHeight)/2+(bounds().height()-textHeight)/2;
  m_secondParameterDefinition.setFrame(KDRectZero);
  if (m_numberOfParameters == 2) {
    defOrigin = (titleHeight+tableHeight+k_buttonHeight)/2+(bounds().height()-2*textHeight-k_textMargin)/2;
    m_secondParameterDefinition.setFrame(KDRect(0, defOrigin+textHeight+k_textMargin, bounds().width(), textHeight));
  }
  m_firstParameterDefinition.setFrame(KDRect(0, defOrigin, bounds().width(), textHeight));
}

/* Parameters Controller */

ParametersController::ParametersController(Responder * parentResponder) :
  FloatParameterController(parentResponder),
  m_menuListCell{EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer),
    EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer)},
  m_contentView(ContentView(this, &m_selectableTableView)),
  m_law(nullptr),
  m_buttonSelected(false),
  m_calculationController(CalculationController(nullptr))
{
}

View * ParametersController::view() {
  return &m_contentView;
}

const char * ParametersController::title() const {
  if (m_law != nullptr) {
    return m_law->title();
  }
  return "";
}

void ParametersController::setLaw(Law * law) {
  m_law = law;
  if (m_law != nullptr) {
    m_contentView.setNumberOfParameters(m_law->numberOfParameter());
  }
  m_calculationController.setLaw(law);
}

bool ParametersController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down && !m_buttonSelected) {
    m_buttonSelected = true;
    m_contentView.button()->setBackgroundColor(Palette::Select);
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(m_contentView.button());
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
  if (!m_law->authorizedValueAtIndex(f, parameterIndex)) {
    app()->displayWarning("Cette valeur est interdite !");
    return;
  }
  m_law->setParameterAtIndex(f, parameterIndex);
}

}

