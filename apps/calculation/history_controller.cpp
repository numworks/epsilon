#include "history_controller.h"
#include "app.h"
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>
#include <assert.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

HistoryController::HistoryController(EditExpressionController * editExpressionController, CalculationStore * calculationStore) :
  ViewController(editExpressionController),
  m_selectableTableView(this, this, this, this),
  m_calculationStore(calculationStore),
  m_complexController(editExpressionController),
  m_integerController(editExpressionController),
  m_rationalController(editExpressionController),
  m_trigonometryController(editExpressionController),
  m_unitController(editExpressionController),
  m_matrixController(editExpressionController),
  m_vectorController(editExpressionController)
{
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_calculationHistory[i].setParentResponder(&m_selectableTableView);
    m_calculationHistory[i].setDataSource(this);
  }
}

void HistoryController::reload() {
  /* When reloading, we might not used anymore cell that hold previous layouts.
   * We clean them all before reloading their content to avoid taking extra
   * useless space in the Poincare pool. */
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_calculationHistory[i].resetMemoization();
  }

  m_selectableTableView.reloadData();
  /* TODO
   * Replace the following by selectCellAtLocation in order to avoid laying out
   * the table view twice.
   */
  if (numberOfRows() > 0) {
    m_selectableTableView.scrollToBottom();
    // Force to reload last added cell (hide the burger and exact output if necessary)
    tableViewDidChangeSelectionAndDidScroll(&m_selectableTableView, 0, -1);
  }
}

void HistoryController::viewWillAppear() {
  ViewController::viewWillAppear();
  reload();
}

void HistoryController::didBecomeFirstResponder() {
  selectCellAtLocation(0, numberOfRows()-1);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void HistoryController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == nullptr) {
    return;
  }
  if (nextFirstResponder == parentResponder()) {
    m_selectableTableView.deselectTable();
  }
}

static bool isIntegerInput(Expression e) {
  return (e.type() == ExpressionNode::Type::BasedInteger || (e.type() == ExpressionNode::Type::Opposite && isIntegerInput(e.childAtIndex(0))));
}

static bool isFractionInput(Expression e) {
  if (e.type() == ExpressionNode::Type::Opposite) {
    return isFractionInput(e.childAtIndex(0));
  }
  if (e.type() != ExpressionNode::Type::Division) {
    return false;
  }
  Expression num = e.childAtIndex(0);
  Expression den = e.childAtIndex(1);
  return isIntegerInput(num) && isIntegerInput(den);
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
    EditExpressionController * editController = (EditExpressionController *)parentResponder();
    if (subviewType == SubviewType::Input) {
      m_selectableTableView.deselectTable();
      editController->insertTextBody(calculationAtIndex(focusRow)->inputText());
    } else if (subviewType == SubviewType::Output) {
      m_selectableTableView.deselectTable();
      Shared::ExpiringPointer<Calculation> calculation = calculationAtIndex(focusRow);
      ScrollableTwoExpressionsView::SubviewPosition outputSubviewPosition = selectedCell->outputView()->selectedSubviewPosition();
      if (outputSubviewPosition == ScrollableTwoExpressionsView::SubviewPosition::Right
          && !PoincareHelpers::ShouldOnlyDisplayExactOutput(calculation->input()))
      {
        editController->insertTextBody(calculation->approximateOutputText(Calculation::NumberOfSignificantDigits::Maximal));
      } else {
        assert(calculation->displayOutput(nullptr) != Calculation::DisplayOutput::ApproximateOnly);
        editController->insertTextBody(calculation->exactOutputText());
      }
    } else {
      assert(subviewType == SubviewType::Ellipsis);
      UserCircuitBreakerCheckpoint checkpoint;
      if (CircuitBreakerRun(checkpoint)) {
        Calculation::AdditionalInformationType additionalInfoType = selectedCell->additionalInformationType();
        ListController * vc = nullptr;
        ExpiringPointer<Calculation> focusCalculation = calculationAtIndex(focusRow);
        Expression e = focusCalculation->exactOutput();
        if (additionalInfoType == Calculation::AdditionalInformationType::Complex) {
          vc = &m_complexController;
        } else if (additionalInfoType == Calculation::AdditionalInformationType::Trigonometry) {
          vc = &m_trigonometryController;
          // Find which of the input or output is the cosine/sine
          ExpressionNode::Type t = e.type();
          e = t == ExpressionNode::Type::Cosine || t == ExpressionNode::Type::Sine ? e : calculationAtIndex(focusRow)->input();
        } else if (additionalInfoType == Calculation::AdditionalInformationType::Integer) {
          vc = &m_integerController;
        } else if (additionalInfoType == Calculation::AdditionalInformationType::Rational) {
          Expression focusInput = focusCalculation->input();
          if (isFractionInput(focusInput)) {
            e = focusInput;
          }
          vc = &m_rationalController;
        } else if (additionalInfoType == Calculation::AdditionalInformationType::Unit) {
          vc = &m_unitController;
        } else if (additionalInfoType == Calculation::AdditionalInformationType::Vector) {
          vc = &m_vectorController;
        } else if (additionalInfoType == Calculation::AdditionalInformationType::Matrix) {
          vc = &m_matrixController;
        }
        if (vc) {
          assert(!e.isUninitialized());
          vc->setExactAndApproximateExpression(e, calculationAtIndex(focusRow)->approximateOutput(Calculation::NumberOfSignificantDigits::Maximal));
          Container::activeApp()->displayModalViewController(vc, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
        }
      }
    }
    return true;
  }
  if (event == Ion::Events::Backspace) {
    int focusRow = selectedRow();
    SubviewType subviewType = selectedSubviewType();
    m_selectableTableView.deselectTable();
    m_calculationStore->deleteCalculationAtIndex(storeIndex(focusRow));
    reload();
    if (numberOfRows()== 0) {
      Container::activeApp()->setFirstResponder(parentResponder());
      return true;
    }
    m_selectableTableView.selectCellAtLocation(0, focusRow > 0 ? focusRow - 1 : 0);
    /* The parameters 'sameCell' and 'previousSelectedY' are chosen to enforce
     * toggling of the output when necessary. */
    setSelectedSubviewType(subviewType, false, 0, (subviewType == SubviewType::Input) ? selectedRow() : -1);
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
    m_selectableTableView.deselectTable();
    Container::activeApp()->setFirstResponder(parentResponder());
    return true;
  }
  return false;
}

