#include "list_controller.h"
#include "app.h"
#include <assert.h>

using namespace Shared;

namespace Solver {

ListController::ListController(Responder * parentResponder, EquationStore * equationStore, ButtonRowController * footer) :
  ExpressionModelListController(parentResponder, I18n::Message::AddEquation),
  ButtonRowDelegate(nullptr, footer),
  m_equationStore(equationStore),
  m_resolveButton(this, I18n::Message::Resolve, Invocation([](void * context, void * sender) {
    ListController * list = (ListController *)context;
    StackViewController * stackController = list->stackController();
    // TODO
    //stackController->push(list->solutionPage ??)
  }, this), KDText::FontSize::Small, Palette::PurpleBright)
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
  EvenOddCell * myCell = (EvenOddCell *)cell;
  myCell->setEven(index%2 == 0);
  myCell->setHighlighted(index == selectedRow());
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
}

bool textRepresentsAnEquality(const char * text) {
  if (strchr(text, '=')) {
    return true;
  }
  return false;
}

bool ListController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (Shared::TextFieldDelegate::textFieldDidReceiveEvent(textField, event)) {
    return true;
  }
  if (textField->isEditing() && textField->textFieldShouldFinishEditing(event)) {
    if (!textRepresentsAnEquality(textField->text())) {
      app()->displayWarning(I18n::Message::RequireEquation);
      return true;
    }
  }
  return false;
}

bool ListController::expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  if (Shared::ExpressionLayoutFieldDelegate::expressionLayoutFieldDidReceiveEvent(expressionLayoutField, event)) {
    return true;
  }
  if (expressionLayoutField->isEditing() && expressionLayoutField->expressionLayoutFieldShouldFinishEditing(event)) {
    char buffer[TextField::maxBufferSize()];
    expressionLayoutField->writeTextInBuffer(buffer, TextField::maxBufferSize());
    if (!textRepresentsAnEquality(buffer)) {
      app()->displayWarning(I18n::Message::RequireEquation);
      return true;
    }
  }
  return false;
}

View * ListController::loadView() {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i] = new ModelExpressionCell();
  }
  return Shared::ExpressionModelListController::loadView();
}

void ListController::unloadView(View * view) {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    delete m_expressionCells[i];
    m_expressionCells[i] = nullptr;
  }
  Shared::ExpressionModelListController::unloadView(view);
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
