#include "list_controller.h"
#include "../app.h"
#include <assert.h>
#include <algorithm>
#include <apps/i18n.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

constexpr KDCoordinate ListController::k_minTitleColumnWidth;

ListController::ListController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, ButtonRowController * footer) :
  Shared::FunctionListController(parentResponder, header, footer, I18n::Message::AddSequence),
  m_selectableTableView(this, this, this, this),
  m_parameterController(inputEventHandlerDelegate, this),
  m_typeParameterController(this, this),
  m_typeStackController(nullptr, &m_typeParameterController, StackViewController::Style::PurpleWhite),
  m_titlesColumnWidth(k_minTitleColumnWidth)
{
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_expressionCells[i].setLeftMargin(k_expressionMargin);
  }
  /* m_memoizedCellBaseline is not initialized by the call to
   * resetMemoizationForIndex in ExpressionModelListController's
   * constructor, because it is a virtual method in a constructor. */
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    m_memoizedCellBaseline[i] = -1;
  }
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setVerticalCellOverlap(0);
}

const char * ListController::title() {
  return I18n::translate(I18n::Message::SequenceTab);
}

int ListController::numberOfExpressionRows() const {
  int numberOfRows = 0;
  SequenceStore * store = const_cast<ListController *>(this)->modelStore();
  const int modelsCount = store->numberOfModels();
  for (int i = 0; i < modelsCount; i++) {
    Shared::Sequence * sequence = store->modelForRecord(store->recordAtIndex(i));
    numberOfRows += sequence->numberOfElements();
  }
  return numberOfRows + (modelsCount == store->maxNumberOfModels()? 0 : 1);
};

KDCoordinate ListController::expressionRowHeight(int j) {
  KDCoordinate defaultHeight = Metric::StoreRowHeight;
  if (isAddEmptyRow(j)) {
    return defaultHeight;
  }
  Shared::Sequence * sequence = modelStore()->modelForRecord(modelStore()->recordAtIndex(modelIndexForRow(j)));
  Layout layout = sequence->layout();
  if (sequenceDefinitionForRow(j) == 1) {
    layout = sequence->firstInitialConditionLayout();
  }
  if (sequenceDefinitionForRow(j) == 2) {
    layout = sequence->secondInitialConditionLayout();
  }
  if (layout.isUninitialized()) {
    return defaultHeight;
  }
  KDCoordinate sequenceHeight = layout.layoutSize().height();
  return std::max<KDCoordinate>(defaultHeight, sequenceHeight + 2*k_expressionCellVerticalMargin);
}

Toolbox * ListController::toolboxForInputEventHandler(InputEventHandler * textInput) {
  // Set extra cells
  int recurrenceDepth = -1;
  int sequenceDefinition = sequenceDefinitionForRow(selectedRow());
  Shared::Sequence * sequence = modelStore()->modelForRecord(modelStore()->recordAtIndex(modelIndexForRow(selectedRow())));
  if (sequenceDefinition == 0) {
    recurrenceDepth = sequence->numberOfElements()-1;
  }
  m_sequenceToolbox.buildExtraCellsLayouts(sequence->fullName(), recurrenceDepth);
  // Set sender
  m_sequenceToolbox.setSender(textInput);
  return &m_sequenceToolbox;
}

void ListController::selectPreviousNewSequenceCell() {
  if (sequenceDefinitionForRow(selectedRow()) >= 0) {
    selectCellAtLocation(selectedColumn(), selectedRow()-sequenceDefinitionForRow(selectedRow()));
  }
}

