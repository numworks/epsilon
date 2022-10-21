#include "graph_controller.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>
#include <poincare/layout_helper.h>
#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Graph {

GraphController::GraphController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * rangeVersion, ButtonRowController * header) :
  FunctionGraphController(parentResponder, inputEventHandlerDelegate, header, curveViewRange, &m_view, cursor, indexFunctionSelectedByCursor, rangeVersion),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(curveViewRange, m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(curveViewRange),
  m_curveParameterController(inputEventHandlerDelegate, curveViewRange, &m_bannerView, m_cursor, &m_view, this),
  m_functionSelectionController(this)
{
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

void GraphController::viewWillAppear() {
  m_view.drawTangent(false);
  m_cursorView.resetMemoization();
  m_view.setInterest(Solver<double>::Interest::None);
  m_view.setCursorView(&m_cursorView);
  FunctionGraphController::viewWillAppear();
  selectFunctionWithCursor(indexFunctionSelectedByCursor(), true);
}

void GraphController::didBecomeFirstResponder() {
  FunctionGraphController::didBecomeFirstResponder();
  m_view.selectRecord(functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor()));
  refreshPointsOfInterest();
}

Range2D GraphController::optimalRange(bool computeX, bool computeY, Range2D originalRange) const {
  Zoom::Function2DWithContext evaluator = [](float t, const void * model, Context * context) {
    const ContinuousFunction * f = static_cast<const ContinuousFunction *>(model);
    return f->evaluateXYAtParameter(t, context);
  };
  Zoom::Function2DWithContext evaluatorSecondCurve = [](float t, const void * model, Context * context) {
    const ContinuousFunction * f = static_cast<const ContinuousFunction *>(model);
    return f->evaluateXYAtParameter(t, context, 1);
  };

  Context * context = App::app()->localContext();
  Zoom zoom(NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(), context);
  ContinuousFunctionStore * store = functionStore();
  int nbFunctions = store->numberOfActiveFunctions();
  constexpr float k_maxFloat = InteractiveCurveViewRange::k_maxFloat;
  Range1D xBounds = computeX ? Range1D(-k_maxFloat, k_maxFloat) : originalRange.x();

  for (int i = 0; i < nbFunctions; i++) {
    ExpiringPointer<ContinuousFunction> f = store->modelForRecord(store->activeRecordAtIndex(i));
    if (f->basedOnCostlyAlgorithms(context)) {
      continue;
    }
    if (f->plotType() == ContinuousFunction::PlotType::Polar || f->plotType() == ContinuousFunction::PlotType::Parametric) {
      assert(std::isfinite(f->tMin()) && std::isfinite(f->tMax()));
      zoom.setBounds(f->tMin(), f->tMax());
      zoom.fitFullFunction(evaluator, f.operator->());
    } else if (!f->isAlongY()) { // TODO Update Zoom to handle x=f(y) functions
      assert(f->isAlongXOrY());
      zoom.setBounds(xBounds.min(), xBounds.max());
      zoom.fitPointsOfInterest(evaluator, f.operator->());
      if (f->numberOfSubCurves() > 1) {
        assert(f->numberOfSubCurves() == 2);
        zoom.fitPointsOfInterest(evaluatorSecondCurve, f.operator->());
      }
      if (f->isIntersectable()) {
        ContinuousFunction * mainF = f.operator->();
        for (int j = 0; j < nbFunctions; j++) {
          ExpiringPointer<ContinuousFunction> g = store->modelForRecord(store->activeRecordAtIndex(j));
          if (i == j || !g->isIntersectable() || g->basedOnCostlyAlgorithms(context)) {
            continue;
          }
          zoom.fitIntersections(evaluator, mainF, evaluator, g.operator->());
        }
      }
    }
  }

  if (computeY) {
    zoom.setBounds(xBounds.min(), xBounds.max());
    for (int i = 0; i < nbFunctions; i++) {
      ExpiringPointer<ContinuousFunction> f = store->modelForRecord(store->activeRecordAtIndex(i));
      if (f->basedOnCostlyAlgorithms(context) || f->isAlongY() || !f->isAlongXOrY()) {
        continue;
      }
      zoom.fitMagnitude(evaluator, f.operator->());
      if (f->numberOfSubCurves() > 1) {
        zoom.fitMagnitude(evaluatorSecondCurve, f.operator->());
      }
    }
  }

  Range2D newRange = zoom.range(k_maxFloat, defaultRangeIsNormalized());
  return Range2D((computeX ? newRange : originalRange).x(), (computeY ? newRange : originalRange).y());
}

bool GraphController::handleZoom(Ion::Events::Event event) {
  bool res = FunctionGraphController::handleZoom(event);
  if (res) {
    refreshPointsOfInterest();
  }
  return res;
}

bool GraphController::defaultRangeIsNormalized() const {
  return functionStore()->displaysFunctionsToNormalize();
}

Layout GraphController::FunctionSelectionController::nameLayoutAtIndex(int j) const {
  GraphController * graphController = static_cast<GraphController *>(m_graphController);
  ContinuousFunctionStore * store = graphController->functionStore();
  ExpiringPointer<ContinuousFunction> function = store->modelForRecord(store->activeRecordAtIndex(j));
  constexpr size_t bufferSize = ContinuousFunction::k_maxNameWithArgumentSize;
  char buffer[bufferSize];
  int size = function->nameWithArgument(buffer, bufferSize);
  return LayoutHelper::String(buffer, size);
}

void GraphController::selectFunctionWithCursor(int functionIndex, bool willBeVisible) {
  FunctionGraphController::selectFunctionWithCursor(functionIndex, willBeVisible);
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(functionIndex);
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
  m_cursorView.setColor(f->color());
  m_view.selectRecord(record);

  // Compute points of interest
  if (willBeVisible) {
    m_pointsOfInterest.setRecord(record);
    refreshPointsOfInterest();
  }
}

bool GraphController::displayDerivativeInBanner() const {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  return  functionStore()->modelForRecord(record)->displayDerivative() &&
    functionStore()->modelForRecord(record)->canDisplayDerivative();
}

 void GraphController::reloadBannerView() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  bool displayDerivative = displayDerivativeInBanner();
  m_bannerView.setDisplayParameters(true, displayDerivative, false);
  FunctionGraphController::reloadBannerView();
  if (!displayDerivative) {
    return;
  }
  reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, record);
}

