#include "calculation_parameter_controller.h"

#include <assert.h>

#include <cmath>

#include "../app.h"
#include "area_between_curves_parameter_controller.h"
#include "graph_controller.h"

using namespace Shared;
using namespace Escher;

namespace Graph {

CalculationParameterController::CalculationParameterController(
    Responder *parentResponder, GraphView *graphView, BannerView *bannerView,
    InteractiveCurveViewRange *range, CurveViewCursor *cursor)
    : ExplicitSelectableListViewController(parentResponder),
      m_preimageParameterController(nullptr, range, cursor,
                                    &m_preimageGraphController),
      m_preimageGraphController(nullptr, graphView, bannerView, range, cursor),
      m_tangentGraphController(nullptr, graphView, bannerView, range, cursor),
      m_normalGraphController(nullptr, graphView, bannerView, range, cursor),
      m_integralGraphController(nullptr, graphView, range, cursor),
      m_areaParameterController(nullptr, &m_areaGraphController),
      m_areaGraphController(nullptr, graphView, range, cursor),
      m_minimumGraphController(nullptr, graphView, bannerView, range, cursor),
      m_maximumGraphController(nullptr, graphView, bannerView, range, cursor),
      m_rootGraphController(nullptr, graphView, bannerView, range, cursor),
      m_intersectionGraphController(nullptr, graphView, bannerView, range,
                                    cursor) {
  m_intersectionCell.label()->setMessage(I18n::Message::Intersection);
  m_minimumCell.label()->setMessage(I18n::Message::Minimum);
  m_maximumCell.label()->setMessage(I18n::Message::Maximum);
  m_integralCell.label()->setMessage(I18n::Message::Integral);
  m_tangentCell.label()->setMessage(I18n::Message::Tangent);
  m_normalCell.label()->setMessage(I18n::Message::NormalLine);
  m_rootCell.label()->setMessage(I18n::Message::Zeros);
  m_preimageCell.label()->setMessage(I18n::Message::Preimage);
}

HighlightCell *CalculationParameterController::cell(int index) {
  HighlightCell *cells[k_numberOfRows] = {
      &m_preimageCell, &m_intersectionCell, &m_maximumCell,  &m_minimumCell,
      &m_rootCell,     &m_tangentCell, &m_normalCell,      &m_integralCell,
      &m_areaCell
    };
  return cells[index];
}

const char *CalculationParameterController::title() {
  return I18n::translate(I18n::Message::Find);
}

void CalculationParameterController::viewWillAppear() {
  bool intersectionWasVisible = m_intersectionCell.isVisible();
  bool areaWasVisible = m_areaCell.isVisible();
  m_intersectionCell.setVisible(ShouldDisplayIntersection());
  m_areaCell.setVisible(ShouldDisplayAreaBetweenCurves());
  bool resetSizeMemoization =
      intersectionWasVisible != m_intersectionCell.isVisible() ||
      areaWasVisible != m_areaCell.isVisible();
  if (m_areaCell.isVisible()) {
    fillAreaCell();
  }
  ViewController::viewWillAppear();
  m_selectableListView.reloadData(true, resetSizeMemoization);
}

template <class T>
void CalculationParameterController::push(T *controller, bool pop) {
  StackViewController *stack =
      static_cast<StackViewController *>(parentResponder());
  assert(App::app()->functionStore()->modelForRecord(m_record)->isActive());
  controller->setRecord(m_record);
  if (pop) {
    stack->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        false);
  }
  stack->push(controller);
}

