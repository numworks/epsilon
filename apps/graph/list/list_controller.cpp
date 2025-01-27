#include "list_controller.h"

#include <apps/i18n.h>
#include <apps/shared/function_name_helper.h>
#include <assert.h>
#include <escher/metric.h>
#include <omg/utf8_helper.h>
#include <poincare/code_points.h>
#include <poincare/helpers/symbol.h>
#include <poincare/layout.h>

#include "../app.h"

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Graph {

ListController::ListController(
    Responder* parentResponder, ButtonRowController* header,
    ButtonRowController* footer,
    FunctionParameterController* functionParameterController,
    DerivativeColumnParameterController* derivativeColumnParameterController)
    : Shared::FunctionListController(parentResponder, header, footer,
                                     I18n::Message::AddFunction),
      m_editableCell(this, this, &m_modelsStackController),
      m_functionParameterController(functionParameterController),
      m_derivativeColumnParameterController(
          derivativeColumnParameterController),
      m_modelsParameterController(this, this),
      m_modelsStackController(nullptr, &m_modelsParameterController,
                              StackViewController::Style::PurpleWhite) {}

/* TableViewDataSource */

HighlightCell* ListController::reusableCell(int index, int type) {
  assert(index >= 0 && index < reusableCellCount(type));
  if (type == k_editableCellType) {
    return &m_editableCell;
  }
  if (type == k_addNewModelCellType) {
    return &m_addNewModelCell;
  }
  assert(type == k_expressionCellType);
  return functionCells(index);
}

/* ViewController */

void ListController::viewWillAppear() {
  Shared::FunctionListController::viewWillAppear();
  /* FunctionListcontroller::handleResponderChainEvent might not be called,
   * (if the list tab is displayed but not selected using Back-Back)
   * therefore we also need to manually reload the table here. */
  selectableListView()->reloadData(false);
  App::app()->defaultToolbox()->setExtraCellsDataSource(this);
}

void ListController::viewDidDisappear() {
  Shared::FunctionListController::viewDidDisappear();
  App::app()->defaultToolbox()->setExtraCellsDataSource(nullptr);
}

// Fills buffer with a default function equation, such as "f(x)=", "y=" or
// "r1(θ)="
void ListController::fillWithDefaultFunctionEquation(char* buffer,
                                                     size_t bufferSize,
                                                     CodePoint symbol) const {
  size_t length;
  if (symbol == CodePoints::k_cartesianSymbol &&
      FunctionModelsParameterController::EquationsPrefered()) {
    length = UTF8Helper::WriteCodePoint(buffer, bufferSize,
                                        CodePoints::k_ordinateSymbol);
  } else {
    length = FunctionNameHelper::DefaultName(buffer, bufferSize, symbol);
    assert(0 < length && length < bufferSize - 1);
    length += Shared::Function::WithArgument(symbol, buffer + length,
                                             bufferSize - length);
  }
  UTF8Helper::WriteCodePoint(buffer + length, bufferSize - length, '=');
}

bool ListController::shouldCompleteEquation(Poincare::UserExpression expression,
                                            CodePoint symbol) {
  Dimension dimension = expression.dimension();
  /* We do not want to complete equation if expression is already an
   * (in)equation, a point or a list of points. */
  return !expression.isComparison() &&
         (!dimension.isPoint() || symbol == CodePoints::k_parametricSymbol) &&
         !dimension.isListOfPoints();
}

bool ListController::completeEquation(LayoutField* equationField,
                                      CodePoint symbol) {
  equationField->putCursorOnOneSide(OMG::Direction::Left());
  // Retrieve the edited function
  ExpiringPointer<ContinuousFunction> f =
      modelStore()->modelForRecord(selectedRecord());
  constexpr size_t k_bufferSize =
      SymbolHelper::k_maxNameSize + sizeof("(θ)≥") - 1;
  char buffer[k_bufferSize];
  if (f->isNull() || f->properties().status() ==
                         ContinuousFunctionProperties::Status::Undefined) {
    // Function is new or undefined, complete the equation with a default name
    // Insert "f(x)=", with f the default function name and x the symbol
    fillWithDefaultFunctionEquation(buffer, k_bufferSize, symbol);
  } else {
    // Insert the name, symbol and equation symbol of the existing function
    size_t nameLength = f->nameWithArgument(buffer, k_bufferSize);
    nameLength += strlcpy(buffer + nameLength, f->properties().equationSymbol(),
                          k_bufferSize - nameLength);
    assert(nameLength < k_bufferSize);
  }
  bool handled = equationField->handleEventWithText(buffer, false, true);
  equationField->putCursorOnOneSide(OMG::Direction::Right());
  return handled;
}

void ListController::layoutFieldDidHandleEvent(LayoutField* layoutField) {
  bool isVisible = m_editableCell.isTemplateButtonVisible();
  bool shouldBeVisible = m_editableCell.templateButtonShouldBeVisible();

  if (shouldBeVisible == isVisible) {
    return;
  }
  if (!shouldBeVisible) {
    m_editableCell.setTemplateButtonHighlighted(false);
  }
  m_editableCell.setTemplateButtonVisible(shouldBeVisible);
}

bool ListController::layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                                Ion::Events::Event event) {
  assert(layoutField == this->layoutField());
  m_parameterColumnSelected = false;

  if (m_editableCell.isTemplateButtonVisible()) {
    bool onTemplateButton = m_editableCell.isTemplateButtonHighlighted();

    if (event == Ion::Events::Right && !onTemplateButton) {
      m_editableCell.setTemplateButtonHighlighted(true);
      return true;
    }
    if ((event == Ion::Events::Left || event == Ion::Events::Back) &&
        onTemplateButton) {
      m_editableCell.setTemplateButtonHighlighted(false);
      return true;
    }
  }

  if (event.isMoveEvent()) {
    return true;
  }

  return ExpressionModelListController::layoutFieldDidReceiveEvent(layoutField,
                                                                   event);
}

