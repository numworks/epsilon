#include "list_controller.h"

#include <apps/global_preferences.h>
#include <assert.h>

#include <algorithm>

#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

constexpr KDCoordinate ListController::k_minTitleColumnWidth;

ListController::ListController(
    Responder *parentResponder,
    Escher::InputEventHandlerDelegate *inputEventHandlerDelegate,
    ButtonRowController *header, ButtonRowController *footer)
    : Shared::FunctionListController(parentResponder, header, footer,
                                     I18n::Message::AddSequence),
      m_selectableTableView(this, this, this, this),
      m_editableCell(this, this, this),
      m_parameterController(inputEventHandlerDelegate, this),
      m_typeParameterController(this, this),
      m_typeStackController(nullptr, &m_typeParameterController,
                            StackViewController::Style::PurpleWhite),
      m_titlesColumnWidth(k_minTitleColumnWidth),
      m_heightManager(this) {
  m_editableCell.setMargins(k_expressionMargin, k_expressionMargin);
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i].setLeftMargin(k_expressionMargin);
    m_expressionCells[i].setRightMargin(0);
  }
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setVerticalCellOverlap(0);
}

int ListController::numberOfExpressionRows() const {
  int numberOfRows = 0;
  SequenceStore *store = const_cast<ListController *>(this)->modelStore();
  const int modelsCount = store->numberOfModels();
  for (int i = 0; i < modelsCount; i++) {
    Shared::Sequence *sequence = store->modelForRecord(store->recordAtIndex(i));
    numberOfRows += sequence->numberOfElements();
  }
  return numberOfRows + (modelsCount == store->maxNumberOfModels() ? 0 : 1);
};

KDCoordinate ListController::expressionRowHeight(int j) {
  KDCoordinate defaultHeight = Metric::StoreRowHeight;
  KDCoordinate sequenceHeight;
  if (j == m_editedCellIndex) {
    sequenceHeight = m_editableCell.minimalSizeForOptimalDisplay().height();
  } else {
    if (isAddEmptyRow(j)) {
      return defaultHeight;
    }
    Shared::Sequence *sequence = modelStore()->modelForRecord(
        modelStore()->recordAtIndex(modelIndexForRow(j)));
    Layout layout = sequence->layout();
    int sequenceDefinition = sequenceDefinitionForRow(j);
    if (sequenceDefinition == k_firstInitialCondition) {
      layout = sequence->firstInitialConditionLayout();
    } else if (sequenceDefinition == k_secondInitialCondition) {
      layout = sequence->secondInitialConditionLayout();
    }
    if (layout.isUninitialized()) {
      return defaultHeight;
    }
    sequenceHeight = layout.layoutSize(k_font).height();
  }
  return std::max<KDCoordinate>(
      defaultHeight, sequenceHeight + 2 * k_expressionCellVerticalMargin);
}

Toolbox *ListController::toolbox() {
  // Set extra cells
  int recurrenceDepth = -1;
  int row = selectedRow();
  Shared::Sequence *sequence = modelStore()->modelForRecord(
      modelStore()->recordAtIndex(modelIndexForRow(row)));
  if (sequenceDefinitionForRow(row) == k_sequenceDefinition) {
    recurrenceDepth = sequence->numberOfElements() - 1;
  }
  m_sequenceToolbox.buildExtraCellsLayouts(sequence->fullName(),
                                           recurrenceDepth);
  return &m_sequenceToolbox;
}

void ListController::selectPreviousNewSequenceCell() {
  int row = selectedRow();
  if (sequenceDefinitionForRow(row) >= 0) {
    selectCellAtLocation(selectedColumn(), row - sequenceDefinitionForRow(row));
  }
}

/* ViewController */

void ListController::viewWillAppear() {
  resetMemoization();  // A sequence could have been deleted
  ExpressionModelListController::viewWillAppear();
  computeTitlesColumnWidth();
}

KDCoordinate ListController::nonMemoizedColumnWidth(int i) {
  switch (i) {
    case 0:
      return m_titlesColumnWidth;
    default:
      assert(i == 1);
      return selectableTableView()->bounds().width() - m_titlesColumnWidth;
  }
}

