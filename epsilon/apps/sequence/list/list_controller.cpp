#include "list_controller.h"

#include <apps/global_preferences.h>
#include <assert.h>
#include <poincare/helpers/symbol.h>

#include <algorithm>

#include "../app.h"
#include "poincare/helpers/sequence.h"
#include "shared/global_store.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

ListController::ListController(Responder* parentResponder,
                               ButtonRowController* header,
                               ButtonRowController* footer)
    : Shared::FunctionListController(parentResponder, header, footer,
                                     I18n::Message::AddSequence),
      m_editableCell(this, this),
      m_parameterController(this),
      m_typeParameterController(this, this),
      m_typeStackController(nullptr, &m_typeParameterController,
                            StackViewController::Style::PurpleWhite) {}

KDCoordinate ListController::expressionRowHeight(int row) {
  assert(typeAtRow(row) == k_expressionCellType);
  Shared::Sequence* sequence = modelStore()->modelForRecord(recordAtRow(row));
  Layout layout = sequence->aggregatedLayout();
  KDCoordinate sequenceHeight =
      layout.isUninitialized() ? 0 : layout->layoutSize(k_font).height();
  return sequenceHeight + 2 * k_defaultVerticalMargin;
}

/* ViewController */

void ListController::viewWillAppear() {
  // Reset memoization because a sequence could have been deleted
  selectableListView()->resetSizeAndOffsetMemoization();
  ExpressionModelListController::viewWillAppear();
  App::app()->defaultToolbox()->setExtraCellsDataSource(
      &m_sequenceToolboxDataSource);
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
  if (type == k_editableCellType && isAddingNewModel()) {
    AbstractSequenceCell* sequenceCell =
        static_cast<AbstractSequenceCell*>(cell);
    sequenceCell->setColor(Palette::GrayDark);
    sequenceCell->setParameterSelected(m_parameterColumnSelected);
  } else if (type != k_addNewModelCellType) {
    assert(type == k_expressionCellType || type == k_editableCellType);
    int sequenceDefinition;
    Ion::Storage::Record record = recordAtRow(row, &sequenceDefinition);
    Shared::Sequence* sequence = modelStore()->modelForRecord(record);
    AbstractSequenceCell* sequenceCell =
        static_cast<AbstractSequenceCell*>(cell);
    // Set color
    KDColor nameColor =
        sequence->isActive() ? sequence->color() : Palette::GrayDark;
    sequenceCell->setColor(nameColor);
    if (type == k_expressionCellType) {
      fillExpressionCellForRow(sequenceCell->mainCell(), row);
    }
    sequenceCell->setParameterSelected(m_parameterColumnSelected);
  }
  FunctionListController::fillCellForRow(cell, row);
}

/* MathLayoutFieldDelegate */

void ListController::layoutFieldDidAbortEditing(LayoutField* layoutField) {
  // Parent's layoutFieldDidAbortEditing will set m_isAddingNewModel to false
  bool wasAddingNewModel = isAddingNewModel();
  ExpressionModelListController::layoutFieldDidAbortEditing(layoutField);
  if (wasAddingNewModel) {
    return;
  }

  // Remove sequence if main expression is empty
  Shared::Sequence* sequence = modelStore()->modelForRecord(selectedRecord());
  if (sequence->layout().isEmpty() && removeModelRow(selectedRecord())) {
    int newSelectedRow =
        selectedRow() >= numberOfRows() ? numberOfRows() - 1 : selectedRow();
    selectRow(newSelectedRow);
    selectableListView()->reloadData();
  }
}