void ListController::layoutFieldDidAbortEditing(
    Escher::LayoutField* layoutField) {
  assert(!m_editableCell.isTemplateButtonHighlighted());
  ExpressionModelListController::layoutFieldDidAbortEditing(layoutField);
}

CodePoint ListController::defaultXNT() {
  assert(selectedRow() >= 0);
  return modelStore()->modelForRecord(selectedRecord())->symbol();
}

void ListController::editExpression(Ion::Events::Event event) {
  int relativeRow;
  modelIndexForRow(selectedRow(), &relativeRow);
  if (relativeRow != 0) {
    // Cell f' and f" are not editable
    return;
  }
  ExpressionModelListController::editExpression(event);
  if (m_editableCell.isTemplateButtonHighlighted()) {
    // TODO: should be done in layoutFieldDidStartEditing
    m_editableCell.setTemplateButtonHighlighted(false);
  }
  m_editableCell.setTemplateButtonVisible(
      m_editableCell.templateButtonShouldBeVisible());
  m_editableCell.setHighlighted(true);
}

bool ListController::editSelectedRecordWithLayout(Poincare::Layout layout) {
  GlobalContext::DeleteParametricComponentsOfRecord(selectedRecord());
  bool result = FunctionListController::editSelectedRecordWithLayout(layout);
  GlobalContext::StoreParametricComponentsOfRecord(selectedRecord());
  return result;
}

/* Responder */