int ListController::typeAtLocation(int i, int j) {
  assert(i == 0 || i == 1);
  if (i == 1 && j == m_editedCellIndex) {
    return k_editableCellType;
  }
  return isAddEmptyRow(j) ? k_emptyRowCellType + i : i;
}

HighlightCell *ListController::reusableCell(int index, int type) {
  assert(index >= 0 && index < maxNumberOfDisplayableRows());
  switch (type) {
    case k_titleCellType:
      return titleCells(index);
    case k_expressionCellType:
      return functionCells(index);
    case k_editableCellType:
      return &m_editableCell;
    case k_emptyRowCellType:
      return &m_emptyCell;
    default:
      assert(type == k_addModelCellType);
      return &(m_addNewModel);
  }
}

void ListController::willDisplayCellAtLocation(HighlightCell *cell, int i,
                                               int j) {
  if (!isAddEmptyRow(j)) {
    if (i == 0) {
      willDisplayTitleCellAtIndex(cell, j);
    } else {
      willDisplayExpressionCellAtIndex(cell, j);
    }
  }
  if (cell == &m_editableCell) {
    return;
  }
  EvenOddCell *myCell = static_cast<EvenOddCell *>(cell);
  myCell->setEven(modelIndexForRow(j) % 2 == 0);
  myCell->reloadCell();
}

/* Responder */

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      footer()->setSelectedButton(-1);
      selectableTableView()->selectCellAtLocation(1, numberOfRows() - 1);
      Container::activeApp()->setFirstResponder(selectableTableView());
      return true;
    }
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    tabController()->selectTab();
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
  if (selectedRow() < 0) {
    return false;
  }
  if (event == Ion::Events::Backspace && !isAddEmptyRow(selectedRow())) {
    assert(selectedRow() >= 0);
    Ion::Storage::Record record =
        modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    if (removeModelRow(record)) {
      int newSelectedRow =
          selectedRow() >= numberOfRows() ? numberOfRows() - 1 : selectedRow();
      selectCellAtLocation(selectedColumn(), newSelectedRow);
      selectableTableView()->reloadData();
    }
    return true;
  }
  if (selectedColumn() == 1) {
    return handleEventOnExpression(event);
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedColumn() == 0);
    configureFunction(
        modelStore()->recordAtIndex(modelIndexForRow(selectedRow())));
    return true;
  }
  return false;
}

/* SelectableTableViewDelegate*/

void ListController::tableViewDidChangeSelection(
    SelectableTableView *t, int previousSelectedCellX,
    int previousSelectedCellY, bool withinTemporarySelection) {
  // Update memoization of cell heights
  ExpressionModelListController::tableViewDidChangeSelection(
      t, previousSelectedCellX, previousSelectedCellY,
      withinTemporarySelection);
  // Do not select the cell left of the "addEmptyFunction" cell
  if (isAddEmptyRow(selectedRow()) && selectedColumn() == 0) {
    t->selectCellAtLocation(1, numberOfRows() - 1);
  }
}

/* LayoutFieldDelegate */
bool ListController::layoutFieldDidReceiveEvent(LayoutField *layoutField,
                                                Ion::Events::Event event) {
  // Do not accept empty input
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event) &&
      layoutField->isEmpty()) {
    App::app()->displayWarning(I18n::Message::SyntaxError);
    return true;
  }
  return LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField, event);
}

void ListController::computeTitlesColumnWidth(bool forceMax) {
  if (forceMax) {
    m_titlesColumnWidth =
        nameWidth(Poincare::SymbolAbstractNode::k_maxNameLength +
                  Shared::Function::k_parenthesedArgumentCodePointLength) +
        k_functionTitleSumOfMargins;
    return;
  }
  KDCoordinate maxTitleWidth =
      maxFunctionNameWidth() + k_functionTitleSumOfMargins;
  m_titlesColumnWidth = std::max(maxTitleWidth, k_minTitleColumnWidth);
}

