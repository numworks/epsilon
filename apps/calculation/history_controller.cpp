#include "history_controller.h"
#include "app.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Calculation {

HistoryController::HistoryController(Responder * parentResponder, CalculationStore * calculationStore) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, this),
  m_calculationHistory{},
  m_calculationStore(calculationStore)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_calculationHistory[i].setParentResponder(&m_selectableTableView);
    m_calculationHistory[i].setDataSource(this);
  }
}

void HistoryController::reload() {
  m_selectableTableView.reloadData();
  /* TODO
   * Replace the following by selectCellAtLocation in order to avoid laying out
   * the table view twice.
   */
  if (numberOfRows() > 0) {
    m_selectableTableView.scrollToCell(0, numberOfRows()-1);
    // Force to reload last added cell (hide the burger and exact output if necessary)
    tableViewDidChangeSelection(&m_selectableTableView, 0, numberOfRows()-1);
  }
}

void HistoryController::viewWillAppear() {
  reload();
}

void HistoryController::didBecomeFirstResponder() {
  selectCellAtLocation(0, numberOfRows()-1);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void HistoryController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == parentResponder()) {
    m_selectableTableView.deselectTable();
  }
}

bool HistoryController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down) {
    m_selectableTableView.deselectTable();
    Container::activeApp()->setFirstResponder(parentResponder());
    return true;
  }
  if (event == Ion::Events::Up) {
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    int focusRow = selectedRow();
    HistoryViewCell * selectedCell = (HistoryViewCell *)m_selectableTableView.selectedCell();
    SubviewType subviewType = selectedSubviewType();
    Shared::ExpiringPointer<Calculation> calculation = calculationAtIndex(focusRow);
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    if (subviewType == SubviewType::Input) {
      m_selectableTableView.deselectTable();
      Container::activeApp()->setFirstResponder(editController);
      editController->insertTextBody(calculation->inputText());
    } else {
      ScrollableExactApproximateExpressionsView::SubviewPosition outputSubviewPosition = selectedCell->outputView()->selectedSubviewPosition();
      if (outputSubviewPosition == ScrollableExactApproximateExpressionsView::SubviewPosition::Left) {
        Expression::AdditionalInformationType additionalInfoType = selectedCell->additionalInformationType();
        /* TODO
         * Controller * c = additionalInformationType ? graphController : listController?
         * m_controller->setType(additionalInformationType)*/
        if (additionalInfoType == Expression::AdditionalInformationType::Complex) {
          m_complexController.complexModel()->setComplex(std::complex<float>(1.2f,2.0f));
          Container::activeApp()->displayModalViewController(&m_complexController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, Metric::PopUpTopMargin, Metric::PopUpRightMargin);
        }
      } else {
        m_selectableTableView.deselectTable();
        Container::activeApp()->setFirstResponder(editController);
        if (outputSubviewPosition == ScrollableExactApproximateExpressionsView::SubviewPosition::Right
            && !calculation->shouldOnlyDisplayExactOutput())
        {
          editController->insertTextBody(calculation->approximateOutputText());
        } else {
          editController->insertTextBody(calculation->exactOutputText());
        }
      }
    }
    return true;
  }
  if (event == Ion::Events::Backspace) {
    int focusRow = selectedRow();
    SubviewType subviewType = selectedSubviewType();
    m_selectableTableView.deselectTable();
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    m_calculationStore->deleteCalculationAtIndex(storeIndex(focusRow));
    reload();
    if (numberOfRows()== 0) {
      Container::activeApp()->setFirstResponder(editController);
      return true;
    }
    if (focusRow > 0) {
      m_selectableTableView.selectCellAtLocation(0, focusRow-1);
    } else {
      m_selectableTableView.selectCellAtLocation(0, 0);
    }
    if (subviewType == SubviewType::Input) {
      tableViewDidChangeSelection(&m_selectableTableView, 0, selectedRow());
    } else {
      tableViewDidChangeSelection(&m_selectableTableView, 0, -1);
    }
    m_selectableTableView.scrollToCell(0, selectedRow());
    return true;
  }
  if (event == Ion::Events::Clear) {
    m_selectableTableView.deselectTable();
    m_calculationStore->deleteAll();
    reload();
    Container::activeApp()->setFirstResponder(parentResponder());
    return true;
  }
  if (event == Ion::Events::Back) {
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    m_selectableTableView.deselectTable();
    Container::activeApp()->setFirstResponder(editController);
    return true;
  }
  return false;
}

Shared::ExpiringPointer<Calculation> HistoryController::calculationAtIndex(int i) {
  return m_calculationStore->calculationAtIndex(storeIndex(i));
}

void HistoryController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection || previousSelectedCellY == selectedRow()) {
    return;
  }
  if (previousSelectedCellY == -1) {
    setSelectedSubviewType(SubviewType::Output, previousSelectedCellX, previousSelectedCellY);
  } else if (selectedRow() < previousSelectedCellY) {
    setSelectedSubviewType(SubviewType::Output, previousSelectedCellX, previousSelectedCellY);
  } else if (selectedRow() > previousSelectedCellY) {
    setSelectedSubviewType(SubviewType::Input, previousSelectedCellX, previousSelectedCellY);
  } else if (selectedRow() == -1) {
    setSelectedSubviewType(SubviewType::Input, previousSelectedCellX, previousSelectedCellY);
  }
  HistoryViewCell * selectedCell = (HistoryViewCell *)(t->selectedCell());
  if (selectedCell == nullptr) {
    return;
  }
  Container::activeApp()->setFirstResponder(selectedCell);
}

int HistoryController::numberOfRows() const {
  return m_calculationStore->numberOfCalculations();
};

HighlightCell * HistoryController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0);
  assert(index < k_maxNumberOfDisplayedRows);
  return &m_calculationHistory[index];
}

int HistoryController::reusableCellCount(int type) {
  assert(type == 0);
  return k_maxNumberOfDisplayedRows;
}

void HistoryController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  HistoryViewCell * myCell = (HistoryViewCell *)cell;
  myCell->setCalculation(calculationAtIndex(index).pointer());
  myCell->setEven(index%2 == 0);
  myCell->setHighlighted(myCell->isHighlighted());
}

KDCoordinate HistoryController::rowHeight(int j) {
  if (j >= m_calculationStore->numberOfCalculations()) {
    return 0;
  }
  Shared::ExpiringPointer<Calculation> calculation = calculationAtIndex(j);
  return calculation->height(App::app()->localContext(), j == selectedRow() && selectedSubviewType() == SubviewType::Output) + 4 * Metric::CommonSmallMargin;
}

int HistoryController::typeAtLocation(int i, int j) {
  return 0;
}

void HistoryController::scrollToCell(int i, int j) {
  m_selectableTableView.scrollToCell(i, j);
}

void HistoryController::historyViewCellDidChangeSelection(HistoryViewCell ** cell, HistoryViewCell ** previousCell, int previousSelectedCellX, int previousSelectedCellY) {
  /* Update the whole table as the height of the selected cell row might have
   * changed. */
  m_selectableTableView.reloadData();
  // Fill the selected cell and the previous selected cell because cells repartition might have changed
  *cell = static_cast<HistoryViewCell *>(m_selectableTableView.selectedCell());
  *previousCell = static_cast<HistoryViewCell *>(m_selectableTableView.cellAtLocation(previousSelectedCellX, previousSelectedCellY));
}

}
