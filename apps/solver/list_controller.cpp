#include "list_controller.h"
#include "app.h"
#include <assert.h>

using namespace Shared;

namespace Solver {

ListController::ListController(Responder * parentResponder, EquationStore * equationStore, ButtonRowController * footer) :
  ExpressionModelListController(parentResponder, I18n::Message::AddEquation),
  ButtonRowDelegate(nullptr, footer),
  m_equationStore(equationStore),
  m_resolveButton(this, equationStore->numberOfDefinedModels() > 1 ? I18n::Message::ResolveSystem : I18n::Message::ResolveEquation, Invocation([](void * context, void * sender) {
    ListController * list = (ListController *)context;
    list->resolveEquations();
  }, this), KDText::FontSize::Large, Palette::PurpleBright),
  m_modelsParameterController(this, equationStore, this),
  m_modelsStackController(nullptr, &m_modelsParameterController, KDColorWhite, Palette::PurpleDark, Palette::PurpleDark)
{
}

int ListController::numberOfButtons(ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Bottom) {
    return 1;
  }
  return 0;
}

Button * ListController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  return const_cast<Button *>(&m_resolveButton);
}

int ListController::typeAtLocation(int i, int j) {
  if (j == m_equationStore->numberOfModels()) {
    return 1;
  }
  return 0;
}

HighlightCell * ListController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_maxNumberOfRows);
  switch (type) {
    case 0:
      return m_expressionCells[index];
    case 1:
      return m_addNewModel;
    default:
      assert(false);
      return nullptr;
  }
}

int ListController::reusableCellCount(int type) {
  if (type == 1) {
    return 1;
  }
  return k_maxNumberOfRows;
}

void ListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index != m_equationStore->numberOfModels()) {
    willDisplayExpressionCellAtIndex(cell, index);
  }
  cell->setHighlighted(index == selectedRow());
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up && selectedRow() == -1) {
    footer()->setSelectedButton(-1);
    selectableTableView()->selectCellAtLocation(0, numberOfRows()-1);
    app()->setFirstResponder(selectableTableView());
    return true;
  }
  if (event == Ion::Events::Down) {
    if (selectedRow() == -1) {
      return false;
    }
    selectableTableView()->deselectTable();
    footer()->setSelectedButton(0);
    return true;
  }
  return handleEventOnExpression(event);
}

void ListController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  if (selectedRow() >= numberOfRows()) {
    selectCellAtLocation(selectedColumn(), numberOfRows()-1);
  }
  footer()->setSelectedButton(-1);
  app()->setFirstResponder(selectableTableView());
}

void ListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectableTableView()->reloadData();
  // Reload brace if the model store has evolved
  reloadBrace();
}

bool textRepresentsAnEquality(const char * text) {
  if (strchr(text, '=')) {
    return true;
  }
  return false;
}

bool ListController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->textFieldShouldFinishEditing(event)) {
    if (!textRepresentsAnEquality(textField->text())) {
      textField->handleEvent(Ion::Events::ShiftRight);
      textField->handleEventWithText("=0");
    }
  }
  if (Shared::TextFieldDelegate::textFieldDidReceiveEvent(textField, event)) {
    return true;
  }
  return false;
}

bool ListController::expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  if (expressionLayoutField->isEditing() && expressionLayoutField->expressionLayoutFieldShouldFinishEditing(event)) {
    char buffer[TextField::maxBufferSize()];
    expressionLayoutField->writeTextInBuffer(buffer, TextField::maxBufferSize());
    if (!textRepresentsAnEquality(buffer)) {
      expressionLayoutField->handleEvent(Ion::Events::ShiftRight);
      expressionLayoutField->handleEventWithText("=0");
    }
  }
  if (Shared::ExpressionLayoutFieldDelegate::expressionLayoutFieldDidReceiveEvent(expressionLayoutField, event)) {
    return true;
  }
  return false;
}

bool ListController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  reloadButtonMessage();
  return true;
}

bool ListController::expressionLayoutFieldDidFinishEditing(ExpressionLayoutField * expressionLayoutField, Poincare::ExpressionLayout * layout, Ion::Events::Event event) {
  reloadButtonMessage();
  return true;
}

void ListController::resolveEquations() {
  if (m_equationStore->numberOfDefinedModels() == 0) {
    app()->displayWarning(I18n::Message::EnterEquation);
    return;
  }
  EquationStore::Error e = m_equationStore->exactSolve(static_cast<App *>(app())->localContext());
  switch (e) {
    case EquationStore::Error::EquationUndefined:
      app()->displayWarning(I18n::Message::UndefinedEquation);
      return;
    case EquationStore::Error::TooManyVariables:
      app()->displayWarning(I18n::Message::TooManyVariables);
      return;
    case EquationStore::Error::NonLinearSystem:
      app()->displayWarning(I18n::Message::NonLinearSystem);
      return;
    case EquationStore::Error::RequireApproximateSolution:
    {
      StackViewController * stack = stackController();
      App * solverApp = static_cast<App *>(app());
      stack->push(solverApp->intervalController(), KDColorWhite, Palette::PurpleBright, Palette::PurpleBright);
      return;
    }
    default:
    {
      assert(e == EquationStore::Error::NoError);
      StackViewController * stack = stackController();
      App * solverApp = static_cast<App *>(app());
      stack->push(solverApp->solutionsControllerStack(), KDColorWhite, Palette::PurpleBright, Palette::PurpleBright);
    }
 }
}

void ListController::reloadButtonMessage() {
  footer()->setMessageOfButtonAtIndex(m_equationStore->numberOfDefinedModels() > 1 ? I18n::Message::ResolveSystem : I18n::Message::ResolveEquation, 0);
}

void ListController::addEmptyModel() {
  app()->displayModalViewController(&m_modelsStackController, 0.f, 0.f, Metric::CommonTopMargin, Metric::CommonRightMargin, 0, Metric::CommonLeftMargin);
}

bool ListController::removeModelRow(ExpressionModel * model) {
  ExpressionModelListController::removeModelRow(model);
  reloadButtonMessage();
  reloadBrace();
  return true;
}

void ListController::reloadBrace() {
  EquationListView * listView = static_cast<EquationListView *>(view());
  listView->displayBrace(m_equationStore->numberOfModels() > 1);
  listView->layoutSubviews();
}

SelectableTableView * ListController::selectableTableView() {
  return static_cast<EquationListView *>(view())->selectableTableView();
}

View * ListController::loadView() {
  loadAddModelCell();
  m_addNewModel->setAlignment(0.3f, 0.5f); // (EquationListView::k_braceTotalWidth+k_expressionMargin) / (Ion::Display::Width-m_addNewModel.text().size()) = (30+5)/(320-200)
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i] = new EvenOddExpressionCell();
    m_expressionCells[i]->setMargin(EquationListView::k_braceTotalWidth+k_expressionMargin);
    m_expressionCells[i]->setEven(true);
  }
  EquationListView * listView = new EquationListView(this, this, this);
  return listView;
}

void ListController::unloadView(View * view) {
  unloadAddModelCell();
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    delete m_expressionCells[i];
    m_expressionCells[i] = nullptr;
  }
  delete view;
}

Shared::TextFieldDelegateApp * ListController::textFieldDelegateApp() {
  return static_cast<App *>(app());
}

Shared::ExpressionFieldDelegateApp * ListController::expressionFieldDelegateApp() {
  return static_cast<App *>(app());
}

StackViewController * ListController::stackController() const {
  return static_cast<StackViewController *>(parentResponder()->parentResponder());
}

InputViewController * ListController::inputController() {
  return static_cast<App *>(app())->inputViewController();
}

}
