#include "list_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <apps/exam_mode_configuration.h>
#include <assert.h>
#include <escher/metric.h>
#include <poincare/code_point_layout.h>
#include <poincare/matrix_layout.h>
#include <poincare/symbol_abstract.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

constexpr CodePoint ListController::k_equationSymbols[];

ListController::ListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer) :
  Shared::FunctionListController(parentResponder, header, footer, I18n::Message::AddFunction),
  Shared::InputEventHandlerDelegate(),
  m_selectableTableView(this, this, this, this),
  m_parameterController(this, I18n::Message::FunctionColor, I18n::Message::DeleteFunction, this),
  m_modelsParameterController(this, nullptr, this),
  m_modelsStackController(nullptr, &m_modelsParameterController, StackViewController::Style::PurpleWhite),
  m_parameterColumnSelected(false)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setVerticalCellOverlap(0);
}

/* TableViewDataSource */

int ListController::typeAtIndex(int index) {
  if (isAddEmptyRow(index)) {
    return k_addNewModelType;
  }
  assert(index >= 0 && index < modelStore()->numberOfModels());
  return k_functionCellType;
}

HighlightCell * ListController::reusableCell(int index, int type) {
  assert(index >= 0 && index < reusableCellCount(type));
  if (type == k_addNewModelType) {
    return &(m_addNewModel);
  }
  assert(type == k_functionCellType);
  return functionCells(index);
}

int ListController::reusableCellCount(int type) {
  if (type == k_addNewModelType) {
    return 1;
  }
  assert(type == k_functionCellType);
  return maxNumberOfDisplayableRows();
}

/* ViewController */

const char * ListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

// Fills buffer with a default function equation, such as "f(x)="
void ListController::fillWithDefaultFunctionEquation(char * buffer, size_t bufferSize, FunctionModelsParameterController * modelsParameterController, CodePoint symbol) const {
  size_t length = modelsParameterController->defaultName(buffer, bufferSize);
  assert(bufferSize > length);
  buffer[length++] = '(';
  length += UTF8Decoder::CodePointToChars(symbol, buffer + length, bufferSize - length);
  assert(bufferSize > length + 2);
  buffer[length++] = ')';
  buffer[length++] = '=';
  buffer[length++] = 0;
}

// Return true if given layout contains a comparison operator
bool ListController::layoutRepresentsAnEquation(Poincare::Layout l) const {
  Poincare::Layout match = l.recursivelyMatches(
      [](Poincare::Layout layout) {
      constexpr size_t k_numberOfSymbols = sizeof(k_equationSymbols)/sizeof(CodePoint);
      if (layout.type() == Poincare::LayoutNode::Type::PiecewiseOperatorLayout) {
        return Poincare::TrinaryBoolean::False;
      }
      if (layout.type() == Poincare::LayoutNode::Type::CodePointLayout) {
        for (size_t i = 0; i < k_numberOfSymbols; i++) {
          if(static_cast<Poincare::CodePointLayout &>(layout).codePoint() == k_equationSymbols[i]) {
          return Poincare::TrinaryBoolean::True;
          }
        }
      }
      return Poincare::TrinaryBoolean::Unknown;
    });
  return !match.isUninitialized();
}

// Return true if given layout contains θ
bool ListController::layoutRepresentsPolarFunction(Poincare::Layout l) const {
  Poincare::Layout match = l.recursivelyMatches(
    [](Poincare::Layout layout) {
      return layout.type() == Poincare::LayoutNode::Type::CodePointLayout && static_cast<Poincare::CodePointLayout &>(layout).codePoint() == ContinuousFunction::k_polarSymbol ? Poincare::TrinaryBoolean::True : Poincare::TrinaryBoolean::Unknown;
    });
  return !match.isUninitialized();
}