Shared::ExpiringPointer<Calculation> HistoryController::calculationAtIndex(int i) {
  return m_calculationStore->calculationAtIndex(storeIndex(i));
}

void HistoryController::tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection || previousSelectedCellY == selectedRow()) {
    return;
  }
  if (previousSelectedCellY == -1) {
    setSelectedSubviewType(SubviewType::Output, false, previousSelectedCellX, previousSelectedCellY);
  } else if (selectedRow() == -1) {
    setSelectedSubviewType(SubviewType::Input, false, previousSelectedCellX, previousSelectedCellY);
  } else {
    HistoryViewCell * selectedCell = (HistoryViewCell *)(t->selectedCell());
    SubviewType nextSelectedSubviewType = selectedSubviewType();
    if (selectedCell && !selectedCell->displaysSingleLine()) {
      nextSelectedSubviewType = previousSelectedCellY < selectedRow() ? SubviewType::Input : SubviewType::Output;
    }
    setSelectedSubviewType(nextSelectedSubviewType, false, previousSelectedCellX, previousSelectedCellY);
  }
  // The selectedCell may change during setSelectedSubviewType
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
  HistoryViewCell * myCell = static_cast<HistoryViewCell *>(cell);
  Poincare::Context * context = App::app()->localContext();
  myCell->setCalculation(calculationAtIndex(index).pointer(), index == selectedRow() && selectedSubviewType() == SubviewType::Output, context);
  myCell->setEven(index%2 == 0);
  myCell->reloadSubviewHighlight();
}

KDCoordinate HistoryController::nonMemoizedRowHeight(int j) {
  if (j >= m_calculationStore->numberOfCalculations()) {
    return 0;
  }
  Shared::ExpiringPointer<Calculation> calculation = calculationAtIndex(j);
  bool expanded = j == selectedRow() && selectedSubviewType() == SubviewType::Output;
  return calculation->height(expanded);
}

bool HistoryController::calculationAtIndexToggles(int index) {
  Context * context = App::app()->localContext();
  return index >= 0 && index < m_calculationStore->numberOfCalculations() && calculationAtIndex(index)->displayOutput(context) == Calculation::DisplayOutput::ExactAndApproximateToggle;
}


void HistoryController::setSelectedSubviewType(SubviewType subviewType, bool sameCell, int previousSelectedX, int previousSelectedY) {
  // Avoid selecting non-displayed ellipsis
  HistoryViewCell * selectedCell = static_cast<HistoryViewCell *>(m_selectableTableView.selectedCell());
  if (subviewType == SubviewType::Ellipsis && selectedCell && selectedCell->additionalInformationType() == Calculation::AdditionalInformationType::None) {
    subviewType = SubviewType::Output;
  }
  HistoryViewCellDataSource::setSelectedSubviewType(subviewType, sameCell, previousSelectedX, previousSelectedY);
}

void HistoryController::historyViewCellDidChangeSelection(HistoryViewCell ** cell, HistoryViewCell ** previousCell, int previousSelectedCellX, int previousSelectedCellY, SubviewType type, SubviewType previousType) {
  /* If the selection change triggers the toggling of the outputs, we update
   * the whole table as the height of the selected cell row might have changed. */
  if ((type == SubviewType::Output || previousType == SubviewType::Output) && (calculationAtIndexToggles(selectedRow()) || calculationAtIndexToggles(previousSelectedCellY))) {
    m_selectableTableView.reloadData();
  }

  // It might be necessary to scroll to the sub type if the cell overflows the screen
  if (selectedRow() >= 0) {
    m_selectableTableView.scrollToSubviewOfTypeOfCellAtLocation(type, m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  // Fill the selected cell and the previous selected cell because cells repartition might have changed
  *cell = static_cast<HistoryViewCell *>(m_selectableTableView.selectedCell());
  *previousCell = static_cast<HistoryViewCell *>(m_selectableTableView.cellAtLocation(previousSelectedCellX, previousSelectedCellY));
  /* 'reloadData' calls 'willDisplayCellForIndex' for each cell while the table
   * has been deselected. To reload the expanded cell, we call one more time
   * 'willDisplayCellForIndex' but once the right cell has been selected. */
  if (*cell) {
    willDisplayCellForIndex(*cell, selectedRow());
  }
}

void HistoryController::recomputeHistoryCellHeightsIfNeeded() {
  if (m_calculationStore->preferencesMightHaveChanged(Poincare::Preferences::sharedPreferences())) {
    m_calculationStore->recomputeHeights(HistoryViewCell::Height);
  };
}

}
