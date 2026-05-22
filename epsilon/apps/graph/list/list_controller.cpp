#include "list_controller.h"

#include <apps/i18n.h>
#include <apps/shared/function_name_helper.h>
#include <assert.h>
#include <escher/metric.h>
#include <escher/theme/color_picker.h>
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
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_expressionCells[index];
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

bool ListController::shouldCompleteEquation(
    const Poincare::UserExpression& expression, CodePoint symbol) {
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
  OMG::ExpiringPointer<ContinuousFunction> f =
      isAddingNewModel() ? nullptr
                         : modelStore()->modelForRecord(selectedRecord());
  constexpr size_t k_bufferSize =
      SymbolHelper::k_maxNameSize + sizeof("(θ)≥") - 1;
  char buffer[k_bufferSize];
  if (!f.pointer() || f->properties().status() ==
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
  bool handled = equationField->handleEventWithText(buffer, false, false, true);
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
  if (isAddingNewModel()) {
    return AbstractMathFieldDelegate::defaultXNT();
  }
  return modelStore()->modelForRecord(selectedRecord())->symbol();
}

void ListController::editExpression(Ion::Events::Event event) {
  if (!isAddModelRow(selectedRow())) {
    OMG::ExpiringPointer<ContinuousFunction> f =
        modelStore()->modelForRecord(selectedRecord());
    if (f->isDerivative()) {
      // Derivative cells are not editable
      return;
    }
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

bool ListController::setSelectedRecordContent(const Poincare::Layout& layout) {
  assert(!isAddingNewModel());
  GlobalStore::DeleteParametricComponentsOfRecord(selectedRecord());
  bool result = FunctionListController::setSelectedRecordContent(layout);
  if (result) {
    GlobalStore::StoreParametricComponentsOfRecord(selectedRecord());
  }
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
      !isAddModelRow(selectedRow()) && m_parameterColumnSelected &&
      (event == Ion::Events::OK || event == Ion::Events::EXE)) {
    // Open function parameter menu
    Ion::Storage::Record record = selectedRecord();
    OMG::ExpiringPointer<ContinuousFunction> f =
        modelStore()->modelForRecord(record);
    if (f->isDerivative()) {
      m_derivativeColumnParameterController->setRecord(record);
      m_derivativeColumnParameterController->setParameterDelegate(this);
      stackController()->push(m_derivativeColumnParameterController);
    } else {
      m_functionParameterController->setRecord(record);
      m_functionParameterController->setParameterDelegate(this);
      stackController()->push(m_functionParameterController);
    }
    return true;
  }
  return FunctionListController::handleEvent(event);
}

Shared::ListParameterController* ListController::parameterController() {
  return m_functionParameterController;
}

void ListController::removeModelRow(Ion::Storage::Record record) {
  OMG::ExpiringPointer<ContinuousFunction> f =
      modelStore()->modelForRecord(record);
  if (f->isDerivative()) {
    assert(f->numberOfSubCurves() == 1);
    // Update the parent's derivative toggle before removing model
    int derivationOrder = f->derivationOrder();
    f->parentFunction()->disablePlotDerivative(derivationOrder);
    return FunctionListController::removeModelRow(record);
  }
  GlobalStore::DeleteParametricComponentsOfRecord(record);
  GlobalStore::DeleteDerivativeOfRecord(record);
  FunctionListController::removeModelRow(record);
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

void ListController::addNewModelAction() {
  // In Grapher, go straight to the expression edition without template menu.
  editExpression(Ion::Events::OK);
}

void ListController::fillCellForRow(HighlightCell* cell, int row) {
  int type = typeAtRow(row);
  if (type == k_editableCellType && isAddingNewModel()) {
    static_cast<AbstractFunctionCell*>(cell)->setColor(
        ThemeComponent::TextGrayDark);
  } else if (type != k_addNewModelCellType) {
    assert(type == k_expressionCellType || type == k_editableCellType);
    OMG::ExpiringPointer<ContinuousFunction> f =
        modelStore()->modelForRecord(recordAtRow(row));
    assert(!f->isNull());
    if (type == k_expressionCellType) {
      bool isDerivative = f->isDerivative();
      FunctionCell* functionCell = static_cast<FunctionCell*>(cell);
      functionCell->expressionCell()->setLayout(f->layout());
      functionCell->setMessage(isDerivative ? I18n::Message::Default
                                            : f->properties().caption());
      functionCell->setHideMessage(isDerivative);
      ThemeComponent textColor = f->isActive() ? ThemeComponent::TextDefault
                                               : ThemeComponent::TextGrayDark;
      functionCell->expressionCell()->setTextColor(textColor);
      functionCell->setParameterSelected(m_parameterColumnSelected);
    }
    // f can be null if the entry was just created and is still empty.
    ThemeComponent functionColor =
        f->isActive() ? f->color() : ThemeComponent::AxisGrayDark;
    static_cast<AbstractFunctionCell*>(cell)->setColor(functionColor);
  }
  FunctionListController::fillCellForRow(cell, row);
}

void ListController::hideDerivative(Ion::Storage::Record record,
                                    int derivationOrder) {
  selectableListView()->selectCell(selectableListView()->selectedRow() - 1);
  ParameterDelegate::hideDerivative(record, derivationOrder);
  stackController()->pop();
}

ContinuousFunctionStore* ListController::modelStore() const {
  return &App::app()->functionStore();
}

bool ListController::isValidExpressionModel(const UserExpression& expression) {
  OMG::ExpiringPointer<ContinuousFunction> f =
      isAddingNewModel() ? nullptr
                         : modelStore()->modelForRecord(selectedRecord());
  if (FunctionNameHelper::ParametricComponentsNameError(expression,
                                                        f.pointer())) {
    App::app()->displayWarning(I18n::Message::ParametricNameError);
    return false;
  }
  return true;
}

}  // namespace Graph
