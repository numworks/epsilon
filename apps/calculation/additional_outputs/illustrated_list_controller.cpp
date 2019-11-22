#include "illustrated_list_controller.h"
#include "../app.h"

using namespace Poincare;

namespace Calculation {

/* List Controller */

IllustratedListController::ListController::ListController(IllustratedListController * dataSource) :
  ViewController(dataSource),
  m_selectableTableView(this, dataSource, dataSource, dataSource)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void IllustratedListController::ListController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  m_selectableTableView.selectCellAtLocation(0, 1);
}

/* Illustrated list controller */

IllustratedListController::IllustratedListController(Responder * parentResponder) :
  StackViewController(parentResponder, &m_listController, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_listController(this),
  m_additionalCalculationCells{}
{
  for (int i = 0; i < k_maxNumberOfAdditionalCalculations; i++) {
    m_additionalCalculationCells[i].setParentResponder(m_listController.selectableTableView());
    m_additionalCalculationCells[i].setDisplayLeft(true);
  }
}

bool IllustratedListController::handleEvent(Ion::Events::Event event) {
  return false;
}

void IllustratedListController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_listController);
}

int IllustratedListController::numberOfRows() const {
  return k_maxNumberOfAdditionalCalculations + 1;
}

int IllustratedListController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == 0) {
    return 1;
  }
  return k_maxNumberOfAdditionalCalculations;
}

HighlightCell * IllustratedListController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == 0) {
    return illustrationCell();
  }
  return &m_additionalCalculationCells[index];
}

KDCoordinate IllustratedListController::rowHeight(int j) {
  if (j == 0) {
    return k_illustrationHeight;
  }
  int calculationIndex = j-1;
  if (calculationIndex >= m_calculationStore.numberOfCalculations()) {
    return 0;
  }
  Shared::ExpiringPointer<Calculation> calculation = m_calculationStore.calculationAtIndex(calculationIndex);
  return calculation->height(App::app()->localContext(), j == selectedRow(), true) + 2 * Metric::CommonSmallMargin + Metric::CellSeparatorThickness;
}

int IllustratedListController::typeAtLocation(int i, int j) {
  return j == 0 ? 0 : 1;
}

void IllustratedListController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == 0) {
    // TODO ?
    return;
  }
  Poincare::Context * context = App::app()->localContext();
  ScrollableInputExactApproximateExpressionsCell * myCell = (ScrollableInputExactApproximateExpressionsCell *)cell;
  Calculation * c = m_calculationStore.calculationAtIndex(index-1).pointer();
  myCell->setCalculation(c);
  myCell->setDisplayCenter(c->displayOutput(context) != Calculation::DisplayOutput::ApproximateOnly);
  //myCell->setHighlighted(myCell->isHighlighted()); //TODO??
}

void IllustratedListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) {
  if (withinTemporarySelection) {
    return;
  }
  // Forbid selecting Illustration cell
  if (t->selectedRow() == 0) {
    t->selectCellAtLocation(0, 1);
  }
  /* But scroll to the top when we select the first
   * ScrollableInputExactApproximateExpressionsCell in order display the
   * illustration cell. */
  if (t->selectedRow() == 1) {
    t->scrollToCell(0, 0);
  }
}

void IllustratedListController::fillCalculationStoreFromExpression(Expression e) {
  // TODO
  Poincare::Context * context = App::app()->localContext();
  m_calculationStore.push("1+root(2,3)", context);
  m_calculationStore.push("2+root(2,3)", context);
  m_calculationStore.push("3+root(2,3)", context);
}

}