bool CalculationParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    StackViewController *stack =
        static_cast<StackViewController *>(parentResponder());
    stack->pop();
    return true;
  }
  AbstractMenuCell *cell = static_cast<AbstractMenuCell *>(selectedCell());
  if (!cell->canBeActivatedByEvent(event)) {
    return false;
  }
  if (cell == &m_preimageCell) {
    push(&m_preimageParameterController, false);
  } else if (cell == &m_tangentCell) {
    push(&m_tangentGraphController, true);
  } else if (cell == &m_normalCell) {
    push(&m_normalGraphController, true);
  } else if (cell == &m_integralCell) {
    push(&m_integralGraphController, true);
  } else if (cell == &m_minimumCell) {
    push(&m_minimumGraphController, true);
  } else if (cell == &m_maximumCell) {
    push(&m_maximumGraphController, true);
  } else if (cell == &m_intersectionCell) {
    push(&m_intersectionGraphController, true);
  } else if (cell == &m_rootCell) {
    push(&m_rootGraphController, true);
  } else {
    assert(cell == &m_areaCell);
    if (!ShouldDisplayChevronInAreaCell()) {
      Ion::Storage::Record secondRecord =
          AreaBetweenCurvesParameterController::DerivableActiveFunctionAtIndex(
              0, m_record);
      m_areaGraphController.setSecondRecord(secondRecord);
      push(&m_areaGraphController, true);
    } else {
      push(&m_areaParameterController, false);
    }
  }
  return true;
}

void CalculationParameterController::fillAreaCell() {
  assert(ShouldDisplayAreaBetweenCurves());
  // If there is only two derivable functions, hide the chevron
  m_areaCell.accessory()->displayChevron(ShouldDisplayChevronInAreaCell());
  // Get the name of the selected function
  ExpiringPointer<ContinuousFunction> mainFunction =
      App::app()->functionStore()->modelForRecord(m_record);
  constexpr static int bufferSize = Shared::Function::k_maxNameWithArgumentSize;
  char mainFunctionName[bufferSize];
  mainFunction->nameWithArgument(mainFunctionName, bufferSize);

  char secondPlaceHolder[bufferSize];
  if (!ShouldDisplayChevronInAreaCell()) {
    // If there are only 2 functions, display "Area between f(x) and g(x)"
    secondPlaceHolder[0] = ' ';
    Ion::Storage::Record secondRecord =
        AreaBetweenCurvesParameterController::DerivableActiveFunctionAtIndex(
            0, m_record);
    ExpiringPointer<ContinuousFunction> secondFunction =
        App::app()->functionStore()->modelForRecord(secondRecord);
    secondFunction->nameWithArgument(secondPlaceHolder + 1, bufferSize);
    if (strcmp(mainFunctionName, secondPlaceHolder + 1) == 0) {
      // If both functions are name "y", display "Area between curves"
      m_areaCell.label()->setMessageWithPlaceholders(
          I18n::Message::AreaBetweenCurves);
      return;
    }
  } else {
    // If there are more than 2 functions, display "Area between f(x) and"
    secondPlaceHolder[0] = 0;
  }
  bool labelFitsInBuffer = m_areaCell.label()->unsafeSetMessageWithPlaceholders(
      I18n::Message::AreaBetweenCurvesWithFunctionName, mainFunctionName,
      secondPlaceHolder);
  if (!labelFitsInBuffer ||
      m_areaCell.labelView()->minimalSizeForOptimalDisplay().width() >
          m_areaCell.innerWidth()) {
    // If there is not enough space in the cell, display "Area between curves"
    m_areaCell.label()->setMessageWithPlaceholders(
        I18n::Message::AreaBetweenCurves);
  }
}

void CalculationParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  selectRow(0);
  m_intersectionCell.setVisible(ShouldDisplayIntersection());
  m_areaCell.setVisible(ShouldDisplayAreaBetweenCurves());
  m_selectableListView.resetSizeAndOffsetMemoization();
}

bool CalculationParameterController::ShouldDisplayIntersection() {
  /* Intersection is handled between all active functions having one subcurve,
   * except Polar and Parametric. */
  ContinuousFunctionStore *store = App::app()->functionStore();
  /* Intersection row is displayed if there is at least two intersectable
   * functions. */
  return store->numberOfIntersectableFunctions() > 1;
}

bool CalculationParameterController::ShouldDisplayAreaBetweenCurves() {
  ContinuousFunctionStore *store = App::app()->functionStore();
  /* Area between curves is displayed if there is at least two derivable
   * functions. */
  return store->numberOfActiveDerivableFunctions() > 1;
}

bool CalculationParameterController::ShouldDisplayChevronInAreaCell() {
  /* Area between curves row does not always have a chevron. */
  return App::app()->functionStore()->numberOfActiveDerivableFunctions() > 2;
}

}  // namespace Graph
