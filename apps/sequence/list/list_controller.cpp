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

ListController::ListController(Responder* parentResponder,
                               ButtonRowController* header,
                               ButtonRowController* footer)
    : Shared::FunctionListController(parentResponder, header, footer,
                                     I18n::Message::AddSequence),
      m_editableCell(this, this),
      m_parameterController(this),
      m_typeParameterController(this, this),
      m_typeStackController(nullptr, &m_typeParameterController,
                            StackViewController::Style::PurpleWhite),
      m_titlesColumnWidth(k_minTitleColumnWidth) {
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_sequenceCells[i].expressionCell()->setMargins({k_expressionMargin, 0});
  }
}

KDCoordinate ListController::expressionRowHeight(int row) {
  assert(typeAtRow(row) == k_expressionCellType);
  KDCoordinate sequenceHeight;
  int sequenceDefinition;
  Shared::Sequence* sequence =
      modelStore()->modelForRecord(recordAtRow(row, &sequenceDefinition));
  Layout layout;
  if (sequenceDefinition == k_firstInitialCondition) {
    layout = sequence->firstInitialConditionLayout();
  } else if (sequenceDefinition == k_secondInitialCondition) {
    layout = sequence->secondInitialConditionLayout();
  } else {
    assert(sequenceDefinition == k_sequenceDefinition);
    layout = sequence->layout();
  }
  sequenceHeight =
      layout.isUninitialized() ? 0 : layout->layoutSize(k_font).height();
  return sequenceHeight + 2 * k_defaultVerticalMargin;
}

void ListController::selectPreviousNewSequenceCell() {
  int row = selectedRow();
  int sequenceDefinition;
  modelIndexForRow(row, &sequenceDefinition);
  if (sequenceDefinition >= 0) {
    selectRow(row - sequenceDefinition);
  }
}

/* ViewController */

void ListController::viewWillAppear() {
  // Reset memoization because a sequence could have been deleted
  selectableListView()->resetSizeAndOffsetMemoization();
  ExpressionModelListController::viewWillAppear();
  App::app()->defaultToolbox()->setExtraCellsDataSource(
      &m_sequenceToolboxDataSource);
  computeTitlesColumnWidth();
}

void ListController::viewDidDisappear() {
  ExpressionModelListController::viewDidDisappear();
  App::app()->defaultToolbox()->setExtraCellsDataSource(nullptr);
}

HighlightCell* ListController::reusableCell(int index, int type) {
  assert(index >= 0 && index < maxNumberOfDisplayableRows());
  switch (type) {
    case k_expressionCellType:
      return &m_sequenceCells[index];
    case k_editableCellType:
      return &m_editableCell;
    default:
      assert(type == k_addNewModelCellType);
      return &m_addNewModelCell;
  }
}

void ListController::fillCellForRow(HighlightCell* cell, int row) {
  int type = typeAtRow(row);
  if (type != k_addNewModelCellType) {
    assert(type == k_expressionCellType || type == k_editableCellType);
    AbstractSequenceCell* sequenceCell =
        static_cast<AbstractSequenceCell*>(cell);
    // Update the expression cell first since the title's baseline depends on it
    if (type == k_expressionCellType) {
      fillExpressionCellForRow(sequenceCell->mainCell(), row);
    }
    fillTitleCellForRow(sequenceCell->titleCell(), row,
                        sequenceCell->mainCell());
    sequenceCell->setParameterSelected(m_parameterColumnSelected);
  }
  FunctionListController::fillCellForRow(cell, row);
}

/* MathLayoutFieldDelegate */

bool ListController::layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                                Ion::Events::Event event) {
  if (event == Ion::Events::Toolbox) {
    // Set extra cells
    int recurrenceDepth = -1;
    int sequenceDefinition;
    Shared::Sequence* sequence =
        modelStore()->modelForRecord(selectedRecord(&sequenceDefinition));
    if (sequenceDefinition == k_sequenceDefinition) {
      recurrenceDepth = sequence->numberOfElements() - 1;
    }
    m_sequenceToolboxDataSource.buildExtraCellsLayouts(sequence->fullName(),
                                                       recurrenceDepth);
  }
  return ExpressionModelListController::layoutFieldDidReceiveEvent(layoutField,
                                                                   event);
}