/* Return true if given layout contains a Matrix of correct dimensions as first
 * child. Note: A more precise detection would require an expression reduction
 * to distinguish:
 * - sum(1,t,0,100) : Cartesian
 * - norm([[4][5]]) : Cartesian
 * - 31*[[4][5]]*10 : Parametric */
bool ListController::layoutRepresentsParametricFunction(Poincare::Layout l) const {
  if (l.type() == Poincare::LayoutNode::Type::HorizontalLayout && l.numberOfChildren() > 0) {
    l = l.childAtIndex(0);
  }
  if (l.type() != Poincare::LayoutNode::Type::MatrixLayout) {
    return false;
  }
  Poincare::MatrixLayout m = static_cast<Poincare::MatrixLayout &>(l);
  return m.numberOfColumns() == 1 && m.numberOfRows() == 2;
}

bool ListController::completeEquation(InputEventHandler * equationField, CodePoint symbol) {
  // Retrieve the edited function
  ExpiringPointer<ContinuousFunction> f = modelStore()->modelForRecord(modelStore()->recordAtIndex(selectedRow()));
  if (f->isNull() || f->plotType() == ContinuousFunction::PlotType::Undefined) {
    // Function is new or undefined, complete the equation with a default name
    constexpr size_t k_bufferSize = Shared::ContinuousFunction::k_maxDefaultNameSize + sizeof("(θ)=") - 1;
    char buffer[k_bufferSize];
    // Insert "f(x)=", with f the default function name and x the symbol
    fillWithDefaultFunctionEquation(buffer, k_bufferSize, &m_modelsParameterController, symbol);
    return equationField->handleEventWithText(buffer);
  }
  // Insert the name, symbol and equation symbol of the existing function
  constexpr size_t k_bufferSize = Poincare::SymbolAbstract::k_maxNameSize + sizeof("(x)≥") - 1;
  static_assert(sizeof("(x)≥") >= sizeof("(θ)="), "k_bufferSize should fit both situations.");
  // (θ)≥ would not fit, but inequations on polar are not handled.
  assert(f->symbol() != ContinuousFunction::k_polarSymbol || f->equationSymbol() == '=');
  char buffer[k_bufferSize];
  size_t nameLength = f->nameWithArgument(buffer, k_bufferSize);
  nameLength += UTF8Decoder::CodePointToChars(f->equationSymbol(), buffer + nameLength, k_bufferSize - nameLength);
  assert(nameLength < k_bufferSize);
  buffer[nameLength] = 0;
  return equationField->handleEventWithText(buffer);
}

// TODO: factorize with solver
bool ListController::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  m_parameterColumnSelected = false;
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutRepresentsAnEquation(layoutField->layout())) {
      layoutField->putCursorLeftOfLayout();
      CodePoint symbol = layoutRepresentsPolarFunction(layoutField->layout())
                             ? ContinuousFunction::k_polarSymbol
                             : (layoutRepresentsParametricFunction(
                                    layoutField->layout())
                                    ? ContinuousFunction::k_parametricSymbol
                                    : ContinuousFunction::k_cartesianSymbol);
      // Inserted Layout must be an equation
      if (!completeEquation(layoutField, symbol)) {
        layoutField->putCursorRightOfLayout();
        Container::activeApp()->displayWarning(I18n::Message::RequireEquation);
        return true;
      }
    }
  }
  return Shared::LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField, event);
}

/* TextFieldDelegate */

bool ListController::textRepresentsAnEquation(const char * text) const {
  constexpr size_t k_numberOfSymbols = sizeof(k_equationSymbols)/sizeof(CodePoint);
  for (size_t i = 0; i < k_numberOfSymbols; i++) {
    if (UTF8Helper::CodePointIs(UTF8Helper::CodePointSearch(text, k_equationSymbols[i]), k_equationSymbols[i])) {
      return true;
    }
  }
  return false;
}

bool ListController::textRepresentsPolarFunction(const char * text) const {
  return UTF8Helper::CodePointIs(UTF8Helper::CodePointSearch(text, ContinuousFunction::k_polarSymbol), ContinuousFunction::k_polarSymbol);
}

