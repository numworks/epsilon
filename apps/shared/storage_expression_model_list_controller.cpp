#include "storage_expression_model_list_controller.h"
#include <apps/constant.h>
#include <poincare/symbol.h>

namespace Shared {

/* Table Data Source */

StorageExpressionModelListController::StorageExpressionModelListController(Responder * parentResponder, I18n::Message text) :
  ViewController(parentResponder),
  m_addNewModel()
{
  resetMemoization();
  m_addNewModel.setMessage(text);
}

void StorageExpressionModelListController::tableSelectionDidChange(int previousSelectedRow) {
  constexpr int currentSelectedMemoizedIndex = k_memoizedCellsCount/2 + 1; // Needs k_memoizedCellsCount to be odd, which is static asserted in the header file
  int currentSelectedRow = selectedRow();

  // The previously selected cell's height might have changed.
  resetMemoizationForIndex(currentSelectedMemoizedIndex);

  // Update m_cumulatedHeightForSelectedIndex if we scrolled one cell up/down
  if (previousSelectedRow >= 0 && currentSelectedRow == previousSelectedRow + 1) {
    /* We selected the cell under the previous cell. Shift the memoized cell
     * heights. */
    shiftMemoization(true);
    resetMemoizationForIndex(k_memoizedCellsCount-1);
    // Update m_cumulatedHeightForSelectedIndex
    if (previousSelectedRow >= 0) {
      m_cumulatedHeightForSelectedIndex+= memoizedRowHeight(previousSelectedRow);
    } else {
      assert(currentSelectedRow == 0);
      m_cumulatedHeightForSelectedIndex = 0;
    }
  } else if (currentSelectedRow >= 0 && currentSelectedRow == previousSelectedRow - 1) {
    /* We selected the cell above the previous cell. Shift the memoized cell
     * heights. */
    shiftMemoization(false);
    resetMemoizationForIndex(0);
    // Update m_cumulatedHeightForSelectedIndex
    if (currentSelectedRow >= 0) {
      m_cumulatedHeightForSelectedIndex-= memoizedRowHeight(currentSelectedRow);
    } else {
      m_cumulatedHeightForSelectedIndex = 0;
    }
  } else if (previousSelectedCellY != currentSelectedRow || previousSelectedCellY < 0) {
    resetMemoization();
  }
}

KDCoordinate StorageExpressionModelListController::memoizedRowHeight(int j) {
  if (j < 0) {
    return 0;
  }
  int currentSelectedRow = selectedRow();
  constexpr int halfMemoizationCount = k_memoizedCellsCount/2;
  if (j >= currentSelectedRow - halfMemoizationCount && j <= currentSelectedRow + halfMemoizationCount) {
    int memoizedIndex = j - (currentSelectedRow - halfMemoizationCount);
    if (m_memoizedCellHeight[memoizedIndex] == k_resetedMemoizedValue) {
      m_memoizedCellHeight[memoizedIndex] = expressionRowHeight(j);
    }
    return m_memoizedCellHeight[memoizedIndex];
  }
  return expressionRowHeight(j);
}

KDCoordinate StorageExpressionModelListController::memoizedCumulatedHeightFromIndex(int j) {
  if (j <= 0) {
    return 0;
  }
  int currentSelectedRow = selectedRow();
  constexpr int halfMemoizationCount = k_memoizedCellsCount/2;
  /* If j is not easily computable from the memoized values, compute it the hard
   * way. */
  if (j < currentSelectedRow - halfMemoizationCount || j > currentSelectedRow + halfMemoizationCount) {
    return notMemoizedCumulatedHeightFromIndex(j);
  }
  // Recompute the memoized cumulatedHeight if needed
  if (m_cumulatedHeightForSelectedIndex == k_resetedMemoizedValue) {
    m_cumulatedHeightForSelectedIndex = notMemoizedCumulatedHeightFromIndex(currentSelectedRow);
  }
  /* Compute the wanted cumulated height by adding/removing memoized cell
   * heights */
  KDCoordinate result = m_cumulatedHeightForSelectedIndex;
  if (j <= currentSelectedRow) {
    /* If j is smaller than the selected row, remove cell heights from the
     * memoized value */
    for (int i = j; i < currentSelectedRow; i++) {
      result -= memoizedRowHeight(i);
    }
  } else {
    /* If j is bigger than the selected row, add cell heights to the memoized
     * value */
    assert(j > currentSelectedRow && j <= currentSelectedRow + halfMemoizationCount);
    for (int i = currentSelectedRow; i < j; i++) {
      result += memoizedRowHeight(i);
    }
  }
  return result;
}

int StorageExpressionModelListController::numberOfExpressionRows() {
  return 1 + modelStore()->numberOfModels();
}

KDCoordinate StorageExpressionModelListController::expressionRowHeight(int j) {
  if (isAddEmptyRow(j)) {
    return Metric::StoreRowHeight;
  }
  ExpiringPointer<ExpressionModelHandle> m = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  if (m->layout().isUninitialized()) {
    return Metric::StoreRowHeight;
  }
  KDCoordinate modelHeight = m->layout().layoutSize().height();
  KDCoordinate modelHeightWithMargins = modelHeight + Metric::StoreRowHeight - KDFont::LargeFont->glyphSize().height();
  return Metric::StoreRowHeight > modelHeightWithMargins ? Metric::StoreRowHeight : modelHeightWithMargins;
}

void StorageExpressionModelListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  EvenOddExpressionCell * myCell = (EvenOddExpressionCell *)cell;
  ExpiringPointer<ExpressionModelHandle> m = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  myCell->setLayout(m->layout());
}

