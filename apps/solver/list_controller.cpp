#include "list_controller.h"

#include <assert.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/code_point_layout.h>
#include <poincare/variable_context.h>

#include "app.h"

using namespace Shared;
using namespace Escher;

namespace Solver {

ListController::ListController(
    Responder *parentResponder,
    InputEventHandlerDelegate *inputEventHandlerDelegate,
    EquationStore *equationStore, ButtonRowController *footer)
    : ExpressionModelListController(parentResponder,
                                    I18n::Message::AddEquation),
      ButtonRowDelegate(nullptr, footer),
      m_equationListView(this),
      m_editableCell(this, inputEventHandlerDelegate, this),
      m_resolveButton(this,
                      equationStore->numberOfDefinedModels() > 1
                          ? I18n::Message::ResolveSystem
                          : I18n::Message::ResolveEquation,
                      Invocation::Builder<ListController>(
                          [](ListController *list, void *sender) {
                            list->resolveEquations();
                            return true;
                          },
                          this),
                      KDFont::Size::Large, Palette::PurpleBright),
      m_modelsParameterController(this, equationStore, this),
      m_modelsStackController(nullptr, &m_modelsParameterController,
                              StackViewController::Style::PurpleWhite) {
  m_addNewModelCell.setLeftMargin(k_newModelMargin);
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i].setLeftMargin(EquationListView::k_braceTotalWidth +
                                       Metric::BigCellMargin);
    m_expressionCells[i].setEven(true);
  }
}

int ListController::numberOfButtons(
    ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Bottom) {
    return 1;
  }
  return 0;
}

AbstractButtonCell *ListController::buttonAtIndex(
    int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  return const_cast<AbstractButtonCell *>(&m_resolveButton);
}

HighlightCell *ListController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_maxNumberOfRows);
  switch (type) {
    case k_expressionCellType:
      return &m_expressionCells[index];
    case k_addNewModelCellType:
      return &m_addNewModelCell;
    case k_editableCellType:
      return &m_editableCell;
    default:
      assert(false);
      return nullptr;
  }
}

int ListController::reusableCellCount(int type) {
  if (type == k_expressionCellType) {
    return k_maxNumberOfRows;
  }
  return 1;
}

void ListController::willDisplayCellForIndex(HighlightCell *cell, int index) {
  if (!isAddEmptyRow(index) && index != m_editedCellIndex) {
    willDisplayExpressionCellAtIndex(cell, index);
  }
}

KDCoordinate ListController::nonMemoizedRowHeight(int index) {
  if (index == m_editedCellIndex) {
    return ExpressionRowHeightFromLayoutHeight(
        m_editableCell.minimalSizeForOptimalDisplay().height());
  }
  return expressionRowHeight(index);
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up && selectedRow() == -1) {
    footer()->setSelectedButton(-1);
    selectableListView()->selectCell(numberOfRows() - 1);
    Container::activeApp()->setFirstResponder(selectableListView());
    return true;
  }
  if (event == Ion::Events::Down) {
    if (selectedRow() == -1) {
      return false;
    }
    selectableListView()->deselectTable();
    footer()->setSelectedButton(0);
    return true;
  }
  return handleEventOnExpression(event);
}

void ListController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCell(0);
  } else {
    selectCell(selectedRow());
  }
  if (selectedRow() >= numberOfRows()) {
    selectCell(numberOfRows() - 1);
  }
  footer()->setSelectedButton(-1);
  Container::activeApp()->setFirstResponder(selectableListView());
}

void ListController::didEnterResponderChain(Responder *previousFirstResponder) {
  selectableListView()->reloadData(false);
  // Reload brace if the model store has evolved
  reloadBrace();
}

// TODO factorize with Graph?
bool ListController::layoutFieldDidReceiveEvent(LayoutField *layoutField,
                                                Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutField->layout()
             .hasTopLevelComparisonSymbol()) {  // TODO: do like for textField:
                                                // parse and and check is type
                                                // is comparison
      layoutField->putCursorOnOneSide(OMG::Direction::Right());
      if (!layoutField->handleEventWithText("=0")) {
        Container::activeApp()->displayWarning(I18n::Message::RequireEquation);
        return true;
      }
    }
  }
  if (Shared::LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField,
                                                              event)) {
    return true;
  }
  return false;
}

void ListController::layoutFieldDidChangeSize(LayoutField *layoutField) {
  ExpressionModelListController::layoutFieldDidChangeSize(layoutField);
  reloadBrace();
}

bool ListController::layoutFieldDidFinishEditing(LayoutField *layoutField,
                                                 Poincare::Layout layout,
                                                 Ion::Events::Event event) {
  ExpressionModelListController::layoutFieldDidFinishEditing(layoutField,
                                                             layout, event);
  reloadBrace();
  reloadButtonMessage();
  return true;
}