void ListController::editExpression(int sequenceDefinition, Ion::Events::Event event) {
  Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
  Shared::Sequence * sequence = modelStore()->modelForRecord(record);
  InputViewController * inputController = Shared::FunctionApp::app()->inputViewController();
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    char initialTextContent[Constant::MaxSerializedExpressionSize];
    switch (sequenceDefinition) {
      case 0:
        sequence->text(initialTextContent, sizeof(initialTextContent));
        break;
      case 1:
        sequence->firstInitialConditionText(initialTextContent, sizeof(initialTextContent));
        break;
      default:
        sequence->secondInitialConditionText(initialTextContent, sizeof(initialTextContent));
        break;
    }
    inputController->setTextBody(initialTextContent);
  }
  // Invalidate the sequences context cache
  App::app()->localContext()->resetCache();
  switch (sequenceDefinition) {
    case 0:
      inputController->edit(this, event, this,
        [](void * context, void * sender){
        ListController * myController = static_cast<ListController *>(context);
        InputViewController * myInputViewController = (InputViewController *)sender;
        const char * textBody = myInputViewController->textBody();
        return myController->editSelectedRecordWithText(textBody);
        },
        [](void * context, void * sender){
        return true;
      });
      break;
  case 1:
    inputController->edit(this, event, this,
      [](void * context, void * sender){
      ListController * myController = static_cast<ListController *>(context);
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      return myController->editInitialConditionOfSelectedRecordWithText(textBody, true);
      },
      [](void * context, void * sender){
      return true;
    });
    break;
  default:
    inputController->edit(this, event, this,
      [](void * context, void * sender){
      ListController * myController = static_cast<ListController *>(context);
      InputViewController * myInputViewController = (InputViewController *)sender;
      const char * textBody = myInputViewController->textBody();
      return myController->editInitialConditionOfSelectedRecordWithText(textBody, false);
      },
      [](void * context, void * sender){
      return true;
    });
  }
}

/* ViewController */

void ListController::viewWillAppear() {
  ExpressionModelListController::viewWillAppear();
  computeTitlesColumnWidth();
}

KDCoordinate ListController::columnWidth(int i) {
  switch (i) {
    case 0:
      return m_titlesColumnWidth;
    case 1:
      return selectableTableView()->bounds().width()-m_titlesColumnWidth;
    default:
      assert(false);
      return 0;
  }
}

KDCoordinate ListController::cumulatedWidthFromIndex(int i) {
  switch (i) {
    case 0:
      return 0;
    case 1:
      return m_titlesColumnWidth;
    case 2:
      return selectableTableView()->bounds().width();
    default:
      assert(false);
      return 0;
  }
}

int ListController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  if (offsetX <= m_titlesColumnWidth) {
    return 0;
  } else {
    if (offsetX <= selectableTableView()->bounds().width())
      return 1;
    else {
      return 2;
    }
  }
}

int ListController::typeAtLocation(int i, int j) {
  if (isAddEmptyRow(j)){
    return i + 2;
  }
  return i;
}

HighlightCell * ListController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < maxNumberOfDisplayableRows());
  switch (type) {
    case 0:
      return titleCells(index);
    case 1:
      return functionCells(index);
    case 2:
      return &m_emptyCell;
    case 3:
      return &(m_addNewModel);
    default:
      assert(false);
      return nullptr;
  }
}

int ListController::reusableCellCount(int type) {
  if (type > 1) {
    return 1;
  }
  return maxNumberOfDisplayableRows();
}

void ListController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  if (!isAddEmptyRow(j)) {
    if (i == 0) {
      willDisplayTitleCellAtIndex(cell, j);
    } else {
      willDisplayExpressionCellAtIndex(cell, j);
    }
  }
  EvenOddCell * myCell = static_cast<EvenOddCell *>(cell);
  myCell->setEven(modelIndexForRow(j)%2 == 0);
  myCell->setHighlighted(i == selectedColumn() && j == selectedRow());
  myCell->reloadCell();
}

/* Responder */

void ListController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(1, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  if (selectedRow() >= numberOfRows()) {
    selectCellAtLocation(selectedColumn(), numberOfRows()-1);
  }
  footer()->setSelectedButton(-1);
  Container::activeApp()->setFirstResponder(selectableTableView());
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    if (selectedRow() == -1) {
      footer()->setSelectedButton(-1);
      selectableTableView()->selectCellAtLocation(1, numberOfRows()-1);
      Container::activeApp()->setFirstResponder(selectableTableView());
      return true;
    }
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    Container::activeApp()->setFirstResponder(tabController());
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
  if (selectedColumn() == 1) {
    return handleEventOnExpression(event);
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(selectedColumn() == 0);
    configureFunction(modelStore()->recordAtIndex(modelIndexForRow(selectedRow())));
    return true;
  }
  if (event == Ion::Events::Backspace) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    if (removeModelRow(record)) {
      int newSelectedRow = selectedRow() >= numberOfRows() ? numberOfRows()-1 : selectedRow();
      selectCellAtLocation(selectedColumn(), newSelectedRow);
      selectableTableView()->reloadData();
    }
    return true;
  }
  return false;
}

