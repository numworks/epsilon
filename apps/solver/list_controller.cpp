#include "list_controller.h"
#include "app.h"
#include <poincare/code_point_layout.h>
#include <assert.h>

using namespace Shared;

namespace Solver {

ListController::ListController(Responder * parentResponder, EquationStore * equationStore, ButtonRowController * footer) :
  ExpressionModelListController(parentResponder, I18n::Message::AddEquation),
  ButtonRowDelegate(nullptr, footer),
  m_equationStore(equationStore),
  m_equationListView(this),
  m_expressionCells{},
  m_resolveButton(this, equationStore->numberOfDefinedModels() > 1 ? I18n::Message::ResolveSystem : I18n::Message::ResolveEquation, Invocation([](void * context, void * sender) {
    ListController * list = (ListController *)context;
    list->resolveEquations();
    return true;
  }, this), KDFont::LargeFont, Palette::PurpleBright),
  m_modelsParameterController(this, equationStore, this),
  m_modelsStackController(nullptr, &m_modelsParameterController, KDColorWhite, Palette::PurpleDark, Palette::PurpleDark)
{
  m_addNewModel.setAlignment(0.3f, 0.5f); // (EquationListView::k_braceTotalWidth+k_expressionMargin) / (Ion::Display::Width-m_addNewModel.text().size()) = (30+5)/(320-200)
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i].setLeftMargin(EquationListView::k_braceTotalWidth+k_expressionMargin);
    m_expressionCells[i].setEven(true);
  }
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
      return &m_expressionCells[index];
    case 1:
      return &m_addNewModel;
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
    Container::activeApp()->setFirstResponder(selectableTableView());
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
  Container::activeApp()->setFirstResponder(selectableTableView());
}

void ListController::didEnterResponderChain(Responder * previousFirstResponder) {
  selectableTableView()->reloadData();
  // Reload brace if the model store has evolved
  reloadBrace();
}

bool textRepresentsAnEquality(const char * text) {
  char equal = '=';
  return UTF8Helper::CodePointIs(UTF8Helper::CodePointSearch(text, equal), equal);
}

bool layoutRepresentsAnEquality(Poincare::Layout l) {
  Poincare::Layout match = l.recursivelyMatches(
      [](Poincare::Layout layout) {
      return layout.type() == Poincare::LayoutNode::Type::CodePointLayout && static_cast<Poincare::CodePointLayout &>(layout).codePoint() == '='; });
  return !match.isUninitialized();
}

bool ListController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->shouldFinishEditing(event)) {
    if (!textRepresentsAnEquality(textField->text())) {
      textField->handleEvent(Ion::Events::ShiftRight);
      textField->handleEventWithText("=0");
      if (!textRepresentsAnEquality(textField->text())) {
        Container::activeApp()->displayWarning(I18n::Message::RequireEquation);
        return true;
      }
    }
  }
  if (Shared::TextFieldDelegate::textFieldDidReceiveEvent(textField, event)) {
    return true;
  }
  return false;
}

bool ListController::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutRepresentsAnEquality(layoutField->layout())) {
      layoutField->handleEvent(Ion::Events::ShiftRight);
      layoutField->handleEventWithText("=0");
      if (!layoutRepresentsAnEquality(layoutField->layout())) {
        Container::activeApp()->displayWarning(I18n::Message::RequireEquation);
        return true;
      }
    }
  }
  if (Shared::LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField, event)) {
    return true;
  }
  return false;
}

bool ListController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  reloadButtonMessage();
  return true;
}

bool ListController::layoutFieldDidFinishEditing(LayoutField * layoutField, Poincare::Layout layout, Ion::Events::Event event) {
  reloadButtonMessage();
  return true;
}

void ListController::resolveEquations() {
  if (m_equationStore->numberOfDefinedModels() == 0) {
    Container::activeApp()->displayWarning(I18n::Message::EnterEquation);
    return;
  }
  EquationStore::Error e = m_equationStore->exactSolve(textFieldDelegateApp()->localContext());
  switch (e) {
    case EquationStore::Error::EquationUndefined:
      Container::activeApp()->displayWarning(I18n::Message::UndefinedEquation);
      return;
    case EquationStore::Error::EquationUnreal:
      Container::activeApp()->displayWarning(I18n::Message::UnrealEquation);
      return;
    case EquationStore::Error::TooManyVariables:
      Container::activeApp()->displayWarning(I18n::Message::TooManyVariables);
      return;
    case EquationStore::Error::NonLinearSystem:
      Container::activeApp()->displayWarning(I18n::Message::NonLinearSystem);
      return;
    case EquationStore::Error::RequireApproximateSolution:
    {
      StackViewController * stack = stackController();
      stack->push(App::app()->intervalController(), KDColorWhite, Palette::PurpleBright, Palette::PurpleBright);
      return;
    }
    default:
    {
      assert(e == EquationStore::Error::NoError);
      StackViewController * stack = stackController();
      stack->push(App::app()->solutionsControllerStack(), KDColorWhite, Palette::PurpleBright, Palette::PurpleBright);
    }
 }
}

void ListController::reloadButtonMessage() {
  footer()->setMessageOfButtonAtIndex(m_equationStore->numberOfDefinedModels() > 1 ? I18n::Message::ResolveSystem : I18n::Message::ResolveEquation, 0);
}

void ListController::addEmptyModel() {
  Container::activeApp()->displayModalViewController(&m_modelsStackController, 0.f, 0.f, Metric::CommonTopMargin, Metric::CommonRightMargin, 0, Metric::CommonLeftMargin);
}

bool ListController::removeModelRow(Ion::Storage::Record record) {
  ExpressionModelListController::removeModelRow(record);
  reloadButtonMessage();
  reloadBrace();
  return true;
}

void ListController::reloadBrace() {
  EquationListView::BraceStyle braceStyle = m_equationStore->numberOfModels() <= 1 ? EquationListView::BraceStyle::None : (m_equationStore->numberOfModels() == m_equationStore->maxNumberOfModels() ? EquationListView::BraceStyle::Full : EquationListView::BraceStyle::OneRowShort);
  m_equationListView.setBraceStyle(braceStyle);
}

SelectableTableView * ListController::selectableTableView() {
  return m_equationListView.selectableTableView();
}

StackViewController * ListController::stackController() const {
  return static_cast<StackViewController *>(parentResponder()->parentResponder());
}

InputViewController * ListController::inputController() {
  return App::app()->inputViewController();
}

}