KDCoordinate ListController::expressionRowHeight(int row) {
  assert(typeAtRow(row) == k_expressionCellType);
  FunctionCell tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

KDCoordinate ListController::editableRowHeight() {
  return m_editableCell.minimalSizeForOptimalDisplay().height();
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (selectedRow() >= 0 && selectedRow() <= numberOfRows() &&
      !isAddEmptyRow(selectedRow()) && m_parameterColumnSelected &&
      (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    // Will open function parameter menu
    // Open function parameter menu
    int relativeRow;
    Ion::Storage::Record record = selectedRecord(&relativeRow);
    ExpiringPointer<ContinuousFunction> f =
        modelStore()->modelForRecord(record);
    int derivationOrder =
        derivationOrderFromRelativeRow(f.pointer(), relativeRow);
    if (derivationOrder == 0) {
      m_functionParameterController->setRecord(record);
      m_functionParameterController->setParameterDelegate(this);
      stackController()->push(m_functionParameterController);
    } else {
      m_derivativeColumnParameterController->setRecord(record, derivationOrder);
      m_derivativeColumnParameterController->setParameterDelegate(this);
      stackController()->push(m_derivativeColumnParameterController);
    }
    return true;
  }
  return FunctionListController::handleEvent(event);
}

Shared::ListParameterController* ListController::parameterController() {
  return m_functionParameterController;
}

bool ListController::removeModelRow(Ion::Storage::Record record) {
  // Remove parametric components too
  int relativeRow;
  Ion::Storage::Record r = selectedRecord(&relativeRow);
  GlobalContext::DeleteParametricComponentsOfRecord(r);
  // If we are on a derivative row, only hide derivative
  ExpiringPointer<ContinuousFunction> f = modelStore()->modelForRecord(r);
  int derivationOrder =
      derivationOrderFromRelativeRow(f.pointer(), relativeRow);
  if (derivationOrder > 0) {
    if (derivationOrder == 1) {
      f->setDisplayPlotFirstDerivative(false);
    } else {
      assert(derivationOrder == 2);
      f->setDisplayPlotSecondDerivative(false);
    }
    didChangeModelsList();
    return true;
  }
  return FunctionListController::removeModelRow(record);
}

int ListController::maxNumberOfDisplayableRows() const {
  return k_maxNumberOfDisplayableRows;
}

Poincare::Layout ListController::extraCellLayoutAtRow(int row) {
  assert(row < k_numberOfToolboxExtraCells);
  constexpr CodePoint codepoints[k_numberOfToolboxExtraCells] = {
      UCodePointInferiorEqual, UCodePointSuperiorEqual};
  return Layout::CodePoint(codepoints[row]);
}

HighlightCell* ListController::functionCells(int row) {
  assert(row >= 0 && row < k_maxNumberOfDisplayableRows);
  return &m_expressionCells[row];
}

void ListController::fillCellForRow(HighlightCell* cell, int row) {
  int type = typeAtRow(row);
  if (type != k_addNewModelCellType) {
    assert(type == k_expressionCellType || type == k_editableCellType);
    int relativeRow;
    ExpiringPointer<ContinuousFunction> f =
        modelStore()->modelForRecord(recordAtRow(row, &relativeRow));
    int derivationOrder =
        derivationOrderFromRelativeRow(f.pointer(), relativeRow);
    if (type == k_expressionCellType) {
      FunctionCell* functionCell = static_cast<FunctionCell*>(cell);
      Layout layout;
      I18n::Message caption = I18n::Message::Default;
      bool hideMessage;
      if (derivationOrder == 0) {
        layout = f->layout();
        caption = f->properties().caption();
        hideMessage = false;
      } else {
        assert(derivationOrder == 1 || derivationOrder == 2);
        constexpr static size_t bufferSize =
            Shared::Function::k_maxNameWithArgumentSize;
        char buffer[bufferSize];
        size_t length =
            f->nameWithArgument(buffer, bufferSize, derivationOrder);
        layout = Layout::String(buffer, length);
        hideMessage = true;
      }
      functionCell->expressionCell()->setLayout(layout);
      functionCell->setMessage(caption);
      functionCell->setHideMessage(hideMessage);
      KDColor textColor = f->isActive() ? KDColorBlack : Palette::GrayDark;
      functionCell->expressionCell()->setTextColor(textColor);
      static_cast<FunctionCell*>(functionCell)
          ->setParameterSelected(m_parameterColumnSelected);
    }
    // f can be null if the entry was just created and is still empty.
    KDColor functionColor = (!f->isNull() && f->isActive())
                                ? f->color(derivationOrder)
                                : Palette::GrayDark;
    static_cast<AbstractFunctionCell*>(cell)->setColor(functionColor);
  }
  FunctionListController::fillCellForRow(cell, row);
}

void ListController::hideDerivative(Ion::Storage::Record record,
                                    int derivationOrder) {
  selectableListView()->selectCell(selectableListView()->selectedRow() - 1);
  ExpiringPointer<ContinuousFunction> f = modelStore()->modelForRecord(record);
  if (derivationOrder == 1) {
    f->setDisplayPlotFirstDerivative(false);
  } else {
    assert(derivationOrder == 2);
    f->setDisplayPlotSecondDerivative(false);
  }
  stackController()->pop();
}

void ListController::addNewModelAction() {
  Ion::Storage::Record::ErrorStatus error =
      App::app()->functionStore()->addEmptyModel();
  if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
    return;
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);

  editExpression(Ion::Events::OK);
}

ContinuousFunctionStore* ListController::modelStore() const {
  return App::app()->functionStore();
}

int ListController::numberOfRowsForRecord(Ion::Storage::Record record) const {
  ExpiringPointer<ContinuousFunction> f = modelStore()->modelForRecord(record);
  return 1 + f->displayPlotFirstDerivative() + f->displayPlotSecondDerivative();
}

int ListController::derivationOrderFromRelativeRow(ContinuousFunction* f,
                                                   int relativeRow) const {
  return f->derivationOrderFromRelativeIndex(
      relativeRow, ContinuousFunction::DerivativeDisplayType::Plot);
}

bool ListController::isValidExpressionModel(UserExpression expression) {
  ExpiringPointer<ContinuousFunction> f =
      modelStore()->modelForRecord(selectedRecord());
  if (FunctionNameHelper::ParametricComponentsNameError(expression,
                                                        f.pointer())) {
    App::app()->displayWarning(I18n::Message::ParametricNameError);
    return false;
  }
  return true;
}

}  // namespace Graph
