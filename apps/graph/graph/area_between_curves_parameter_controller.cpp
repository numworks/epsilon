#include "area_between_curves_parameter_controller.h"
#include "graph_controller.h"
#include "../app.h"

using namespace Escher;
using namespace Shared;

namespace Graph {

AreaBetweenCurvesParameterController::AreaBetweenCurvesParameterController(Responder * parentResponder, AreaBetweenCurvesGraphController * areaGraphController) :
  SelectableListViewController(parentResponder),
  m_mainRecord(nullptr),
  m_areaGraphController(areaGraphController)
{}

const char * AreaBetweenCurvesParameterController::title() {
  return I18n::translate(I18n::Message::AreaBetweenCurves);
}

int AreaBetweenCurvesParameterController::numberOfRows() const {
  return App::app()->functionStore()->numberOfActiveDerivableFunctions() - 1;
}

KDCoordinate AreaBetweenCurvesParameterController::nonMemoizedRowHeight(int j) {
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(recordAtIndex(j));
  return function->layout().layoutSize(KDFont::Size::Large).height() + Metric::CellTopMargin + Metric::CellBottomMargin + Metric::CellSeparatorThickness;
}

void AreaBetweenCurvesParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  resetMemoization();
  m_selectableTableView.reloadData();
}

void AreaBetweenCurvesParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void AreaBetweenCurvesParameterController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(recordAtIndex(index));
  static_cast<CurveSelectionCell *>(cell)->setColor(function->color());
  static_cast<CurveSelectionCell *>(cell)->setLayout(function->layout().clone());
}

bool AreaBetweenCurvesParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    m_areaGraphController->setRecord(m_mainRecord);
    m_areaGraphController->setSecondRecord(recordAtIndex(selectedRow()));
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->popUntilDepth(Shared::InteractiveCurveViewController::k_graphControllerStackDepth, true);
    stack->push(m_areaGraphController);
    return true;
  }
  return false;
}

Ion::Storage::Record AreaBetweenCurvesParameterController::recordAtIndex(int index) {
  ContinuousFunctionStore * store = App::app()->functionStore();
  assert(index < store->numberOfActiveDerivableFunctions());
  int maxNumberOfFonctions = store->numberOfActiveFunctions();
  int numberOfDerivableActiveFunctionsFound = 0;
  for (int i = 0; i < maxNumberOfFonctions; i++) {
    Ion::Storage::Record currentRecord = store->activeRecordAtIndex(i);
    if (currentRecord == m_mainRecord) {
      continue;
    }
    ExpiringPointer<ContinuousFunction> function = store->modelForRecord(currentRecord);
    if (ContinuousFunctionStore::isFunctionActiveAndDerivable(function.operator->(), nullptr)) {
      if (index == numberOfDerivableActiveFunctionsFound) {
        return currentRecord;
      }
      numberOfDerivableActiveFunctionsFound++;
    }
  }
  assert(false);
  return nullptr;
}


}
