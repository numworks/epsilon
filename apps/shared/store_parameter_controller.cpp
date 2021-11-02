#include "store_parameter_controller.h"
#include <assert.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/stack_view_controller.h>
#include <escher/container.h>
#include <poincare/helpers.h>
#include "store_controller.h"

using namespace Escher;

namespace Shared {

StoreParameterController::StoreParameterController(Responder * parentResponder, DoublePairStore * store, StoreController * storeController) :
  SelectableListViewController(parentResponder),
  m_store(store),
  m_series(0),
  m_confirmPopUpController(
    Invocation([](void * context, void * parent){
      StoreParameterController * param = static_cast<StoreParameterController *>(context);
      param->deleteColumn();
      return true;
    }, this),
    {I18n::Message::ConfirmDeleteColumn1, I18n::Message::ConfirmDeleteColumn2}
  ),
  m_cells{I18n::Message::FillWithFormula, I18n::Message::ClearColumn},
  m_sortCell(I18n::Message::SortCellLabel),
  m_storeController(storeController),
  m_xColumnSelected(true)
{ }

void StoreParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(index >= 0 && index < k_totalNumberOfCell);
  if (index == k_indexOfSortValues) {
    assert(cell == &m_sortCell);
    m_sortCell.setSubLabelMessage(sortMessage());
  }
  ListViewDataSource::willDisplayCellForIndex(cell, index);
}

const char * StoreParameterController::title() {
  return I18n::translate(I18n::Message::ColumnOptions);
}

void StoreParameterController::viewWillAppear() {
  resetMemoization();
  m_selectableTableView.reloadData();
}

void StoreParameterController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool StoreParameterController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::OK && event != Ion::Events::EXE) {
    return false;
  }
  switch (selectedRow()) {
    case k_indexOfRemoveColumn:
    {
      // Display confirmation popup before removing column
      Container::activeApp()->displayModalViewController(&m_confirmPopUpController, 0.f, 0.f,
          Metric::PopUpTopMargin, Metric::PopUpRightMargin, Metric::PopUpBottomMargin, Metric::PopUpLeftMargin);
      return true;
    }
    case k_indexOfFillFormula:
    {
      m_storeController->displayFormulaInput();
      break;
    }
    case k_indexOfSortValues:
    {
      sortColumn();
      break;
    }
  }
  assert(selectedRow() >= 0 && selectedRow() < k_totalNumberOfCell);
  popFromStackView();
  return true;
}

HighlightCell * StoreParameterController::reusableCell(int index, int type) {
  assert(index >= 0);
  if (type == k_sortCellType) {
    return &m_sortCell;
  }
  assert(type == k_defaultCellType);
  return &m_cells[index];
}

KDCoordinate StoreParameterController::nonMemoizedRowHeight(int index) {
  /* We just need to find a cell of the right type
   * heightForCellAtIndex will do the rest */
  Escher::HighlightCell * cell;
  if (index == k_indexOfSortValues) {
    cell = &m_sortCell;
  } else {
    assert(typeAtIndex(index) == k_defaultCellType);
    cell = reusableCell(0, k_defaultCellType);
  }
  return heightForCellAtIndex(cell, index);
}

void StoreParameterController::deleteColumn() {
  if (m_xColumnSelected) {
    m_store->deleteAllPairsOfSeries(m_series);
  } else {
    m_store->resetColumn(m_series, 1);
  }
  Container::activeApp()->dismissModalViewController(false);
  popFromStackView();
}

void StoreParameterController::popFromStackView() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
}

void StoreParameterController::sortColumn() {
  static Poincare::Helpers::Swap swapRows = [](int i, int j, void * context, int numberOfElements) {
    // Swap X and Y values
    double * dataX = static_cast<double*>(context);
    double * dataY = static_cast<double*>(context) + DoublePairStore::k_maxNumberOfPairs;
    double tempX = dataX[i];
    double tempY = dataY[i];
    dataX[i] = dataX[j];
    dataY[i] = dataY[j];
    dataX[j] = tempX;
    dataY[j] = tempY;
  };
  static Poincare::Helpers::Compare compareX = [](int a, int b, void * context, int numberOfElements)->bool{
    double * dataX = static_cast<double*>(context);
    return dataX[a] > dataX[b];
  };
  static Poincare::Helpers::Compare compareY = [](int a, int b, void * context, int numberOfElements)->bool{
    double * dataY = static_cast<double*>(context) + DoublePairStore::k_maxNumberOfPairs;
    return dataY[a] > dataY[b];
  };

  int indexOfSeries = m_series * DoublePairStore::k_numberOfColumnsPerSeries * DoublePairStore::k_maxNumberOfPairs;
  double * seriesContext = &(m_store->data()[indexOfSeries]);
  Poincare::Helpers::Sort(swapRows, m_xColumnSelected ? compareX : compareY, seriesContext, m_store->numberOfPairsOfSeries(m_series));

}

}