/* SelectableTableViewDelegate*/

void ListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  // Update memoization of cell heights
  ExpressionModelListController::tableViewDidChangeSelection(t, previousSelectedCellX, previousSelectedCellY, withinTemporarySelection);
  // Do not select the cell left of the "addEmptyFunction" cell
  if (isAddEmptyRow(selectedRow()) && selectedColumn() == 0) {
    t->selectCellAtLocation(1, numberOfRows()-1);
  }
}

void ListController::computeTitlesColumnWidth(bool forceMax) {
  if (forceMax) {
    m_titlesColumnWidth = nameWidth(Poincare::SymbolAbstract::k_maxNameSize + Shared::Function::k_parenthesedArgumentCodePointLength - 1)+k_functionTitleSumOfMargins;
    return;
  }
  KDCoordinate maxTitleWidth = maxFunctionNameWidth()+k_functionTitleSumOfMargins;
  m_titlesColumnWidth = std::max(maxTitleWidth, k_minTitleColumnWidth);
}

void ListController::resetMemoizationForIndex(int index) {
  assert(index >= 0 && index < k_memoizedCellsCount);
  m_memoizedCellBaseline[index] = -1;
  ExpressionModelListController::resetMemoizationForIndex(index);
}

void ListController::shiftMemoization(bool newCellIsUnder) {
  if (newCellIsUnder) {
    for (int i = 0; i < k_memoizedCellsCount - 1; i++) {
      m_memoizedCellBaseline[i] = m_memoizedCellBaseline[i+1];
    }
  } else {
    for (int i = k_memoizedCellsCount - 1; i > 0; i--) {
      m_memoizedCellBaseline[i] = m_memoizedCellBaseline[i-1];
    }
  }
  ExpressionModelListController::shiftMemoization(newCellIsUnder);
}

bool ListController::editInitialConditionOfSelectedRecordWithText(const char * text, bool firstInitialCondition) {
  // Reset memoization of the selected cell which always corresponds to the k_memoizedCellsCount/2 memoized cell
  resetMemoizationForIndex(k_memoizedCellsCount/2);
  Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
  Shared::Sequence * sequence = modelStore()->modelForRecord(record);
  Context * context = App::app()->localContext();
  Ion::Storage::Record::ErrorStatus error = firstInitialCondition? sequence->setFirstInitialConditionContent(text, context) : sequence->setSecondInitialConditionContent(text, context);
  return (error == Ion::Storage::Record::ErrorStatus::None);
}

ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfDisplayableRows() {
  return k_maxNumberOfRows;
}

FunctionTitleCell * ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_sequenceTitleCells[index];
}

HighlightCell * ListController::functionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}

void ListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  assert(j>=0 && j < k_maxNumberOfRows);
  SequenceTitleCell * myCell = static_cast<SequenceTitleCell *>(cell);
  // Update the corresponding expression cell to get its baseline
  willDisplayExpressionCellAtIndex(m_selectableTableView.cellAtLocation(1, j), j);
  myCell->setBaseline(baseline(j));
  // Set the layout
  Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(j));
  Shared::Sequence * sequence = modelStore()->modelForRecord(record);
  if (sequenceDefinitionForRow(j) == 0) {
    myCell->setLayout(sequence->definitionName());
  }
  if (sequenceDefinitionForRow(j) == 1) {
    myCell->setLayout(sequence->firstInitialConditionName());
  }
  if (sequenceDefinitionForRow(j) == 2) {
    myCell->setLayout(sequence->secondInitialConditionName());
  }
  // All cells except the last of each sequence should connect to the cell below
  myCell->connectColorIndicator(sequenceDefinitionForRow(j) + 1 < sequence->numberOfElements());
  // Set the color
  KDColor nameColor = sequence->isActive() ? sequence->color() : Palette::GrayDark;
  myCell->setColor(nameColor);
}

void ListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  FunctionExpressionCell * myCell = static_cast<FunctionExpressionCell *>(cell);
  Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(j));
  Shared::Sequence * sequence = modelStore()->modelForRecord(record);
  if (sequenceDefinitionForRow(j) == 0) {
    myCell->setLayout(sequence->layout());
  }
  if (sequenceDefinitionForRow(j) == 1) {
    myCell->setLayout(sequence->firstInitialConditionLayout());
  }
  if (sequenceDefinitionForRow(j) == 2) {
    myCell->setLayout(sequence->secondInitialConditionLayout());
  }
  bool active = sequence->isActive();
  KDColor textColor = active ? KDColorBlack : Palette::GrayDark;
  myCell->setTextColor(textColor);
}

