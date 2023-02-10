#include "list_controller.h"

#include <apps/exam_mode_configuration.h>
#include <apps/i18n.h>
#include <assert.h>
#include <escher/metric.h>
#include <poincare/code_point_layout.h>
#include <poincare/matrix_layout.h>
#include <poincare/symbol_abstract.h>

#include "../app.h"

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Graph {

ListController::ListController(
    Responder *parentResponder, ButtonRowController *header,
    ButtonRowController *footer, GraphController *graphController,
    FunctionParameterController *functionParameterController)
    : Shared::FunctionListController(parentResponder, header, footer,
                                     I18n::Message::AddFunction),
      m_selectableTableView(this, this, this, this),
      m_editableCell(this, this, this),
      m_parameterController(functionParameterController),
      m_modelsParameterController(this, nullptr, this),
      m_modelsStackController(nullptr, &m_modelsParameterController,
                              StackViewController::Style::PurpleWhite),
      m_parameterColumnSelected(false) {
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setVerticalCellOverlap(0);
}

/* TableViewDataSource */

int ListController::typeAtIndex(int index) const {
  if (index == m_editedCellIndex) {
    return k_editableCellType;
  }
  if (isAddEmptyRow(index)) {
    return k_addNewModelType;
  }
  assert(index >= 0 && index < modelStore()->numberOfModels());
  return k_functionCellType;
}

HighlightCell *ListController::reusableCell(int index, int type) {
  assert(index >= 0 && index < reusableCellCount(type));
  if (type == k_editableCellType) {
    return &m_editableCell;
  }
  if (type == k_addNewModelType) {
    return &m_addNewModel;
  }
  assert(type == k_functionCellType);
  return functionCells(index);
}

int ListController::reusableCellCount(int type) {
  if (type == k_functionCellType) {
    return maxNumberOfDisplayableRows();
  }
  return 1;
}

/* ViewController */

const char *ListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

void ListController::viewWillAppear() {
  // A function could have been deleted from the option menu of the Graph tab.
  resetMemoization();
  Shared::FunctionListController::viewWillAppear();
  /* FunctionListcontroller::didEnterResponderChain might not be called,
   * (if the list tab is displayed but not selected using Back-Back)
   * therefore we also need to manually reload the table here. */
  selectableTableView()->reloadData(false);
}

// Fills buffer with a default function equation, such as "f(x)=", "y=" or "r="
void ListController::fillWithDefaultFunctionEquation(
    char *buffer, size_t bufferSize,
    FunctionModelsParameterController *modelsParameterController,
    CodePoint symbol) const {
  size_t length;
  if (symbol == ContinuousFunction::k_polarSymbol) {
    length = UTF8Decoder::CodePointToChars(ContinuousFunction::k_radiusSymbol,
                                           buffer, bufferSize);
  } else if (FunctionModelsParameterController::EquationsPrefered()) {
    length = UTF8Decoder::CodePointToChars(ContinuousFunction::k_ordinateSymbol,
                                           buffer, bufferSize);
  } else {
    length = modelsParameterController->DefaultName(buffer, bufferSize);
    assert(bufferSize > length);
    buffer[length++] = '(';
    length += UTF8Decoder::CodePointToChars(symbol, buffer + length,
                                            bufferSize - length);
    assert(bufferSize > length + 2);
    buffer[length++] = ')';
  }
  buffer[length++] = '=';
  buffer[length++] = 0;
}

// Return true if given layout contains θ
bool ListController::layoutRepresentsPolarFunction(Layout l) const {
  Layout match = l.recursivelyMatches([](Layout layout) {
    return layout.type() == LayoutNode::Type::CodePointLayout &&
                   static_cast<CodePointLayout &>(layout).codePoint() ==
                       ContinuousFunction::k_polarSymbol
               ? TrinaryBoolean::True
               : TrinaryBoolean::Unknown;
  });
  return !match.isUninitialized();
}

/* Return true if given layout contains a Matrix of correct dimensions as first
 * child. Note: A more precise detection would require an expression reduction
 * to distinguish:
 * - sum(1,t,0,100) : Cartesian
 * - norm([[4][5]]) : Cartesian
 * - 31*[[4][5]]*10 : Parametric */
bool ListController::layoutRepresentsParametricFunction(Layout l) const {
  if (l.type() == LayoutNode::Type::HorizontalLayout &&
      l.numberOfChildren() > 0) {
    l = l.childAtIndex(0);
  }
  if (l.type() != LayoutNode::Type::MatrixLayout) {
    return false;
  }
  MatrixLayout m = static_cast<MatrixLayout &>(l);
  return m.numberOfColumns() == 1 && m.numberOfRows() == 2;
}

bool ListController::completeEquation(InputEventHandler *equationField,
                                      CodePoint symbol) {
  // Retrieve the edited function
  ExpiringPointer<ContinuousFunction> f =
      modelStore()->modelForRecord(modelStore()->recordAtIndex(selectedRow()));
  if (f->isNull() || f->properties().status() ==
                         ContinuousFunctionProperties::Status::Undefined) {
    // Function is new or undefined, complete the equation with a default name
    constexpr size_t k_bufferSize =
        Shared::ContinuousFunction::k_maxDefaultNameSize + sizeof("(x)=") - 1;
    char buffer[k_bufferSize];
    // Insert "f(x)=", with f the default function name and x the symbol
    fillWithDefaultFunctionEquation(buffer, k_bufferSize,
                                    &m_modelsParameterController, symbol);
    return equationField->handleEventWithText(buffer);
  }
  // Insert the name, symbol and equation symbol of the existing function
  constexpr size_t k_bufferSize =
      SymbolAbstract::k_maxNameSize + sizeof("(x)≥") - 1;
  static_assert(k_bufferSize >= sizeof("r="),
                "k_bufferSize should fit both situations.");
  char buffer[k_bufferSize];
  size_t nameLength = f->nameWithArgument(buffer, k_bufferSize);
  nameLength += strlcpy(buffer + nameLength, f->properties().equationSymbol(),
                        k_bufferSize - nameLength);
  assert(nameLength < k_bufferSize);
  return equationField->handleEventWithText(buffer);
}

// TODO: factorize with solver
bool ListController::layoutFieldDidReceiveEvent(LayoutField *layoutField,
                                                Ion::Events::Event event) {
  m_parameterColumnSelected = false;
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutField->layout().hasTopLevelEquationSymbol()) {
      layoutField->putCursorOnOneSide(OMG::Direction::Left());
      CodePoint symbol =
          layoutRepresentsPolarFunction(layoutField->layout())
              ? ContinuousFunction::k_polarSymbol
              : (layoutRepresentsParametricFunction(layoutField->layout())
                     ? ContinuousFunction::k_parametricSymbol
                     : ContinuousFunction::k_cartesianSymbol);
      // Inserted Layout must be an equation
      if (!completeEquation(layoutField, symbol)) {
        layoutField->putCursorOnOneSide(OMG::Direction::Right());
        Container::activeApp()->displayWarning(I18n::Message::RequireEquation);
        return true;
      }
    }
  }
  return Shared::LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField,
                                                                 event);
}

