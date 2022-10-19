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
  m_intervalParameterController(this, inputEventHandlerDelegate),
  m_setIntervalButton(this, I18n::Message::IntervalSet, Invocation([](void * context, void * sender) {
    ValuesController * valuesController = (ValuesController *) context;
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
  setupSelectableTableViewAndCells(inputEventHandlerDelegate);
  setDefaultStartEndMessages();
  initValueCells();
}

int ValuesController::fillColumnName(int columnIndex, char * buffer) {
  /* The column names U_n, V_n, etc. are implemented as layout for now (see setTitleCellText of this file)
   * Since there is no column parameters for these column, the fillColumnName is not yet implemented.
   */
  assert(typeAtLocation(columnIndex, 0) != k_functionTitleCellType);
  return Shared::ValuesController::fillColumnName(columnIndex, buffer);
}


void ValuesController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  if (typeAtLocation(columnIndex,0) == k_functionTitleCellType) {
    Shared::ExpressionFunctionTitleCell * myCell = static_cast<Shared::ExpressionFunctionTitleCell *>(cell);
    Shared::Sequence * sequence = functionStore()->modelForRecord(recordAtColumn(columnIndex));
    myCell->setLayout(sequence->nameLayout());
    return;
  }
  Shared::ValuesController::setTitleCellText(cell, columnIndex);
}

// AlternateEmptyViewDelegate
I18n::Message ValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

// ValuesController
void ValuesController::setDefaultStartEndMessages() {
  m_intervalParameterController.setStartEndMessages(I18n::Message::NStart, I18n::Message::NEnd);
}

I18n::Message ValuesController::valuesParameterMessageAtColumn(int columnIndex) const {
  return I18n::Message::N;
}

// EditableCellViewController

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  assert(checkDataAtLocation(floatBody, columnIndex, rowIndex));
  return Shared::ValuesController::setDataAtLocation(std::round(floatBody), columnIndex, rowIndex);
}

// Model getters

Shared::Interval * ValuesController::intervalAtColumn(int columnIndex) {
  int index = GlobalPreferences::sharedGlobalPreferences()->sequencesInitialRank();
  if (App::app()->interval()->parameters()->start() != index && !App::app()->snapshot()->intervalModifiedByUser() && App::app()->functionStore()->smallestInitialRank() >= index) {
    App::app()->interval()->parameters()->setStart(index);
    App::app()->interval()->forceRecompute();
  }
  return App::app()->interval();
}

// Function evaluation memoization

void ValuesController::createMemoizedLayout(int column, int row, int index) {
  double abscissa = intervalAtColumn(column)->element(row-1); // Subtract the title row from row to get the element index
  Shared::ExpiringPointer<Shared::Sequence> sequence = functionStore()->modelForRecord(recordAtColumn(column));
  Context * context = textFieldDelegateApp()->localContext();
  Coordinate2D<double> xy = sequence->evaluateXYAtParameter(abscissa, context);
  Float<double> e = Float<double>::Builder(xy.x2());
  *memoizedLayoutAtIndex(index) = e.createLayout(Preferences::PrintFloatMode::Decimal, Preferences::VeryLargeNumberOfSignificantDigits, context);
}

}
