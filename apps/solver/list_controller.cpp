#include "list_controller.h"
#include "app.h"
#include <poincare/code_point_layout.h>
#include <poincare/variable_context.h>
#include <assert.h>

using namespace Shared;

namespace Solver {

ListController::ListController(Responder * parentResponder, EquationStore * equationStore, ButtonRowController * footer) :
  ExpressionModelListController(parentResponder, I18n::Message::AddEquation),
  ButtonRowDelegate(nullptr, footer),
  m_equationListView(this),
  m_expressionCells{},
  m_resolveButton(this, equationStore->numberOfDefinedModels() > 1 ? I18n::Message::ResolveSystem : I18n::Message::ResolveEquation, Invocation([](void * context, void * sender) {
    ListController * list = (ListController *)context;
    list->resolveEquations();
    return true;
  }, this), KDFont::LargeFont, Palette::ButtonText),
  m_modelsParameterController(this, equationStore, this),
  m_modelsStackController(nullptr, &m_modelsParameterController, Palette::BannerFirstVariantText, Palette::BannerFirstVariantBackground, Palette::BannerFirstVariantBorder)
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
  return isAddEmptyRow(j);
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
  if (!isAddEmptyRow(index)) {
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
    const char * text = textField->text();
    if (!textRepresentsAnEquality(text)) {
      textField->setCursorLocation(text + strlen(text));
      textField->handleEventWithText("=0");
      if (!textRepresentsAnEquality(text)) {
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
      layoutField->putCursorRightOfLayout();
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
  if (modelStore()->numberOfDefinedModels() == 0) {
    Container::activeApp()->displayWarning(I18n::Message::EnterEquation);
    return;
  }
  bool resultWithoutUserDefinedSymbols = false;
  EquationStore::Error e = modelStore()->exactSolve(textFieldDelegateApp()->localContext(), &resultWithoutUserDefinedSymbols);
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
      reinterpret_cast<IntervalController *>(App::app()->intervalController())->setShouldReplaceFuncionsButNotSymbols(resultWithoutUserDefinedSymbols);
      stackController()->push(App::app()->intervalController(), Palette::BannerFirstText, Palette::BannerFirstBackground, Palette::BannerFirstBorder);
      return;
    }
    default:
    {
      assert(e == EquationStore::Error::NoError);
      StackViewController * stack = stackController();
      reinterpret_cast<IntervalController *>(App::app()->intervalController())->setShouldReplaceFuncionsButNotSymbols(resultWithoutUserDefinedSymbols);
      stack->push(App::app()->solutionsControllerStack(), Palette::BannerFirstText, Palette::BannerFirstBackground, Palette::BannerFirstBorder);
    }
  }
}

void ListController::reloadButtonMessage() {
  footer()->setMessageOfButtonAtIndex(modelStore()->numberOfDefinedModels() > 1 ? I18n::Message::ResolveSystem : I18n::Message::ResolveEquation, 0);
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
  EquationListView::BraceStyle braceStyle = modelStore()->numberOfModels() <= 1 ? EquationListView::BraceStyle::None : (modelStore()->numberOfModels() == modelStore()->maxNumberOfModels() ? EquationListView::BraceStyle::Full : EquationListView::BraceStyle::OneRowShort);
  m_equationListView.setBraceStyle(braceStyle);
}

EquationStore * ListController::modelStore() {
  return App::app()->equationStore();
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
