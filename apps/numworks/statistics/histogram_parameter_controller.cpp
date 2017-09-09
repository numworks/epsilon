#include "histogram_parameter_controller.h"
#include "app.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Statistics {

HistogramParameterController::HistogramParameterController(Responder * parentResponder, Store * store) :
  FloatParameterController(parentResponder),
  m_cells{},
  m_store(store)
{
}

const char * HistogramParameterController::title() {
  return I18n::translate(&I18n::Common::HistogramSet);
}

int HistogramParameterController::numberOfRows() {
  return 1+k_numberOfCells;
}

void HistogramParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  const I18n::Message *labels[k_numberOfCells] = {&I18n::Common::RectangleWidth, &I18n::Common::BarStart};
  myCell->setMessage(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

double HistogramParameterController::parameterAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfCells);
  if (index == 0) {
    return m_store->barWidth();
  }
  return m_store->firstDrawnBarAbscissa();
}

bool HistogramParameterController::setParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex >= 0 && parameterIndex < k_numberOfCells);
  if (parameterIndex == 0) {
    double newNumberOfBars = std::ceil((m_store->maxValue() - m_store->minValue())/f);
    if (f <= 0.0f || newNumberOfBars > Store::k_maxNumberOfBars || m_store->firstDrawnBarAbscissa() > m_store->maxValue()+f) {
      app()->displayWarning(&I18n::Common::ForbiddenValue);
      return false;
    }
    m_store->setBarWidth(f);
  } else {
    if (f > m_store->maxValue()+m_store->barWidth()) {
      app()->displayWarning(&I18n::Common::ForbiddenValue);
      return false;
    }
    m_store->setFirstDrawnBarAbscissa(f);
  }
  return true;
}

HighlightCell * HistogramParameterController::reusableParameterCell(int index, int type) {
  assert(index >= 0 && index < k_numberOfCells);
  return m_cells[index];
}

int HistogramParameterController::reusableParameterCellCount(int type) {
  return k_numberOfCells;
}

View * HistogramParameterController::loadView() {
  SelectableTableView * tableView = (SelectableTableView *)FloatParameterController::loadView();
  for (int i = 0; i < k_numberOfCells; i++) {
    m_cells[i] = new MessageTableCellWithEditableText(tableView, this, m_draftTextBuffer, &I18n::Common::Default);
  }
  return tableView;
}

void HistogramParameterController::unloadView(View * view) {
  for (int i = 0; i < k_numberOfCells; i++) {
    delete m_cells[i];
    m_cells[i] = nullptr;
  }
  FloatParameterController::unloadView(view);
}

}