bool GraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  bool result = privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, record, m_view.pixelWidth(), scrollSpeed, &m_selectedSubCurveIndex);
  refreshPointsOfInterest();
  return result;
}

int GraphController::nextCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context, int currentSubCurveIndex, int * nextSubCurveIndex) const {
  assert(nextSubCurveIndex != nullptr);
  int nbOfActiveFunctions = 0;
  if (!functionStore()->displaysNonCartesianFunctions(&nbOfActiveFunctions)) {
    return FunctionGraphController::nextCurveIndexVertically(goingUp, currentSelectedCurve, context, currentSubCurveIndex, nextSubCurveIndex);
  }
  // Handle for sub curve in current function
  if (!goingUp) {
    ExpiringPointer<ContinuousFunction> currentF = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(currentSelectedCurve));
    if (currentF->numberOfSubCurves() > currentSubCurveIndex + 1) {
      // Switch to next sub curve
      *nextSubCurveIndex = currentSubCurveIndex + 1;
      return currentSelectedCurve;
    }
  } else if (goingUp && currentSubCurveIndex > 0) {
    // Switch to previous sub curve
    *nextSubCurveIndex = currentSubCurveIndex - 1;
    return currentSelectedCurve;
  }
  // Go to the next function
  int nextActiveFunctionIndex = currentSelectedCurve + (goingUp ? -1 : 1);
  if (nextActiveFunctionIndex >= nbOfActiveFunctions || nextActiveFunctionIndex < 0) {
    return -1;
  }
  if (goingUp) {
    // Select last sub curve in next function when going up
    ExpiringPointer<ContinuousFunction> nextF = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(nextActiveFunctionIndex));
    *nextSubCurveIndex = nextF->numberOfSubCurves() - 1;
  } else {
    // Select first sub curve in next function
    *nextSubCurveIndex = 0;
  }
  return nextActiveFunctionIndex;
}

double GraphController::defaultCursorT(Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
  if (function->properties().isCartesian()) {
    return FunctionGraphController::defaultCursorT(record);
  }
  return function->tMin();
}

void GraphController::jumpToLeftRightCurve(double t, int direction, int functionsCount, Ion::Storage::Record record) {
  if (functionsCount == 1) {
    return;
  }
  int nextCurveIndex = -1;
  double xDelta = DBL_MAX;
  double nextY = 0.0;
  double nextT = 0.0;
  int nextSubCurve = 0;
  for (int i = 0; i < functionsCount; i++) {
    Ion::Storage::Record currentRecord = functionStore()->activeRecordAtIndex(i);
    if (currentRecord == record) {
      continue;
    }
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(currentRecord);
    assert(f->properties().isCartesian());
    /* Select the closest horizontal curve, then the closest vertically, then
     * the lowest curve index. */
    double currentTMin = f->tMin();
    double currentTMax = f->tMax();
    assert(!std::isnan(currentTMin));
    assert(!std::isnan(currentTMax));
    if ((direction > 0 && currentTMax > t)
        ||(direction < 0 && currentTMin < t))
    {
      double currentXDelta = direction > 0 ?
        (t >= currentTMin ? 0.0 : currentTMin - t) :
        (t <= currentTMax ? 0.0 : t - currentTMax);
      assert(currentXDelta >= 0.0);
      if (currentXDelta <= xDelta) {
        double potentialNextTMin = f->tMin();
        double potentialNextTMax = f->tMax();
        double potentialNextT = std::max(potentialNextTMin, std::min(potentialNextTMax, t));
        // If a function has two curves
        for (int subCurveIndex = 0; subCurveIndex < f->numberOfSubCurves(); subCurveIndex++) {
          Coordinate2D<double> xy = f->evaluateXYAtParameter(potentialNextT, App::app()->localContext(), subCurveIndex);
          if (currentXDelta < xDelta || std::abs(xy.x2() - m_cursor->y()) < std::abs(nextY - m_cursor->y())) {
            nextCurveIndex = i;
            xDelta = currentXDelta;
            nextY = xy.x2();
            nextT = potentialNextT;
            nextSubCurve = subCurveIndex;
          }
        }
      }
    }
  }
  if (nextCurveIndex < 0) {
    return;
  }
  m_cursor->moveTo(nextT, nextT, nextY);
  m_selectedSubCurveIndex = nextSubCurve;
  selectFunctionWithCursor(nextCurveIndex, true);
  return;
}

void GraphController::refreshPointsOfInterest() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
  if (f->isAlongY()) {
    m_pointsOfInterest.setBoundsAndCompute(m_graphRange->yMin(), m_graphRange->yMax());
    m_view.reload();
  } else {
    Range1D dirtyRange = m_pointsOfInterest.setBoundsAndCompute(m_graphRange->xMin(), m_graphRange->xMax());
    if (dirtyRange.isValid()) {
      float dotRadius = (Dots::LargeDotDiameter * m_view.pixelWidth()) * 0.5f;
      m_view.reloadBetweenBounds(dirtyRange.min() - dotRadius, dirtyRange.max() + dotRadius);
    }
  }
}

}