void ListController::editExpression(Ion::Events::Event event) {
  ExpressionModelListController::editExpression(event);
  m_editableCell.setHighlighted(true);
  // Invalidate the sequences context cache
  App::app()->localContext()->resetCache();
}

bool ListController::editSelectedRecordWithText(const char *text) {
  Ion::Storage::Record record =
      modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
  Shared::Sequence *sequence = modelStore()->modelForRecord(record);
  Context *context = App::app()->localContext();
  Ion::Storage::Record::ErrorStatus error;
  switch (sequenceDefinitionForRow(selectedRow())) {
    case k_sequenceDefinition:
      error = sequence->setContent(text, context);
      break;
    case k_firstInitialCondition:
      error = sequence->setFirstInitialConditionContent(text, context);
      break;
    default:
      error = sequence->setSecondInitialConditionContent(text, context);
  }
  didChangeModelsList();
  return error == Ion::Storage::Record::ErrorStatus::None;
}

void ListController::getTextForSelectedRecord(char *text, size_t size) const {
  Ion::Storage::Record record =
      modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
  Shared::Sequence *sequence = modelStore()->modelForRecord(record);
  switch (sequenceDefinitionForRow(selectedRow())) {
    case k_sequenceDefinition:
      sequence->text(text, size);
      break;
    case k_firstInitialCondition:
      sequence->firstInitialConditionText(text, size);
      break;
    default:
      sequence->secondInitialConditionText(text, size);
      break;
  }
}

HighlightCell *ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_sequenceTitleCells[index];
}

HighlightCell *ListController::functionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}

void ListController::willDisplayTitleCellAtIndex(HighlightCell *cell, int j) {
  assert(j >= 0 && j < k_maxNumberOfRows);
  VerticalSequenceTitleCell *myCell =
      static_cast<VerticalSequenceTitleCell *>(cell);
  // Update the corresponding expression cell to get its baseline
  willDisplayExpressionCellAtIndex(m_selectableTableView.cellAtLocation(1, j),
                                   j);
  myCell->setBaseline(baseline(j));

  Ion::Storage::Record record =
      modelStore()->recordAtIndex(modelIndexForRow(j));
  Shared::Sequence *sequence = modelStore()->modelForRecord(record);
  // Set the color
  KDColor nameColor =
      sequence->isActive() ? sequence->color() : Palette::GrayDark;
  myCell->setColor(nameColor);
  // Set the layout
  int sequenceDefinition = sequenceDefinitionForRow(j);
  switch (sequenceDefinition) {
    case k_sequenceDefinition:
      return myCell->setLayout(sequence->definitionName());
    case k_firstInitialCondition:
      return myCell->setLayout(sequence->firstInitialConditionName());
    default:
      assert(sequenceDefinition == k_secondInitialCondition);
      return myCell->setLayout(sequence->secondInitialConditionName());
  }
}

void ListController::willDisplayExpressionCellAtIndex(HighlightCell *cell,
                                                      int j) {
  if (j == m_editedCellIndex) {
    return;
  }
  Escher::EvenOddExpressionCell *myCell =
      static_cast<Escher::EvenOddExpressionCell *>(cell);
  Ion::Storage::Record record =
      modelStore()->recordAtIndex(modelIndexForRow(j));
  Shared::Sequence *sequence = modelStore()->modelForRecord(record);
  // Set the color
  KDColor textColor = sequence->isActive() ? KDColorBlack : Palette::GrayDark;
  myCell->setTextColor(textColor);
  // Set the layout
  int sequenceDefinition = sequenceDefinitionForRow(j);
  switch (sequenceDefinition) {
    case k_sequenceDefinition:
      return myCell->setLayout(sequence->layout());
    case k_firstInitialCondition:
      return myCell->setLayout(sequence->firstInitialConditionLayout());
    default:
      assert(sequenceDefinition == k_secondInitialCondition);
      return myCell->setLayout(sequence->secondInitialConditionLayout());
  }
}

