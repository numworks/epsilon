#include "parameters_controller.h"
#include "../constant.h"
#include <assert.h>

namespace Probability {

ParametersController::ContentView::ContentView(Responder * parentResponder, SelectableTableView * selectableTableView) :
  m_nextButton(Button(parentResponder, "Suivant", Invocation([](void * context, void * sender) {}, this))),
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

ParametersController::ParametersController(Responder * parentResponder, Law * law) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_contentView(ContentView(this, &m_selectableTableView)),
  m_law(law),
  m_buttonSelected(false)
{
}

View * ParametersController::view() {
  return &m_contentView;
}

const char * ParametersController::title() const {
  return "Choisir les parametres";
}

bool ParametersController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down && !m_buttonSelected) {
    m_buttonSelected = true;
    m_contentView.button()->setBackgroundColor(Palette::FocusCellBackgroundColor);
    m_selectableTableView.deselectTable();
    app()->setFirstResponder(m_contentView.button());
  }
  if (event == Ion::Events::Up && m_buttonSelected) {
    m_buttonSelected = false;
    m_contentView.button()->setBackgroundColor(KDColorWhite);
    m_selectableTableView.selectCellAtLocation(0, numberOfRows()-1);
    app()->setFirstResponder(&m_selectableTableView);
  }
  return false;
}

void ParametersController::didBecomeFirstResponder() {
  for (int i = 0; i < numberOfRows(); i++) {
    m_contentView.parameterDefinitionAtIndex(i)->setText(m_law->parameterDefinitionAtIndex(i));
  }
  m_contentView.layoutSubviews();
  m_buttonSelected = false;
  m_contentView.button()->setBackgroundColor(KDColorWhite);
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

int ParametersController::numberOfRows() {
  return m_law->numberOfParameter();
};

void ParametersController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  TextMenuListCell * myCell = (TextMenuListCell *) cell;
  myCell->setText(m_law->parameterNameAtIndex(index));
  char buffer[Constant::FloatBufferSizeInScientificMode];
  Float(m_law->parameterValueAtIndex(index)).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  myCell->setAccessoryText(buffer);
}

TableViewCell * ParametersController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < 2);
  return &m_menuListCell[index];
}

int ParametersController::reusableCellCount() {
  return m_law->numberOfParameter();
}

KDCoordinate ParametersController::cellHeight() {
  return 35;
}

}