bool ListController::layoutFieldDidAbortEditing(
    Escher::LayoutField *layoutField) {
  ExpressionModelListController::layoutFieldDidAbortEditing(layoutField);
  reloadBrace();
  reloadButtonMessage();
  return true;
}

void ListController::editExpression(Ion::Events::Event event) {
  ExpressionModelListController::editExpression(event);
  // We set the highlighted state for the background color
  m_editableCell.setHighlighted(true);
}

void ListController::resolveEquations() {
  if (modelStore()->numberOfDefinedModels() == 0) {
    Container::activeApp()->displayWarning(I18n::Message::EnterEquation);
    return;
  }
  // Tidy model before checkpoint, during which older TreeNodes can't be altered
  modelStore()->tidyDownstreamPoolFrom();
  App::app()->system()->tidy();
  Poincare::CircuitBreakerCheckpoint checkpoint(
      Ion::CircuitBreaker::CheckpointType::Back);
  if (CircuitBreakerRun(checkpoint)) {
    bool resultWithoutUserDefinedSymbols = false;
    SystemOfEquations::Error e =
        App::app()->system()->exactSolve(App::app()->localContext());
    switch (e) {
      case SystemOfEquations::Error::EquationUndefined:
        Container::activeApp()->displayWarning(
            I18n::Message::UndefinedEquation);
        return;
      case SystemOfEquations::Error::EquationNonreal:
        Container::activeApp()->displayWarning(I18n::Message::NonrealEquation);
        return;
      case SystemOfEquations::Error::TooManyVariables:
        Container::activeApp()->displayWarning(I18n::Message::TooManyVariables);
        return;
      case SystemOfEquations::Error::NonLinearSystem:
        Container::activeApp()->displayWarning(I18n::Message::NonLinearSystem);
        return;
      case SystemOfEquations::Error::RequireApproximateSolution: {
        reinterpret_cast<IntervalController *>(App::app()->intervalController())
            ->setShouldReplaceFuncionsButNotSymbols(
                resultWithoutUserDefinedSymbols);
        stackController()->push(App::app()->intervalController());
        return;
      }
      default: {
        assert(e == SystemOfEquations::Error::NoError);
        StackViewController *stack = stackController();
        reinterpret_cast<IntervalController *>(App::app()->intervalController())
            ->setShouldReplaceFuncionsButNotSymbols(
                resultWithoutUserDefinedSymbols);
        stack->push(App::app()->solutionsControllerStack());
      }
    }
  } else {
    modelStore()->tidyDownstreamPoolFrom();
    App::app()->system()->tidy();
  }
}

void ListController::reloadButtonMessage() {
  footer()->setMessageOfButtonAtIndex(modelStore()->numberOfDefinedModels() > 1
                                          ? I18n::Message::ResolveSystem
                                          : I18n::Message::ResolveEquation,
                                      0);
}

void ListController::addModel() {
  Container::activeApp()->displayModalViewController(
      &m_modelsStackController, 0.f, 0.f, Metric::PopUpTopMargin,
      Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

bool ListController::removeModelRow(Ion::Storage::Record record) {
  ExpressionModelListController::removeModelRow(record);
  reloadButtonMessage();
  reloadBrace();
  return true;
}

void ListController::reloadBrace() {
  if (modelStore()->numberOfModels() <= 1) {
    m_equationListView.setBraceStyle(EquationListView::BraceStyle::None);
    m_expressionCells[0].setLeftMargin(k_newModelMargin);
    // Set editable cell margins to keep the text at the same place when editing
    m_editableCell.expressionField()->setLeftMargin(k_newModelMargin -
                                                    k_expressionMargin);
    m_editableCell.setMargins(k_expressionMargin, k_expressionMargin);
  } else {
    m_equationListView.setBraceStyle(
        modelStore()->numberOfModels() == modelStore()->maxNumberOfModels()
            ? EquationListView::BraceStyle::Full
            : EquationListView::BraceStyle::OneRowShort);
    m_expressionCells[0].setLeftMargin(EquationListView::k_braceTotalWidth +
                                       Metric::BigCellMargin);
    m_editableCell.expressionField()->setLeftMargin(Metric::BigCellMargin -
                                                    k_expressionMargin);
    m_editableCell.setMargins(
        EquationListView::k_braceTotalWidth + k_expressionMargin,
        k_expressionMargin);
  }
}

EquationStore *ListController::modelStore() const {
  return App::app()->equationStore();
}

SelectableListView *ListController::selectableListView() {
  return m_equationListView.selectableListView();
}

StackViewController *ListController::stackController() const {
  return static_cast<StackViewController *>(
      parentResponder()->parentResponder());
}

}  // namespace Solver
