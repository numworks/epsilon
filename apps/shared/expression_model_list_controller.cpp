#include "expression_model_list_controller.h"
#include <apps/constant.h>
#include <escher/container.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/selectable_table_view.h>
#include <ion/events.h>
#include <layout_events.h>
#include <poincare/symbol.h>
#include <algorithm>

using namespace Escher;

namespace Shared {

/* Table Data Source */

ExpressionModelListController::ExpressionModelListController(Responder * parentResponder, I18n::Message text) :
  ViewController(parentResponder),
  m_addNewModel(KDFont::LargeFont, KDContext::k_alignLeft)
{
  resetMemoization();
  m_addNewModel.setMessage(text);
}

void ExpressionModelListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  int currentSelectedRow = selectedRow();

  /* Update m_cumulatedHeightForSelectedIndex if we scrolled one cell up/down.
   * We want previousSelectedCellY >= 0 and currentSelectedRow >= 0 to ensure
   * that a cell is selected before and after the change.
   * (previousSelectedCellY >= 0 condition is enough as
   * currentSelectedRow > previousSelectedCellY) */
  if (previousSelectedCellY >= 0 && currentSelectedRow == previousSelectedCellY + 1) {
    /* We selected the cell under the previous cell. Shift the memoized cell
     * heights. */
    shiftMemoization(true);
    resetMemoizationForIndex(k_memoizedCellsCount-1);
    // Update m_cumulatedHeightForSelectedIndex
    if (previousSelectedCellY >= 0) {
      m_cumulatedHeightForSelectedIndex+= memoizedRowHeight(previousSelectedCellY);
    } else {
      assert(currentSelectedRow == 0);
      m_cumulatedHeightForSelectedIndex = 0;
    }
  /* We ensure that a cell is selected before and after the selection change by
   * checking that previousSelectedCellY > currentSelectedRow >= 0. */
  } else if (currentSelectedRow >= 0 && currentSelectedRow == previousSelectedCellY - 1) {
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

KDCoordinate ExpressionModelListController::memoizedRowHeight(int j) {
  if (j < 0) {
    return 0;
  }
  int currentSelectedRow = selectedRow() < 0 ? 0 : selectedRow();
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

KDCoordinate ExpressionModelListController::memoizedCumulatedHeightFromIndex(int j) {
  if (j <= 0) {
    return 0;
  }
  int currentSelectedRow = selectedRow() < 0 ? 0 : selectedRow();
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

int ExpressionModelListController::memoizedIndexFromCumulatedHeight(KDCoordinate offsetY) {
  if (offsetY == 0) {
    return 0;
  }
  /* We use memoization to speed up this method: if offsetY is "around" the
   * memoized cumulatedHeightForIndex, we can compute its value easily by
   * adding/substracting memoized row heights. */

  int currentSelectedRow = selectedRow() < 0 ? 0 : selectedRow();
  int rowsCount = numberOfExpressionRows();
  if (rowsCount <= 1 || currentSelectedRow < 1) {
    return notMemoizedIndexFromCumulatedHeight(offsetY);
  }

  KDCoordinate currentCumulatedHeight = memoizedCumulatedHeightFromIndex(currentSelectedRow);
  if (offsetY > currentCumulatedHeight) {
    int iMax = std::min(k_memoizedCellsCount/2 + 1, rowsCount - currentSelectedRow);
    for (int i = 0; i < iMax; i++) {
      currentCumulatedHeight+= memoizedRowHeight(currentSelectedRow + i);
      if (offsetY <= currentCumulatedHeight) {
        return currentSelectedRow + i;
      }
    }
  } else {
    int iMax = std::min(k_memoizedCellsCount/2, currentSelectedRow);
    for (int i = 1; i <= iMax; i++) {
      currentCumulatedHeight-= memoizedRowHeight(currentSelectedRow-i);
      if (offsetY > currentCumulatedHeight) {
        return currentSelectedRow - i;
      }
    }
  }
  return notMemoizedIndexFromCumulatedHeight(offsetY);
}

int ExpressionModelListController::numberOfExpressionRows() const {
  const ExpressionModelStore * store = const_cast<ExpressionModelListController *>(this)->modelStore();
  int modelsCount = store->numberOfModels();
  return modelsCount + (modelsCount == store->maxNumberOfModels() ? 0 : 1);
}

bool ExpressionModelListController::isAddEmptyRow(int j) {
  return j == numberOfExpressionRows() - 1 && modelStore()->numberOfModels() != modelStore()->maxNumberOfModels();
}

KDCoordinate ExpressionModelListController::expressionRowHeight(int j) {
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

void ExpressionModelListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  EvenOddExpressionCell * myCell = static_cast<EvenOddExpressionCell *>(cell);
  ExpiringPointer<ExpressionModelHandle> m = modelStore()->modelForRecord(modelStore()->recordAtIndex(j));
  myCell->setLayout(m->layout());
}

/* Responder */

bool ExpressionModelListController::handleEventOnExpression(Ion::Events::Event event) {
  if (selectedRow() < 0) {
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    if (isAddEmptyRow(selectedRow())) {
      addModel();
      return true;
    }
    editExpression(event);
    return true;
  }
  if (event == Ion::Events::Backspace && !isAddEmptyRow(selectedRow())) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    if (removeModelRow(record)) {
      int newSelectedRow = selectedRow() >= numberOfExpressionRows() ? numberOfExpressionRows()-1 : selectedRow();
      selectCellAtLocation(selectedColumn(), newSelectedRow);
      selectableTableView()->reloadData();
    }
    return true;
  }
  char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
  size_t eventTextLength = Ion::Events::copyText(static_cast<uint8_t>(event), buffer, Ion::Events::EventData::k_maxDataSize);
  if (eventTextLength > 0 || event == Ion::Events::XNT || event == Ion::Events::Paste || event == Ion::Events::Toolbox || event == Ion::Events::Var) {
    // If empty row is selected, try adding an empty model
    if (isAddEmptyRow(selectedRow()) && !addEmptyModel()) {
      // Adding an empty model failed
      return true;
    }
    assert(!isAddEmptyRow(selectedRow()));
    editExpression(event);
    return true;
  }
  return false;
}

void ExpressionModelListController::addModel() {
  if (addEmptyModel()) {
    editExpression(Ion::Events::OK);
  }
}

bool ExpressionModelListController::addEmptyModel() {
  // Return false if empty model couldn't be added.
  Ion::Storage::Record::ErrorStatus error = modelStore()->addEmptyModel();
  if (error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable) {
    return false;
  }
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  didChangeModelsList();
  selectableTableView()->reloadData();
  return true;
}

void ExpressionModelListController::editExpression(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
    ExpiringPointer<ExpressionModelHandle> model = modelStore()->modelForRecord(record);
    constexpr size_t initialTextContentMaxSize = Constant::MaxSerializedExpressionSize;
    char initialTextContent[initialTextContentMaxSize];
    model->text(initialTextContent, initialTextContentMaxSize);
    inputController()->setTextBody(initialTextContent);
  }
  inputController()->edit(event, this,
      [](void * context, void * sender){
        ExpressionModelListController * myController = static_cast<ExpressionModelListController *>(context);
        InputViewController * myInputViewController = (InputViewController *)sender;
        const char * textBody = myInputViewController->textBody();
        return myController->editSelectedRecordWithText(textBody);
      },
      [](void * context, void * sender){
        return true;
      });
}

bool ExpressionModelListController::editSelectedRecordWithText(const char * text) {
  telemetryReportEvent("Edit", text);
  // Reset memoization of the selected cell which always corresponds to the k_memoizedCellsCount/2 memoized cell
  resetMemoizationForIndex(k_memoizedCellsCount/2);
  Ion::Storage::Record record = modelStore()->recordAtIndex(modelIndexForRow(selectedRow()));
  ExpiringPointer<ExpressionModelHandle> model = modelStore()->modelForRecord(record);
  return (model->setContent(text, Container::activeApp()->localContext()) == Ion::Storage::Record::ErrorStatus::None);
}

bool ExpressionModelListController::removeModelRow(Ion::Storage::Record record) {
  modelStore()->removeModel(record);
  didChangeModelsList();
  return true;
}

void ExpressionModelListController::resetMemoizationForIndex(int index) {
  assert(index >= 0 && index < k_memoizedCellsCount);
  m_memoizedCellHeight[index] = k_resetedMemoizedValue;
}

void ExpressionModelListController::shiftMemoization(bool newCellIsUnder) {
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

void ExpressionModelListController::resetMemoization() {
  m_cumulatedHeightForSelectedIndex = k_resetedMemoizedValue;
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    resetMemoizationForIndex(i);
  }
}

}