int ListController::modelIndexForRow(int j) {
  if (j < 0) {
    return j;
  }
  if (isAddEmptyRow(j)) {
    return modelIndexForRow(j-1)+1;
  }
  int rowIndex = 0;
  int sequenceIndex = -1;
  do {
    sequenceIndex++;
    Shared::Sequence * sequence = modelStore()->modelForRecord(modelStore()->recordAtIndex(sequenceIndex));
    rowIndex += sequence->numberOfElements();
  } while (rowIndex <= j);
  return sequenceIndex;
}

int ListController::sequenceDefinitionForRow(int j) {
  if (j < 0) {
    return j;
  }
  if (isAddEmptyRow(j)) {
    return 0;
  }
  int rowIndex = 0;
  int sequenceIndex = -1;
  Shared::Sequence * sequence;
  do {
    sequenceIndex++;
    sequence = modelStore()->modelForRecord(modelStore()->recordAtIndex(sequenceIndex));
    rowIndex += sequence->numberOfElements();
  } while (rowIndex <= j);
  assert(sequence);
  return sequence->numberOfElements()-rowIndex+j;
}

KDCoordinate ListController::maxFunctionNameWidth() {
  int maxNameLength = 0;
  int numberOfModels = modelStore()->numberOfModels();
  for (int i = 0; i < numberOfModels; i++) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(i);
    const char * functionName = record.fullName();
    const char * dotPosition = strchr(functionName, Ion::Storage::Record::k_dotChar);
    assert(dotPosition != nullptr);
    maxNameLength = std::max(maxNameLength, static_cast<int>(dotPosition-functionName));
  }
  return nameWidth(maxNameLength + Shared::Function::k_parenthesedArgumentCodePointLength);
}

void ListController::didChangeModelsList() {
  ExpressionModelListController::didChangeModelsList();
  computeTitlesColumnWidth();
}

KDCoordinate ListController::baseline(int j) {
  if (j < 0) {
    return -1;
  }
  int currentSelectedRow = selectedRow();
  constexpr int halfMemoizationCount = k_memoizedCellsCount/2;
  if (j >= currentSelectedRow - halfMemoizationCount && j <= currentSelectedRow + halfMemoizationCount) {
    int memoizedIndex = j - (currentSelectedRow - halfMemoizationCount);
    if (m_memoizedCellBaseline[memoizedIndex] < 0) {
      m_memoizedCellBaseline[memoizedIndex] = privateBaseline(j);
    }
    return m_memoizedCellBaseline[memoizedIndex];
  }
  return privateBaseline(j);
}

KDCoordinate ListController::privateBaseline(int j) const {
  assert(j >= 0 && j < const_cast<ListController *>(this)->numberOfExpressionRows());
  FunctionExpressionCell * cell = static_cast<Shared::FunctionExpressionCell *>((const_cast<SelectableTableView *>(&m_selectableTableView))->cellAtLocation(1, j));
  Poincare::Layout layout = cell->layout();
  if (layout.isUninitialized()) {
    return -1; // Baseline < 0 triggers default behaviour (centered alignment)
  }
  return 0.5*(const_cast<ListController *>(this)->rowHeight(j)-layout.layoutSize().height())+layout.baseline();
}

void ListController::addModel() {
  Container::activeApp()->displayModalViewController(&m_typeStackController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, 0, Metric::PopUpLeftMargin);
}

void ListController::editExpression(Ion::Events::Event event) {
  editExpression(sequenceDefinitionForRow(selectedRow()), event);
}

bool ListController::removeModelRow(Ion::Storage::Record record) {
  Shared::FunctionListController::removeModelRow(record);
  // Invalidate the sequences context cache
  App::app()->localContext()->resetCache();
  return true;
}

SequenceStore * ListController::modelStore() {
  return App::app()->functionStore();
}

KDCoordinate ListController::nameWidth(int nameLength) const {
  assert(nameLength >= 0);
  return nameLength * const_cast<ListController *>(this)->titleCells(0)->font()->glyphSize().width();
}

}
