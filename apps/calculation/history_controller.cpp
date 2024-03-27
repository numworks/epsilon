#include "history_controller.h"

#include <apps/shared/expression_display_permissions.h>
#include <assert.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>

#include "app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

HistoryController::HistoryController(
    EditExpressionController *editExpressionController,
    CalculationStore *calculationStore)
    : ViewController(editExpressionController),
      m_selectableListView(this, this, this, this),
      m_calculationStore(calculationStore),
      m_additionalResultsController(editExpressionController) {
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_calculationHistory[i].setParentResponder(&m_selectableListView);
    m_calculationHistory[i].setDataSource(this);
  }
}

void HistoryController::reload() {
  // Ensure that the total height never overflows KDCoordinate
  while (m_selectableListView.minimalSizeForOptimalDisplay().height() >=
         KDCOORDINATE_MAX - 2 * HistoryViewCell::k_maxCellHeight) {
    m_calculationStore->deleteCalculationAtIndex(
        m_calculationStore->numberOfCalculations() - 1);
  }
  /* When reloading, we might not used anymore cell that hold previous layouts.
   * We clean them all before reloading their content to avoid taking extra
   * useless space in the Poincare pool. */
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_calculationHistory[i].resetMemoization();
  }

  m_selectableListView.reloadData();
  /* TODO
   * Replace the following by selectCellAtLocation in order to avoid laying out
   * the table view twice.
   */
  if (numberOfRows() > 0) {
    m_selectableListView.scrollToBottom();
    /* Force to reload last added cell (hide the burger and exact output if
     * necessary) */
    listViewDidChangeSelectionAndDidScroll(&m_selectableListView, -1,
                                           KDPointZero);
  }
}

void HistoryController::viewWillAppear() {
  ViewController::viewWillAppear();
  reload();
}

void HistoryController::didBecomeFirstResponder() {
  selectRow(numberOfRows() - 1);
  App::app()->setFirstResponder(&m_selectableListView);
}

void HistoryController::willExitResponderChain(Responder *nextFirstResponder) {
  if (nextFirstResponder == nullptr) {
    return;
  }
  if (nextFirstResponder == parentResponder()) {
    m_selectableListView.deselectTable();
  }
}

bool HistoryController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down || event == Ion::Events::Back ||
      event == Ion::Events::Clear) {
    m_selectableListView.deselectTable();
    if (event == Ion::Events::Clear) {
      m_calculationStore->deleteAll();
      reload();
    }
    App::app()->setFirstResponder(parentResponder());
    return true;
  }
  if (event == Ion::Events::Up) {
    return true;
  }
  if (event == Ion::Events::Backspace) {
    int focusRow = selectedRow();
    SubviewType subviewType = m_selectedSubviewType;
    m_selectableListView.deselectTable();
    m_calculationStore->deleteCalculationAtIndex(storeIndex(focusRow));
    reload();
    if (numberOfRows() == 0) {
      App::app()->setFirstResponder(parentResponder());
      return true;
    }
    m_selectableListView.selectCell(focusRow > 0 ? focusRow - 1 : 0);
    setSelectedSubviewType(subviewType, false);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    handleOK();
    return true;
  }
  return false;
}

Shared::ExpiringPointer<Calculation> HistoryController::calculationAtIndex(
    int i) const {
  return m_calculationStore->calculationAtIndex(storeIndex(i));
}