void ListController::editExpression(Ion::Events::Event event) {
  ExpressionModelListController::editExpression(event);
  m_editableCell.setHighlighted(true);
}

/* Responder */

KDCoordinate ListController::expressionRowHeight(int j) {
  if (j == m_editedCellIndex) {
    return m_editableCell.minimalSizeForOptimalDisplay().height();
  }
  if (typeAtIndex(j) == k_addNewModelType) {
    return Shared::FunctionListController::expressionRowHeight(j);
  }
  FunctionCell tempCell;
  willDisplayCellForIndex(&tempCell, j);
  return tempCell.minimalSizeForOptimalDisplay().height();
}

bool ListController::handleEvent(Ion::Events::Event event) {
  // Here we handle an additional parameter column, within FunctionCell's button
  if (selectedRow() >= 0 && selectedRow() <= numberOfRows() &&
      !isAddEmptyRow(selectedRow())) {
    // Selected row is a function cell
    if (m_parameterColumnSelected) {
      // Parameter column is selected
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        // Open function parameter menu
        m_parameterController->setUseColumnTitle(false);
        configureFunction(
            modelStore()->recordAtIndex(modelIndexForRow(selectedRow())));
        return true;
      }
      if (event == Ion::Events::Left) {
        // Leave parameter column
        m_parameterColumnSelected = false;
        selectableTableView()->reloadData();
        return true;
      }
    } else if (event == Ion::Events::Right) {
      // Enter parameter column
      m_parameterColumnSelected = true;
      selectableTableView()->reloadData();
      return true;
    }
  }
  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      footer()->setSelectedButton(-1);
      selectableTableView()->selectCellAtLocation(0, numberOfRows() - 1);
      Container::activeApp()->setFirstResponder(selectableTableView());
      return true;
    }
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    tabController()->selectTab();
    return true;
  }
  if (selectedRow() < 0) {
    return false;
  }
  if (event == Ion::Events::Down) {
    selectableTableView()->deselectTable();
    footer()->setSelectedButton(0);
    return true;
  }
  return handleEventOnExpression(event);
}

Shared::ListParameterController *ListController::parameterController() {
  return m_parameterController;
}

int ListController::maxNumberOfDisplayableRows() {
  return k_maxNumberOfDisplayableRows;
}

HighlightCell *ListController::functionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_expressionCells[index];
}

void ListController::willDisplayCellForIndex(HighlightCell *cell, int j) {
  assert(cell != nullptr);
  EvenOddCell *evenOddCell = static_cast<EvenOddCell *>(cell);
  evenOddCell->setEven(j % 2 == 0);
  int type = typeAtIndex(j);
  if (type == k_addNewModelType) {
    evenOddCell->reloadCell();
    return;
  }
  assert(type == k_functionCellType || type == k_editableCellType);
  AbstractFunctionCell *functionCell =
      static_cast<AbstractFunctionCell *>(cell);
  ExpiringPointer<ContinuousFunction> f =
      modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  if (type == k_functionCellType) {
    functionCell->setLayout(f->layout());
    functionCell->setMessage(
        Preferences::sharedPreferences->examMode().forbidImplicitPlots()
            ? I18n::Message::Default
            : f->properties().caption());
    KDColor textColor = f->isActive() ? KDColorBlack : Palette::GrayDark;
    functionCell->setTextColor(textColor);
    static_cast<FunctionCell *>(functionCell)
        ->setParameterSelected(m_parameterColumnSelected);
  }
  KDColor functionColor = f->isActive() ? f->color() : Palette::GrayDark;
  functionCell->setColor(functionColor);
  functionCell->reloadCell();
}

FunctionToolbox *ListController::toolbox() { return &m_functionToolbox; }

void ListController::addModel() {
  Container::activeApp()->displayModalViewController(
      &m_modelsStackController, 0.f, 0.f, Metric::PopUpTopMargin,
      Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

ContinuousFunctionStore *ListController::modelStore() const {
  return App::app()->functionStore();
}

}  // namespace Graph