int ListController::modelIndexForRow(int j) const {
  if (j < 0) {
    return j;
  }
  if (isAddEmptyRow(j)) {
    return modelIndexForRow(j - 1) + 1;
  }
  int rowIndex = 0;
  int sequenceIndex = -1;
  do {
    sequenceIndex++;
    Shared::Sequence *sequence = modelStore()->modelForRecord(
        modelStore()->recordAtIndex(sequenceIndex));
    rowIndex += sequence->numberOfElements();
  } while (rowIndex <= j);
  return sequenceIndex;
}

int ListController::sequenceDefinitionForRow(int j) const {
  if (j < 0) {
    return k_otherDefinition;
  }
  if (isAddEmptyRow(j)) {
    return k_sequenceDefinition;
  }
  int rowIndex = 0;
  int sequenceIndex = -1;
  Shared::Sequence *sequence;
  do {
    sequenceIndex++;
    sequence = modelStore()->modelForRecord(
        modelStore()->recordAtIndex(sequenceIndex));
    rowIndex += sequence->numberOfElements();
  } while (rowIndex <= j);
  assert(sequence);
  int sequenceDefinition = j + sequence->numberOfElements() - rowIndex;
  assert(sequenceDefinition >= k_sequenceDefinition &&
         sequenceDefinition <= k_secondInitialCondition);
  return sequenceDefinition;
}

KDCoordinate ListController::maxFunctionNameWidth() {
  int maxNameLength = 0;
  int numberOfModels = modelStore()->numberOfModels();
  for (int i = 0; i < numberOfModels; i++) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(i);
    const char *functionName = record.fullName();
    const char *dotPosition =
        strchr(functionName, Ion::Storage::Record::k_dotChar);
    assert(dotPosition != nullptr);
    maxNameLength =
        std::max(maxNameLength, static_cast<int>(dotPosition - functionName));
  }
  return nameWidth(maxNameLength +
                   Shared::Function::k_parenthesedArgumentCodePointLength);
}

void ListController::didChangeModelsList() {
  ExpressionModelListController::didChangeModelsList();
  computeTitlesColumnWidth();
  if (modelStore()->numberOfModels() == 0) {
    App::app()->snapshot()->resetInterval();
  } else {
    App::app()->snapshot()->updateInterval();
  }
}

KDCoordinate ListController::baseline(int j) {
  assert(j >= 0 &&
         j < const_cast<ListController *>(this)->numberOfExpressionRows());
  Escher::HighlightCell *cell = static_cast<Escher::HighlightCell *>(
      (const_cast<SelectableTableView *>(&m_selectableTableView))
          ->cellAtLocation(1, j));
  Poincare::Layout layout = cell->layout();
  if (layout.isUninitialized()) {
    return -1;  // Baseline < 0 triggers default behaviour (centered alignment)
  }
  return 0.5 * (const_cast<ListController *>(this)->rowHeight(j) -
                layout.layoutSize(k_font).height()) +
         layout.baseline(k_font);
}

void ListController::addModel() {
  Container::activeApp()->displayModalViewController(
      &m_typeStackController, 0.f, 0.f, Metric::PopUpTopMargin,
      Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

bool ListController::removeModelRow(Ion::Storage::Record record) {
  Shared::FunctionListController::removeModelRow(record);
  // Invalidate the sequences context cache
  App::app()->localContext()->resetCache();
  return true;
}

SequenceStore *ListController::modelStore() const {
  return App::app()->functionStore();
}

KDCoordinate ListController::nameWidth(int nameLength) const {
  assert(nameLength >= 0);
  return nameLength * KDFont::GlyphWidth(VerticalSequenceTitleCell::k_font);
}

void ListController::showLastSequence() {
  resetSizesMemoization();
  SequenceStore *store = const_cast<ListController *>(this)->modelStore();
  bool hasAddSequenceButton =
      store->numberOfModels() == store->maxNumberOfModels();
  int lastRow = numberOfExpressionRows() - (hasAddSequenceButton ? 0 : 1) - 1;
  m_selectableTableView.scrollToCell(1, lastRow);
}
}  // namespace Sequence