void HistoryController::listViewDidChangeSelectionAndDidScroll(
    SelectableListView *list, int previousSelectedRow, KDPoint previousOffset,
    bool withinTemporarySelection) {
  assert(list == &m_selectableListView);
  m_selectableListView.didChangeSelectionAndDidScroll();
  if (withinTemporarySelection || previousSelectedRow == selectedRow()) {
    return;
  }
  if (previousSelectedRow == -1) {
    setSelectedSubviewType(SubviewType::Output, false, previousSelectedRow);
  } else if (selectedRow() == -1) {
    setSelectedSubviewType(SubviewType::None, false, previousSelectedRow);
  } else {
    HistoryViewCell *selectedCell =
        static_cast<HistoryViewCell *>(m_selectableListView.selectedCell());
    SubviewType nextSelectedSubviewType = m_selectedSubviewType;
    if (selectedCell && !selectedCell->displaysSingleLine()) {
      nextSelectedSubviewType = previousSelectedRow < selectedRow()
                                    ? SubviewType::Input
                                    : SubviewType::Output;
    }
    setSelectedSubviewType(nextSelectedSubviewType, false, previousSelectedRow);
  }
}

KDPoint HistoryController::offsetToRestoreAfterReload(
    const SelectableTableView *t) const {
  assert(t == &m_selectableListView);
  KDCoordinate delta = 0;
  int selectedRow = m_selectableListView.selectedRow();
  if (0 <= selectedRow &&
      selectedRow < m_calculationStore->numberOfCalculations()) {
    Shared::ExpiringPointer<Calculation> calculation =
        calculationAtIndex(selectedRow);
    delta = calculation->height(true) - calculation->height(false);
  }
  return m_selectableListView.contentOffset().translatedBy(KDPoint(delta, 0));
}

void HistoryController::recomputeHistoryCellHeightsIfNeeded() {
  if (!m_calculationStore->preferencesHaveChanged()) {
    return;
  }
  for (int i = 0; i < numberOfRows(); i++) {
    /* The void context is used since there is no reasons for the
     * heightComputer to resolve symbols */
    HistoryViewCell::ComputeCalculationHeights(calculationAtIndex(i).pointer(),
                                               nullptr);
  }
}

int HistoryController::numberOfRows() const {
  return m_calculationStore->numberOfCalculations();
};

HighlightCell *HistoryController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  return &m_calculationHistory[index];
}

int HistoryController::reusableCellCount(int type) const {
  assert(type == 0);
  return k_maxNumberOfDisplayedRows;
}

void HistoryController::fillCellForRow(HighlightCell *cell, int row) {
  HistoryViewCell *myCell = static_cast<HistoryViewCell *>(cell);
  Poincare::Context *context = App::app()->localContext();
  myCell->setCalculation(
      calculationAtIndex(row).pointer(),
      row == selectedRow() && m_selectedSubviewType == SubviewType::Output,
      context);
  myCell->setEven(row % 2 == 0);
  myCell->reloadSubviewHighlight();
}

KDCoordinate HistoryController::nonMemoizedRowHeight(int row) {
  if (row >= m_calculationStore->numberOfCalculations()) {
    return 0;
  }
  Shared::ExpiringPointer<Calculation> calculation = calculationAtIndex(row);
  bool expanded =
      row == selectedRow() && m_selectedSubviewType == SubviewType::Output;
  return calculation->height(expanded);
}

bool HistoryController::calculationAtIndexToggles(int index) const {
  Context *context = App::app()->localContext();
  return index >= 0 && index < m_calculationStore->numberOfCalculations() &&
         calculationAtIndex(index)->displayOutput(context) ==
             Calculation::DisplayOutput::ExactAndApproximateToggle;
}

