#include "store_controller.h"
#include <apps/apps_container.h>
#include <apps/shared/poincare_helpers.h>
#include <apps/constant.h>
#include <escher/metric.h>
#include <assert.h>
#include <algorithm>

using namespace Poincare;
using namespace Escher;

namespace Shared {

StoreController::StoreController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, DoublePairStore * store, ButtonRowController * header, Context * parentContext) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_store(store),
  m_dataView(m_store, this, this, this),
  m_templateController(this, this),
  m_templateStackController(nullptr, &m_templateController, StackViewController::Style::PurpleWhite),
  m_storeContext(store, parentContext)
{
  m_dataView.setBackgroundColor(Palette::WallScreenDark);
  m_dataView.setVerticalCellOverlap(0);
  m_dataView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    m_editableCells[i].setParentResponder(&m_dataView);
    m_editableCells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

void StoreController::displayFormulaInput() {
  Container::activeApp()->displayModalViewController(&m_templateStackController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

void StoreController::fillFormulaInputWithTemplate(Layout templateLayout) {
  constexpr int k_sizeOfBuffer = DoublePairStore::k_columnNamesLength + 1 + FormulaTemplateMenuController::k_maxSizeOfTemplateText;
  char templateString[k_sizeOfBuffer];
  int filledLength = fillColumnName(selectedColumn(), templateString);
  if (filledLength < Shared::EditableCellTableViewController::k_maxSizeOfColumnName - 1) {
    templateString[filledLength] = '=';
    templateString[filledLength + 1] = 0;
  }
  if (!templateLayout.isUninitialized()) {
    templateLayout.serializeParsedExpression(templateString + filledLength + 1, k_sizeOfBuffer - filledLength - 1, nullptr);
  }
  inputViewController()->setTextBody(templateString);
  inputViewController()->edit(
      this,
      Ion::Events::OK,
      this,
      [](void * context, void * sender) {
        StoreController * thisController = static_cast<StoreController *>(context);
        InputViewController * thisInputViewController = static_cast<InputViewController *>(sender);
        return thisController->createExpressionForFillingColumnWithFormula(thisInputViewController->textBody());
      }, [](void * context, void * sender) {
        return true;
      });
}

bool StoreController::createExpressionForFillingColumnWithFormula(const char * text) {
  Expression expression = Expression::Parse(text, &m_storeContext);
  if (expression.isUninitialized()) {
    Container::activeApp()->displayWarning(I18n::Message::SyntaxError);
    return false;
  }
  if (fillColumnWithFormula(expression)) {
    Container::activeApp()->setFirstResponder(&m_dataView);
    return true;
  }
  return false;
}

bool StoreController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  int series = m_store->seriesAtColumn(selectedColumn());
  bool wasSeriesValid = m_store->seriesIsValid(series);
  bool result = EditableCellTableViewController::textFieldDidFinishEditing(textField, text, event);
  if (wasSeriesValid != m_store->seriesIsValid(series)) {
    // Series changed validity, series' cells have changed color.
    reloadSeriesVisibleCells(series);
  }
  return result;
}

int StoreController::numberOfColumns() const {
  return DoublePairStore::k_numberOfColumnsPerSeries * DoublePairStore::k_numberOfSeries;
}

KDCoordinate StoreController::columnWidth(int i) {
  return k_cellWidth;
}

KDCoordinate StoreController::cumulatedWidthFromIndex(int i) {
  return i*k_cellWidth;
}

int StoreController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return (offsetX-1) / k_cellWidth;
}

HighlightCell * StoreController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case k_titleCellType:
      assert(index < k_numberOfTitleCells);
      return &m_titleCells[index];
    case k_editableCellType:
      assert(index < k_maxNumberOfEditableCells);
      return &m_editableCells[index];
    default:
      assert(false);
      return nullptr;
  }
}

int StoreController::reusableCellCount(int type) {
  return type == k_titleCellType ? k_numberOfTitleCells : k_maxNumberOfEditableCells;
}

int StoreController::typeAtLocation(int i, int j) {
  return j == 0 ? k_titleCellType : k_editableCellType;
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  // Handle hidden cells
  const int numberOfElementsInCol = numberOfElementsInColumn(i);
  if (j > numberOfElementsInCol + 1) {
    StoreCell * myCell = static_cast<StoreCell *>(cell);
    myCell->editableTextCell()->textField()->setText("");
    myCell->setHide(true);
    return;
  }
  if (typeAtLocation(i, j) == k_editableCellType) {
    Shared::StoreCell * myCell = static_cast<StoreCell *>(cell);
    myCell->setHide(false);
    myCell->setSeparatorLeft(i > 0 && (m_store->relativeColumnIndex(i) == 0));
    KDColor textColor = m_store->seriesIsValid(m_store->seriesAtColumn(i)) ? KDColorBlack : Palette::GrayDark;
    myCell->editableTextCell()->textField()->setTextColor(textColor);
  }
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, Preferences::sharedPreferences()->displayMode());
}

void StoreController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  // Default : put column name in titleCell
  StoreTitleCell * myTitleCell = static_cast<StoreTitleCell *>(cell);
  fillColumnName(columnIndex, const_cast<char *>(myTitleCell->text()));
}

void StoreController::setTitleCellStyle(HighlightCell * cell, int columnIndex) {
  int seriesIndex = m_store->seriesAtColumn(columnIndex);
  int realColumnIndex = m_store->relativeColumnIndex(columnIndex);
  Shared::StoreTitleCell * myCell = static_cast<Shared::StoreTitleCell *>(cell);
  myCell->setColor(!m_store->seriesIsValid(seriesIndex) ? Palette::GrayDark : DoublePairStore::colorOfSeriesAtIndex(seriesIndex)); // TODO Share GrayDark with graph/list_controller
  myCell->setSeparatorLeft(columnIndex > 0 && ( realColumnIndex == 0));
}