/* Responder */

bool StorageExpressionModelListController::handleEventOnExpression(Ion::Events::Event event) {
  if (selectedRow() < 0) {
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (isAddEmptyRow(selectedRow())) {
      addEmptyModel();
      return true;
    }
    editExpression(event);
    return true;
  }
  if (event == Ion::Events::Backspace && !isAddEmptyRow(selectedRow())) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    ExpiringPointer<ExpressionModelHandle> model =  modelStore()->modelForRecord(record);
    if (model->shouldBeClearedBeforeRemove()) {
      reinitExpression(model);
    } else {
      if (removeModelRow(record)) {
        int newSelectedRow = selectedRow() >= numberOfExpressionRows() ? numberOfExpressionRows()-1 : selectedRow();
        selectCellAtLocation(selectedColumn(), newSelectedRow);
        selectableTableView()->reloadData();
      }
    }
    return true;
  }
  if ((event.hasText() || event == Ion::Events::XNT || event == Ion::Events::Paste || event == Ion::Events::Toolbox || event == Ion::Events::Var)
      && !isAddEmptyRow(selectedRow())) {
    editExpression(event);
    return true;
  }
  return false;
}
void StorageExpressionModelListController::addEmptyModel() {
  Ion::Storage::Record::ErrorStatus error = modelStore()->addEmptyModel();
  if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
    return;
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  didChangeModelsList();
  selectableTableView()->reloadData();
  editExpression(Ion::Events::OK);
}

void StorageExpressionModelListController::reinitExpression(ExpiringPointer<ExpressionModelHandle> model) {
  model->setContent("");
  resetMemoization();
  selectableTableView()->reloadData();
}

void StorageExpressionModelListController::editExpression(Ion::Events::Event event) {
  char * initialText = nullptr;
  constexpr int initialTextContentMaxSize = Constant::MaxSerializedExpressionSize;
  char initialTextContent[initialTextContentMaxSize];
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    ExpiringPointer<ExpressionModelHandle> model = modelStore()->modelForRecord(record);
    model->text(initialTextContent, initialTextContentMaxSize);
    initialText = initialTextContent;
    // Replace Poincare::Symbol::SpecialSymbols::UnknownX with 'x'
    size_t initialTextLength = strlen(initialText);
    for (size_t i = 0; i < initialTextLength; i++) {
      if (initialTextContent[i] == Poincare::Symbol::SpecialSymbols::UnknownX) {
        initialTextContent[i] = Poincare::Symbol::k_unknownXReadableChar;
      }
    }
  }
  inputController()->edit(this, event, this, initialText,
      [](void * context, void * sender){
        StorageExpressionModelListController * myController = static_cast<StorageExpressionModelListController *>(context);
        InputViewController * myInputViewController = (InputViewController *)sender;
        const char * textBody = myInputViewController->textBody();
        myController->resetMemoization();
        return myController->editSelectedRecordWithText(textBody);
      },
      [](void * context, void * sender){
        return true;
      });
}

bool StorageExpressionModelListController::editSelectedRecordWithText(const char * text) {
  Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
  ExpiringPointer<ExpressionModelHandle> model = modelStore()->modelForRecord(record);
  return (model->setContent(text) == Ion::Storage::Record::ErrorStatus::None);
}

bool StorageExpressionModelListController::removeModelRow(Ion::Storage::Record record) {
  modelStore()->removeModel(record);
  didChangeModelsList();
  return true;
}

bool StorageExpressionModelListController::isAddEmptyRow(int j) {
  return j == modelStore()->numberOfModels();
}

void StorageExpressionModelListController::resetMemoizationForIndex(int index) {
  assert(index >= 0 && index < k_memoizedCellsCount);
  m_memoizedCellHeight[index] = k_resetedMemoizedValue;
}

void StorageExpressionModelListController::shiftMemoization(bool newCellIsUnder) {
  if (newCellIsUnder) {
    for (int i = 0; i < k_memoizedCellsCount - 1; i++) {
      m_memoizedCellHeight[i] = m_memoizedCellHeight[i+1];
    }
  } else {
    for (int i = k_memoizedCellsCount - 1; i > 0; i--) {
      m_memoizedCellHeight[i] = m_memoizedCellHeight[i-1];
    }
  }
}

void StorageExpressionModelListController::resetMemoization() {
  m_cumulatedHeightForSelectedIndex = k_resetedMemoizedValue;
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    resetMemoizationForIndex(i);
  }
}

}
