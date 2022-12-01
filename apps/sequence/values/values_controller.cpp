#include "values_controller.h"
#include <assert.h>
#include <cmath>
#include "../../shared/poincare_helpers.h"
#include "../app.h"
#include <apps/i18n.h>

using namespace Poincare;
using namespace Escher;

namespace Sequence {

ValuesController::ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header),
  m_prefacedTwiceTableView(0, 0, this, &m_selectableTableView, this),
  m_selectableTableView(this, this, this, this, &m_prefacedTwiceTableView),
  m_intervalParameterController(this, inputEventHandlerDelegate),
  m_setIntervalButton(this, I18n::Message::IntervalSet, Invocation::Builder<ValuesController>([](ValuesController * valuesController, void * sender) {
    StackViewController * stack = ((StackViewController *)valuesController->stackController());
    IntervalParameterController * controller = valuesController->intervalParameterController();
    controller->setInterval(valuesController->intervalAtColumn(valuesController->selectedColumn()));
    App::app()->snapshot()->setIntervalModifiedByUser(true);
    /* No need to change Nstart/Nend messages because they are the only messages
     * used and we set them in ValuesController::ValuesController(...) */
    stack->push(controller);
    return true;
  }, this), k_cellFont)
{
  m_prefacedTwiceTableView.setBackgroundColor(Palette::WallScreenDark);
  m_prefacedTwiceTableView.setCellOverlap(0, 0);
  m_prefacedTwiceTableView.setMargins(k_margin, k_scrollBarMargin, k_scrollBarMargin, k_margin);
  setupSelectableTableViewAndCells(inputEventHandlerDelegate);
  setDefaultStartEndMessages();
  initValueCells();
}

// ColumnHelper

int ValuesController::fillColumnName(int columnIndex, char * buffer) {
  /* The column names U_n, V_n, etc. are implemented as layout for now (see setTitleCellText of this file)
   * Since there is no column parameters for these column, the fillColumnName is not yet implemented. */
  assert(typeAtLocation(columnIndex, 0) != k_functionTitleCellType);
  return Shared::ValuesController::fillColumnName(columnIndex, buffer);
}

// EditableCellTableViewController

void ValuesController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  if (typeAtLocation(columnIndex,0) == k_functionTitleCellType) {
    Shared::ExpressionFunctionTitleCell * myCell = static_cast<Shared::ExpressionFunctionTitleCell *>(cell);
    Shared::Sequence * sequence = functionStore()->modelForRecord(recordAtColumn(columnIndex));
    myCell->setLayout(sequence->nameLayout());
    return;
  }
  Shared::ValuesController::setTitleCellText(cell, columnIndex);
}

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  assert(checkDataAtLocation(floatBody, columnIndex, rowIndex));
  return Shared::ValuesController::setDataAtLocation(std::round(floatBody), columnIndex, rowIndex);
}

// Shared::ValuesController

Poincare::Layout * ValuesController::memoizedLayoutAtIndex(int i) {
  assert(i >= 0 && i < k_maxNumberOfDisplayableCells);
  return &m_memoizedLayouts[i];
}

void ValuesController::createMemoizedLayout(int column, int row, int index) {
  double abscissa = intervalAtColumn(column)->element(row-1); // Subtract the title row from row to get the element index
  Shared::ExpiringPointer<Shared::Sequence> sequence = functionStore()->modelForRecord(recordAtColumn(column));
  Context * context = textFieldDelegateApp()->localContext();
  Coordinate2D<double> xy = sequence->evaluateXYAtParameter(abscissa, context);
  Float<double> e = Float<double>::Builder(xy.x2());
  *memoizedLayoutAtIndex(index) = e.createLayout(Preferences::PrintFloatMode::Decimal, Preferences::VeryLargeNumberOfSignificantDigits, context);
}

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  return App::app()->interval();
}

Shared::ExpressionFunctionTitleCell * ValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableSequences);
  return &m_sequenceTitleCells[j];
}

Escher::EvenOddExpressionCell * ValuesController::valueCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfDisplayableCells);
  return &m_valueCells[j];
}

Escher::EvenOddEditableTextCell * ValuesController::abscissaCells(int j) {
  assert (j >= 0 && j < k_maxNumberOfDisplayableRows);
  return &m_abscissaCells[j];
}

Escher::EvenOddMessageTextCell * ValuesController::abscissaTitleCells(int j) {
  assert (j >= 0 && j < abscissaTitleCellsCount());
  return &m_abscissaTitleCell;
}

void ValuesController::setDefaultStartEndMessages() {
  m_intervalParameterController.setStartEndMessages(I18n::Message::NStart, I18n::Message::NEnd);
}

}