const char * StoreController::title() {
  return I18n::translate(I18n::Message::DataTab);
}

bool StoreController::handleEvent(Ion::Events::Event event) {
  if (EditableCellTableViewController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    Container::activeApp()->setFirstResponder(tabController());
    return true;
  }
  assert(selectedColumn() >= 0 && selectedColumn() < numberOfColumns());
  int series = m_store->seriesAtColumn(selectedColumn());
  if (event == Ion::Events::Backspace) {
    if (selectedRow() == 0 || selectedRow() > numberOfElementsInColumn(selectedColumn())) {
      return false;
    }
    if (m_store->deleteValueAtIndex(series, m_store->relativeColumnIndex(selectedColumn()), selectedRow()-1)) {
      // A row has been deleted
      selectableTableView()->reloadData();
    } else {
      reloadSeriesVisibleCells(series);
    }
    return true;
  }
  return false;
}

void StoreController::didBecomeFirstResponder() {
  if (selectedRow() < 0 || selectedColumn() < 0) {
    selectCellAtLocation(0, 0);
  }
  EditableCellTableViewController::didBecomeFirstResponder();
  Container::activeApp()->setFirstResponder(&m_dataView);
}

StackViewController * StoreController::stackController() const {
  return static_cast<StackViewController *>(parentResponder()->parentResponder());
}

Responder * StoreController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

bool StoreController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  return typeAtLocation(columnIndex, rowIndex) == k_editableCellType;
}

bool StoreController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  m_store->set(floatBody, m_store->seriesAtColumn(columnIndex), m_store->relativeColumnIndex(columnIndex), rowIndex-1, true);
  return true;
}

double StoreController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_store->get(m_store->seriesAtColumn(columnIndex), m_store->relativeColumnIndex(columnIndex), rowIndex-1);
}

int StoreController::numberOfElementsInColumn(int columnIndex) const {
  return m_store->numberOfPairsOfSeries(m_store->seriesAtColumn(columnIndex));
}

void StoreController::reloadSeriesVisibleCells(int series, int relativeColumn) {
  // Reload visible cells of the series and, if not -1, relative column
  for (int i = 0; i < numberOfColumns(); i++) {
    if (m_store->seriesAtColumn(i) == series && (relativeColumn == -1 || relativeColumn == m_store->relativeColumnIndex(i))) {
      selectableTableView()->reloadVisibleCellsAtColumn(i);
    }
  }
}

bool displayNotSuitableWarning() {
   Container::activeApp()->displayWarning(I18n::Message::DataNotSuitable);
   return false;
}

bool StoreController::fillColumnWithFormula(Expression formula) {
  int columnToFill = m_store->relativeColumnIndex(selectedColumn());
  int seriesToFill = m_store->seriesAtColumn(selectedColumn());
  if (formula.type() == ExpressionNode::Type::Equal) {
    bool isValidEquality = false;
    Expression leftOfEqual = formula.childAtIndex(0);
    if (leftOfEqual.type() == ExpressionNode::Type::Symbol) {
      Symbol symbolLeftOfEqual = static_cast<Symbol &>(leftOfEqual);
      if (m_store->isColumnName(symbolLeftOfEqual.name(), strlen(symbolLeftOfEqual.name()), &seriesToFill, &columnToFill)) {
        formula = formula.childAtIndex(1);
        isValidEquality = true;
      }
    }
    if (!isValidEquality) {
      return displayNotSuitableWarning();
    }
  }
  PoincareHelpers::CloneAndSimplify(&formula, &m_storeContext, ExpressionNode::ReductionTarget::SystemForApproximation, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  if (formula.isUndefined()) {
      return displayNotSuitableWarning();
  }
  if (formula.type() == ExpressionNode::Type::List) {
    bool allChildrenAreUndefined = true;
    for (int i = 0; i < formula.numberOfChildren(); i++) {
      if (!formula.childAtIndex(i).isUndefined()) {
        allChildrenAreUndefined = false;
        break;
      }
    }
    if (allChildrenAreUndefined) {
      return displayNotSuitableWarning();
    }
    m_store->setList(static_cast<List &>(formula), seriesToFill, columnToFill);
    selectableTableView()->reloadData();
    return true;
  }
  double evaluation = PoincareHelpers::ApproximateToScalar<double>(formula, &m_storeContext);
  if (std::isnan(evaluation)) {
      return displayNotSuitableWarning();
  }
  for (int j = 0; j < m_store->numberOfPairsOfSeries(seriesToFill); j++) {
    m_store->set(evaluation, seriesToFill, columnToFill, j);
  }
  reloadSeriesVisibleCells(selectedSeries());
  return true;
}

void StoreController::sortSelectedColumn() {
  m_store->sortColumn(selectedSeries(), m_store->relativeColumnIndex(selectedColumn()));
}

bool StoreController::switchSelectedColumnHideStatus() {
  int series = selectedSeries();
  bool previousStatus = m_store->seriesIsValid(series);
  if (previousStatus) {
    // Any previously valid series can be hidden
    m_store->hideSeries(series);
    return true;
  } else {
    // Series may still be invalid, in that case nothing happens
    m_store->updateSeriesValidity(series);
    return m_store->seriesIsValid(series);
  }
}

}