bool ListController::isAcceptableExpression(const UserExpression expression,
                                            Context* context) {
  // Do not accept any OperatorType.
  return MathLayoutFieldDelegate::isAcceptableExpression(expression, context) &&
         !NewExpression::IsComparison(expression);
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

bool ListController::editSelectedRecordWithLayout(Poincare::Layout layout) {
  int sequenceDefinition;
  Ion::Storage::Record record = selectedRecord(&sequenceDefinition);
  Shared::Sequence* sequence = modelStore()->modelForRecord(record);
  Context* context = App::app()->localContext();
  Ion::Storage::Record::ErrorStatus error;
  switch (sequenceDefinition) {
    case k_sequenceDefinition:
      error = sequence->setContent(layout, context);
      break;
    case k_firstInitialCondition:
      error = sequence->setFirstInitialConditionContent(layout, context);
      break;
    default:
      error = sequence->setSecondInitialConditionContent(layout, context);
  }
  didChangeModelsList();
  return error == Ion::Storage::Record::ErrorStatus::None;
}

Layout ListController::getLayoutForSelectedRecord() const {
  int sequenceDefinition;
  Ion::Storage::Record record = selectedRecord(&sequenceDefinition);
  Shared::Sequence* sequence = modelStore()->modelForRecord(record);
  switch (sequenceDefinition) {
    case k_sequenceDefinition:
      return sequence->layout();
    case k_firstInitialCondition:
      return sequence->firstInitialConditionLayout();
    default:
      return sequence->secondInitialConditionLayout();
  }
}

HighlightCell* ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return m_sequenceCells[index].titleCell();
}

HighlightCell* ListController::functionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return m_sequenceCells[index].expressionCell();
}

void ListController::fillTitleCellForRow(VerticalSequenceTitleCell* cell,
                                         int row,
                                         HighlightCell* expressionCell) {
  assert(row >= 0 && row < k_maxNumberOfRows);
  cell->setBaseline(baseline(row, expressionCell));
  int sequenceDefinition;
  Ion::Storage::Record record = recordAtRow(row, &sequenceDefinition);
  Shared::Sequence* sequence = modelStore()->modelForRecord(record);
  // Set the color
  KDColor nameColor =
      sequence->isActive() ? sequence->color() : Palette::GrayDark;
  cell->setColor(nameColor);
  // Set the layout
  switch (sequenceDefinition) {
    case k_sequenceDefinition:
      return cell->setLayout(sequence->definitionName());
    case k_firstInitialCondition:
      return cell->setLayout(sequence->firstInitialConditionName());
    default:
      assert(sequenceDefinition == k_secondInitialCondition);
      return cell->setLayout(sequence->secondInitialConditionName());
  }
}

void ListController::fillExpressionCellForRow(HighlightCell* cell, int row) {
  assert(typeAtRow(row) == k_expressionCellType);
  Escher::EvenOddExpressionCell* myCell =
      static_cast<Escher::EvenOddExpressionCell*>(cell);
  int sequenceDefinition;
  Ion::Storage::Record record = recordAtRow(row, &sequenceDefinition);
  Shared::Sequence* sequence = modelStore()->modelForRecord(record);
  // Set the color
  KDColor textColor = sequence->isActive() ? KDColorBlack : Palette::GrayDark;
  myCell->setTextColor(textColor);
  // Set the layout
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

KDCoordinate ListController::maxFunctionNameWidth() {
  int maxNameLength = 0;
  int numberOfModels = modelStore()->numberOfModels();
  for (int i = 0; i < numberOfModels; i++) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(i);
    const char* functionName = record.fullName();
    const char* dotPosition =
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

KDCoordinate ListController::baseline(int j, HighlightCell* cell) {
  assert(j >= 0 && j < const_cast<ListController*>(this)->numberOfRows());
  Poincare::Layout layout = cell->layout();
  if (layout.isUninitialized()) {
    return -1;  // Baseline < 0 triggers default behaviour (centered alignment)
  }
  return 0.5 * (const_cast<ListController*>(this)->rowHeight(j) -
                layout->layoutSize(k_font).height()) +
         layout->baseline(k_font);
}

void ListController::addNewModelAction() {
  App::app()->displayModalViewController(&m_typeStackController, 0.f, 0.f,
                                         Metric::PopUpMarginsNoBottom);
}

bool ListController::removeModelRow(Ion::Storage::Record record) {
  Shared::FunctionListController::removeModelRow(record);
  // Invalidate the sequences context cache
  App::app()->localContext()->resetCache();
  return true;
}

SequenceStore* ListController::modelStore() const {
  return App::app()->functionStore();
}

KDCoordinate ListController::nameWidth(int nameLength) const {
  assert(nameLength >= 0);
  return nameLength * KDFont::GlyphWidth(VerticalSequenceTitleCell::k_font);
}

void ListController::showLastSequence() {
  selectableListView()->resetSizeAndOffsetMemoization();
  SequenceStore* store = const_cast<ListController*>(this)->modelStore();
  bool hasAddSequenceButton =
      store->numberOfModels() == store->maxNumberOfModels();
  int lastRow = numberOfRows() - (hasAddSequenceButton ? 0 : 1) - 1;
  selectableListView()->scrollToCell(lastRow);
}

int ListController::numberOfRowsForRecord(Ion::Storage::Record record) const {
  return modelStore()->modelForRecord(record)->numberOfElements();
}

}  // namespace Sequence