void HistoryController::setSelectedSubviewType(SubviewType subviewType,
                                               bool sameCell,
                                               int previousSelectedRow) {
  // Avoid selecting non-displayed ellipsis
  HistoryViewCell *selectedCell =
      static_cast<HistoryViewCell *>(m_selectableListView.selectedCell());
  if (subviewType == SubviewType::Ellipsis && selectedCell &&
      !selectedCell->hasEllipsis()) {
    subviewType = SubviewType::Output;
  }
  SubviewType previousSubviewType = m_selectedSubviewType;
  m_selectedSubviewType = subviewType;
  /* We need to notify the whole table that the selection changed if it
   * involves the selection/deselection of an output. Indeed, only them can
   * trigger change in the displayed expressions. */

  /* If the selection change triggers the toggling of the outputs, we update
   * the whole table as the height of the selected cell row might have changed.
   */
  if ((subviewType == SubviewType::Output ||
       previousSubviewType == SubviewType::Output) &&
      (calculationAtIndexToggles(selectedRow()) ||
       calculationAtIndexToggles(previousSelectedRow))) {
    m_selectableListView.reloadData(false);
  }

  /* It might be necessary to scroll to the subviewType if the cell overflows
   * the screen */
  if (selectedRow() >= 0) {
    m_selectableListView.scrollToSubviewOfTypeOfCellAtRow(
        subviewType, m_selectableListView.selectedRow());
  }
  /* Refill the selected cell and the previous selected cell because cells
   * repartition might have changed */
  selectedCell =
      static_cast<HistoryViewCell *>(m_selectableListView.selectedCell());
  HistoryViewCell *previousSelectedCell = static_cast<HistoryViewCell *>(
      m_selectableListView.cell(previousSelectedRow));
  /* 'reloadData' calls 'fillCellForRow' for each cell while the table
   * has been deselected. To reload the expanded cell, we call one more time
   * 'fillCellForRow' but once the right cell has been selected. */
  if (selectedCell) {
    fillCellForRow(selectedCell, selectedRow());
  }

  previousSubviewType = sameCell ? previousSubviewType : SubviewType::None;
  if (selectedCell) {
    selectedCell->reloadSubviewHighlight();
    selectedCell->cellDidSelectSubview(subviewType, previousSubviewType);
    App::app()->setFirstResponder(selectedCell, true);
  }
  if (previousSelectedCell) {
    previousSelectedCell->cellDidSelectSubview(SubviewType::Input);
  }
}

void HistoryController::handleOK() {
  Context *context = App::app()->localContext();
  int focusRow = selectedRow();
  Calculation::DisplayOutput displayOutput =
      calculationAtIndex(focusRow)->displayOutput(context);
  HistoryViewCell *selectedCell =
      static_cast<HistoryViewCell *>(m_selectableListView.selectedCell());
  EditExpressionController *editController =
      static_cast<EditExpressionController *>(parentResponder());

  if (m_selectedSubviewType == SubviewType::Input) {
    m_selectableListView.deselectTable();
#if 0  // TODO_PCJ
    editController->insertTextBody(calculationAtIndex(focusRow)->inputText());
#else
    assert(false);
#endif
    return;
  }

  if (m_selectedSubviewType == SubviewType::Output) {
    m_selectableListView.deselectTable();
    Shared::ExpiringPointer<Calculation> calculation =
        calculationAtIndex(focusRow);
    ScrollableTwoLayoutsView::SubviewPosition outputSubviewPosition =
        selectedCell->outputView()->selectedSubviewPosition();
    if (outputSubviewPosition ==
            ScrollableTwoLayoutsView::SubviewPosition::Right &&
        displayOutput != Calculation::DisplayOutput::ExactOnly) {
      editController->insertTextBody(calculation->approximateOutputText(
          Calculation::NumberOfSignificantDigits::Maximal));
    } else {
      assert(displayOutput != Calculation::DisplayOutput::ApproximateOnly);
      editController->insertTextBody(calculation->exactOutputText());
    }
    return;
  }

  assert(m_selectedSubviewType == SubviewType::Ellipsis);
  assert(displayOutput != Calculation::DisplayOutput::ExactOnly);
  ExpiringPointer<Calculation> selectedCalculation =
      calculationAtIndex(focusRow);
  Expression i, a, e;
  selectedCalculation->fillExpressionsForAdditionalResults(&i, &e, &a);

  /* Reuse the same complex format and angle unit as when the calculation was
   * computed. */
  m_additionalResultsController.openAdditionalResults(
      selectedCalculation->additionalResultsType(), i, e, a,
      selectedCalculation->calculationPreferences());
}

}  // namespace Calculation
