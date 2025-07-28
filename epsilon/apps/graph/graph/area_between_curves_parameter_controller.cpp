#include "area_between_curves_parameter_controller.h"

#include "../app.h"
#include "graph_controller.h"

using namespace Escher;
using namespace Shared;

namespace Graph {

Ion::Storage::Record
AreaBetweenCurvesParameterController::AreaCompatibleFunctionAtIndex(
    int index, Ion::Storage::Record excludedRecord) {
  ContinuousFunctionStore* store = App::app()->functionStore();
  assert(index < store->numberOfAreaCompatibleFunctions());
  int maxNumberOfFonctions = store->numberOfActiveFunctions();
  int numberOfDerivableActiveFunctionsFound = 0;
  for (int i = 0; i < maxNumberOfFonctions; i++) {
    Ion::Storage::Record currentRecord = store->activeRecordAtIndex(i);
    if (currentRecord == excludedRecord) {
      continue;
    }
    ExpiringPointer<ContinuousFunction> function =
        store->modelForRecord(currentRecord);
    if (ContinuousFunctionStore::IsFunctionAreaCompatible(function.operator->(),
                                                          nullptr)) {
      if (index == numberOfDerivableActiveFunctionsFound) {
        return currentRecord;
      }
      numberOfDerivableActiveFunctionsFound++;
    }
  }
  assert(false);
  return nullptr;
}

AreaBetweenCurvesParameterController::AreaBetweenCurvesParameterController(
    Responder* parentResponder,
    AreaBetweenCurvesGraphController* areaGraphController)
    : ListWithTopAndBottomController(parentResponder, &m_topView),
      m_mainRecord(nullptr),
      m_areaGraphController(areaGraphController),
      m_topView(I18n::Message::SelectSecondCurve, k_messageFormat) {}

const char* AreaBetweenCurvesParameterController::title() const {
  return I18n::translate(I18n::Message::AreaBetweenCurves);
}

int AreaBetweenCurvesParameterController::numberOfRows() const {
  return App::app()->functionStore()->numberOfAreaCompatibleFunctions() - 1;
}

KDCoordinate AreaBetweenCurvesParameterController::nonMemoizedRowHeight(
    int row) {
  ExpiringPointer<ContinuousFunction> function =
      App::app()->functionStore()->modelForRecord(
          AreaCompatibleFunctionAtIndex(row, m_mainRecord));
  CurveSelectionCell tempCell;
  tempCell.label()->setLayout(function->layout());
  return tempCell.labelView()->minimalSizeForOptimalDisplay().height() +
         Metric::CellMargins.height() + Metric::CellSeparatorThickness;
}

void AreaBetweenCurvesParameterController::fillCellForRow(
    Escher::HighlightCell* cell, int row) {
  ExpiringPointer<ContinuousFunction> function =
      App::app()->functionStore()->modelForRecord(
          AreaCompatibleFunctionAtIndex(row, m_mainRecord));
  CurveSelectionCell* curveSelectionCell =
      static_cast<CurveSelectionCell*>(cell);
  curveSelectionCell->setColor(function->color());
  curveSelectionCell->label()->setLayout(function->layout().clone());
}

bool AreaBetweenCurvesParameterController::handleEvent(
    Ion::Events::Event event) {
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  if (event == Ion::Events::Left) {
    stack->pop();
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    Ion::Storage::Record secondRecord =
        AreaCompatibleFunctionAtIndex(innerSelectedRow(), m_mainRecord);
    assert(
        App::app()->functionStore()->modelForRecord(m_mainRecord)->isActive() &&
        App::app()->functionStore()->modelForRecord(secondRecord)->isActive());
    m_areaGraphController->setRecord(m_mainRecord);
    m_areaGraphController->setSecondRecord(secondRecord);
    stack->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        false);
    stack->push(m_areaGraphController);
    return true;
  }
  return false;
}

}  // namespace Graph