// See ListController::layoutRepresentsParametricFunction comment.
bool ListController::textRepresentsParametricFunction(const char * text) const {
  // Only catch very basic parametric expressions, even if dimension is invalid.
  return text[0] == '[';
}

// TODO: factorize with solver
bool ListController::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->shouldFinishEditing(event)) {
    const char * text = textField->text();
    if (!textRepresentsAnEquation(text)) {
      // Inserted text must be an equation
      textField->setCursorLocation(text);
      CodePoint symbol = textRepresentsPolarFunction(text)
                             ? ContinuousFunction::k_polarSymbol
                             : (textRepresentsParametricFunction(text)
                                    ? ContinuousFunction::k_parametricSymbol
                                    : ContinuousFunction::k_cartesianSymbol);
      if (!completeEquation(textField, symbol)) {
        textField->setCursorLocation(text + strlen(text));
        Container::activeApp()->displayWarning(I18n::Message::RequireEquation);
        return true;
      }
    }
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

/* Responder */

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

KDCoordinate ListController::expressionRowHeight(int j) {
  if (typeAtIndex(j) == k_addNewModelType) {
    return Shared::FunctionListController::expressionRowHeight(j);
  }
  FunctionCell tempCell;
  willDisplayCellForIndex(&tempCell, j);
  return tempCell.minimalSizeForOptimalDisplay().height();
}

bool ListController::handleEvent(Ion::Events::Event event) {
  // Here we handle an additional parameter column, within FunctionCell's button
  if (selectedRow() >= 0 && selectedRow() <= numberOfRows() && !isAddEmptyRow(selectedRow())) {
    // Selected row is a function cell
    if (m_parameterColumnSelected) {
      // Parameter column is selected
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        // Open function parameter menu
        configureFunction(modelStore()->recordAtIndex(modelIndexForRow(selectedRow())));
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
      selectableTableView()->selectCellAtLocation(0, numberOfRows()-1);
      Container::activeApp()->setFirstResponder(selectableTableView());
      return true;
    }
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    Container::activeApp()->setFirstResponder(tabController());
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

Shared::ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfDisplayableRows() {
  return k_maxNumberOfDisplayableRows;
}

HighlightCell * ListController::functionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_expressionCells[index];
}

void ListController::willDisplayCellForIndex(HighlightCell * cell, int j) {
  assert(cell != nullptr);
  EvenOddCell * evenOddCell = static_cast<EvenOddCell *>(cell);
  evenOddCell->setEven(j%2 == 0);
  evenOddCell->setHighlighted(j == selectedRow());
  int type = typeAtIndex(j);
  if (type == k_addNewModelType) {
    evenOddCell->reloadCell();
    return;
  }
  assert(type == k_functionCellType);
  FunctionCell * functionCell = static_cast<FunctionCell *>(cell);
  ExpiringPointer<ContinuousFunction> f = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  functionCell->setLayout(f->layout());
  functionCell->setMessage(ExamModeConfiguration::implicitPlotsAreForbidden() ? I18n::Message::Default : f->plotTypeMessage());
  KDColor functionColor = f->isActive() ? f->color() : Palette::GrayDark;
  functionCell->setColor(functionColor);
  KDColor textColor = f->isActive() ? KDColorBlack : Palette::GrayDark;
  functionCell->setTextColor(textColor);
  functionCell->setParameterSelected(m_parameterColumnSelected);
  functionCell->reloadCell();
}

FunctionToolbox * ListController::toolboxForInputEventHandler(InputEventHandler * textInput) {
  // Set sender
  m_functionToolbox.setSender(textInput);
  return &m_functionToolbox;
}

void ListController::addModel() {
  Container::activeApp()->displayModalViewController(&m_modelsStackController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

ContinuousFunctionStore * ListController::modelStore() {
  return App::app()->functionStore();
}

}