bool ListController::layoutFieldDidReceiveEvent(LayoutField* layoutField,
                                                Ion::Events::Event event) {
  if (event == Ion::Events::Toolbox) {
    // Set extra cells
    const char* sequenceName;
    int recurrenceDepth;
    SequenceHelper::RecursiveNotation notation;
    if (isAddingNewModel()) {
      sequenceName = SequenceStore::FirstAvailableName();
      // New model are always explicit.
      recurrenceDepth = 0;
      notation = SequenceHelper::RecursiveNotation::Default;
    } else {
      int sequenceDefinition;
      // FIXME: It seems sequenceDefinition will always be 0 here.
      Shared::Sequence* sequence =
          modelStore()->modelForRecord(selectedRecord(&sequenceDefinition));
      sequenceName = sequence->fullName();
      recurrenceDepth = (sequenceDefinition == k_sequenceDefinition)
                            ? sequence->numberOfElements() - 1
                            : -1;
      notation = sequence->recursiveNotation();
    }
    m_sequenceToolboxDataSource.buildExtraCellsLayouts(
        sequenceName, recurrenceDepth,
        notation == SequenceHelper::RecursiveNotation::Shifted);
  }
  return ExpressionModelListController::layoutFieldDidReceiveEvent(layoutField,
                                                                   event);
}

bool ListController::isAcceptableExpression(
    const UserExpression expression, const SymbolContext& symbolContext) {
  // Only accept scalar values
  return MathLayoutFieldDelegate::isAcceptableExpression(expression,
                                                         symbolContext) &&
         SequenceHelper::IsOfValidDimension(expression, symbolContext);
}

void ListController::editExpression(Ion::Events::Event event) {
  ExpressionModelListController::editExpression(event);
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    // Perform backspace to start with cursor inside layout
    ExpressionModelListController::editExpression(Ion::Events::Backspace);
  }
  m_editableCell.setHighlighted(true);
  // Invalidate the sequences context cache
  App::app()->localContext().resetCache();
}

bool ListController::editSelectedRecordWithLayout(Poincare::Layout layout) {
  if (isAddingNewModel()) {
    // TODO: Add model and setContent at the same time
    if (!addEmptyModel()) {
      return false;
    }
  }
  assert(!isAddingNewModel());
  Ion::Storage::Record record = selectedRecord();
  Shared::Sequence* sequence = modelStore()->modelForRecord(record);
  Ion::Storage::Record::ErrorStatus error;
  error = sequence->setLayoutsForAggregated(layout, App::app()->localContext());
  didChangeModelsList();
  return error == Ion::Storage::Record::ErrorStatus::None;
}

Layout ListController::getLayoutForSelectedRecord() const {
  Ion::Storage::Record record = selectedRecord();
  Shared::Sequence* sequence = modelStore()->modelForRecord(record);
  return sequence->aggregatedLayout();
}

HighlightCell* ListController::functionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return m_sequenceCells[index].expressionCell();
}

void ListController::fillExpressionCellForRow(HighlightCell* cell, int row) {
  assert(typeAtRow(row) == k_expressionCellType);
  Escher::EvenOddExpressionCell* myCell =
      static_cast<Escher::EvenOddExpressionCell*>(cell);
  Ion::Storage::Record record = recordAtRow(row);
  Shared::Sequence* sequence = modelStore()->modelForRecord(record);
  // Set the color
  KDColor textColor = sequence->isActive() ? KDColorBlack : Palette::GrayDark;
  myCell->setTextColor(textColor);
  // Set the layout
  return myCell->setLayout(sequence->aggregatedLayout());
}

void ListController::didChangeModelsList() {
  ExpressionModelListController::didChangeModelsList();
  if (modelStore()->numberOfModels() == 0) {
    App::app()->snapshot()->resetInterval();
  } else {
    App::app()->snapshot()->updateInterval();
  }
}

void ListController::addNewModelAction() {
  App::app()->displayModalViewController(&m_typeStackController, 0.f, 0.f,
                                         Metric::PopUpMarginsNoBottom);
}

bool ListController::removeModelRow(Ion::Storage::Record record) {
  Shared::FunctionListController::removeModelRow(record);
  // Invalidate the sequences context cache
  App::app()->localContext().resetCache();
  return true;
}

SequenceStore* ListController::modelStore() const {
  return &GlobalContextAccessor::SequenceStore();
}

void ListController::showLastSequence() {
  selectableListView()->resetSizeAndOffsetMemoization();
  int lastSequenceRow = numberOfRows() - hasAddElementRow() - 1;
  selectableListView()->scrollToCell(lastSequenceRow);
}

}  // namespace Sequence
