#include "list_controller.h"

#include <assert.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/comparison.h>
#include <poincare/variable_context.h>

#include "app.h"

using namespace Shared;
using namespace Escher;

namespace Solver {

ListController::ListController(Responder *parentResponder,
                               EquationStore *equationStore,
                               ButtonRowController *footer)
    : ExpressionModelListController(parentResponder,
                                    I18n::Message::AddEquation),
      ButtonRowDelegate(nullptr, footer),
      m_equationListView(this),
      m_editableCell(this, this),
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
                      ButtonCell::Style::EmbossedGray, KDFont::Size::Large,
                      Palette::PurpleBright),
      m_modelsParameterController(this, equationStore, this),
      m_modelsStackController(nullptr, &m_modelsParameterController,
                              StackViewController::Style::PurpleWhite) {
  m_addNewModelCell.setLeftMargin(k_newModelMargin);
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i].setMargins(
        {EquationListView::k_braceTotalWidth + Metric::CellMargins.left(), 0});
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

ButtonCell *ListController::buttonAtIndex(
    int index, ButtonRowController::Position position) const {
  if (position == ButtonRowController::Position::Top) {
    return nullptr;
  }
  return const_cast<ButtonCell *>(&m_resolveButton);
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

void ListController::fillCellForRow(HighlightCell *cell, int row) {
  int type = typeAtRow(row);
  if (type == k_expressionCellType) {
    EvenOddExpressionCell *evenOddCell =
        static_cast<EvenOddExpressionCell *>(cell);
    ExpiringPointer<Equation> equation =
        modelStore()->modelForRecord(recordAtRow(row));
    evenOddCell->setLayout(equation->layout());
  }
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up && selectedRow() == -1) {
    footer()->setSelectedButton(-1);
    selectableListView()->selectCell(numberOfRows() - 1);
    App::app()->setFirstResponder(selectableListView());
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
    selectRow(0);
  } else {
    selectRow(selectedRow());
  }
  if (selectedRow() >= numberOfRows()) {
    selectRow(numberOfRows() - 1);
  }
  footer()->setSelectedButton(-1);
  App::app()->setFirstResponder(selectableListView());
}

void ListController::didEnterResponderChain(Responder *previousFirstResponder) {
  selectableListView()->reloadData(false);
  // Reload brace if the model store has evolved
  reloadBrace();
}

bool ListController::completeEquation(LayoutField *equationField,
                                      CodePoint symbol) {
  equationField->putCursorOnOneSide(OMG::Direction::Right());
  return equationField->handleEventWithText("=0");
}

void ListController::layoutFieldDidChangeSize(LayoutField *layoutField) {
  ExpressionModelListController::layoutFieldDidChangeSize(layoutField);
  reloadBrace();
}

bool ListController::layoutFieldDidFinishEditing(LayoutField *layoutField,
                                                 Ion::Events::Event event) {
  assert(!layoutField->isEditing());
  if (ExpressionModelListController::layoutFieldDidFinishEditing(layoutField,
                                                                 event)) {
    reloadBrace();
    reloadButtonMessage();
    return true;
  }
  return false;
}

void ListController::layoutFieldDidAbortEditing(
    Escher::LayoutField *layoutField) {
  ExpressionModelListController::layoutFieldDidAbortEditing(layoutField);
  reloadBrace();
  reloadButtonMessage();
}

bool ListController::isAcceptableExpression(const Poincare::Expression exp,
                                            Poincare::Context *context) {
  /* Complete SharedApp acceptable conditions by only accepting
   * the Equal OperatorType in the list of equations. */
  return MathLayoutFieldDelegate::isAcceptableExpression(exp, context) &&
         Poincare::ComparisonNode::IsBinaryEquality(exp);
}

void ListController::editExpression(Ion::Events::Event event) {
  ExpressionModelListController::editExpression(event);
  // We set the highlighted state for the background color
  m_editableCell.setHighlighted(true);
}

void ListController::resolveEquations() {
  if (modelStore()->numberOfDefinedModels() == 0) {
    App::app()->displayWarning(I18n::Message::EnterEquation);
    return;
  }
  // Tidy model before checkpoint, during which older TreeNodes can't be altered
  modelStore()->tidyDownstreamPoolFrom();
  App::app()->system()->tidy();
  Poincare::Context *context = App::app()->localContext();
  Poincare::CircuitBreakerCheckpoint checkpoint(
      Ion::CircuitBreaker::CheckpointType::Back);
  if (CircuitBreakerRun(checkpoint)) {
    using Error = SystemOfEquations::Error;
    Error e = App::app()->system()->exactSolve(context);
    switch (e) {
      case Error::EquationUndefined:
        App::app()->displayWarning(I18n::Message::UndefinedEquation);
        return;
      case Error::EquationNonreal:
        App::app()->displayWarning(I18n::Message::NonrealEquation);
        return;
      case Error::TooManyVariables:
        App::app()->displayWarning(I18n::Message::TooManyVariables);
        return;
      case Error::NonLinearSystem:
        App::app()->displayWarning(I18n::Message::NonLinearSystem);
        return;
      case Error::DisabledInExamMode:
        App::app()->displayWarning(I18n::Message::DisabledFeatureInExamMode);
        return;
      case Error::RequireApproximateSolution:
        App::app()->system()->autoComputeApproximateSolvingRange(context);
        App::app()->system()->approximateSolve(context);
        // break; intentionally omitted
      default: {
        assert(e == Error::NoError || e == Error::RequireApproximateSolution);
        App::app()->openSolutionsController(e ==
                                            Error::RequireApproximateSolution);
      }
    }
  } else {
    modelStore()->tidyDownstreamPoolFrom(
        checkpoint.endOfPoolBeforeCheckpoint());
    App::app()->system()->tidy();
  }
}

void ListController::reloadButtonMessage() {
  footer()->setMessageOfButtonAtIndex(modelStore()->numberOfDefinedModels() > 1
                                          ? I18n::Message::ResolveSystem
                                          : I18n::Message::ResolveEquation,
                                      0);
}

void ListController::addNewModelAction() {
  App::app()->displayModalViewController(&m_modelsStackController, 0.f, 0.f,
                                         Metric::PopUpMarginsNoBottom);
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
    m_expressionCells[0].setMargins({k_newModelMargin, 0});
    // Set editable cell margins to keep the text at the same place when editing
    m_editableCell.layoutField()->margins()->setLeft(k_newModelMargin -
                                                     k_expressionMargin);
    m_editableCell.setMargins({k_expressionMargin, k_expressionMargin});
  } else {
    m_equationListView.setBraceStyle(
        modelStore()->numberOfModels() == modelStore()->maxNumberOfModels()
            ? EquationListView::BraceStyle::Full
            : EquationListView::BraceStyle::OneRowShort);
    m_expressionCells[0].setMargins(
        {EquationListView::k_braceTotalWidth + Metric::CellMargins.left(), 0});
    m_editableCell.layoutField()->margins()->setLeft(
        Metric::CellMargins.left() - k_expressionMargin);
    m_editableCell.setMargins(
        {EquationListView::k_braceTotalWidth + k_expressionMargin,
         k_expressionMargin});
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
