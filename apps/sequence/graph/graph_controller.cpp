#include "graph_controller.h"
#include <cmath>
#include <limits.h>
#include "../app.h"
#include <algorithm>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

GraphController::GraphController(Responder * parentResponder, ::InputEventHandlerDelegate * inputEventHandlerDelegate, SequenceStore * sequenceStore, CurveViewRange * graphRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * previousModelsVersions, uint32_t * rangeVersion, Preferences::AngleUnit * angleUnitVersion, ButtonRowController * header) :
  FunctionGraphController(parentResponder, inputEventHandlerDelegate, header, graphRange, &m_view, cursor, indexFunctionSelectedByCursor, modelVersion, previousModelsVersions, rangeVersion, angleUnitVersion),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(sequenceStore, graphRange, m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(graphRange),
  m_curveParameterController(inputEventHandlerDelegate, this, graphRange, m_cursor),
  m_termSumController(this, inputEventHandlerDelegate, &m_view, graphRange, m_cursor)
{
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

void GraphController::viewWillAppear() {
  m_view.setCursorView(&m_cursorView);
  FunctionGraphController::viewWillAppear();
}

float GraphController::interestingXMin() const {
  int nmin = INT_MAX;
  int nbOfActiveModels = functionStore()->numberOfActiveFunctions();
  for (int i = 0; i < nbOfActiveModels; i++) {
    Sequence * s = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    nmin = std::min(nmin, s->initialRank());
  }
  assert(nmin < INT_MAX);
  return nmin;
}

float GraphController::interestingXHalfRange() const {
  float standardRange = Shared::FunctionGraphController::interestingXHalfRange();
  int nmin = INT_MAX;
  int nmax = 0;
  int nbOfActiveModels = functionStore()->numberOfActiveFunctions();
  for (int i = 0; i < nbOfActiveModels; i++) {
    Sequence * s = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    int firstInterestingIndex = s->initialRank();
    nmin = std::min(nmin, firstInterestingIndex);
    nmax = std::max(nmax, firstInterestingIndex + static_cast<int>(standardRange));
  }
  assert(nmax - nmin >= standardRange);
  return nmax - nmin;
}

bool GraphController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  Shared::TextFieldDelegateApp * myApp = textFieldDelegateApp();
  double floatBody;
  if (myApp->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  floatBody = std::fmax(0, std::round(floatBody));
  double y = xyValues(selectedCurveIndex(), floatBody, myApp->localContext()).x2();
  m_cursor->moveTo(floatBody, floatBody, y);
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
  reloadBannerView();
  m_view.reload();
  return true;
}

bool GraphController::handleEnter() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  m_termSumController.setRecord(record);
  return FunctionGraphController::handleEnter();
}

bool GraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  double xCursorPosition = std::round(m_cursor->x());
  if (direction < 0 && xCursorPosition <= 0) {
    return false;
  }
  // The cursor moves by step that is larger than 1 and than a pixel's width.
  const int step = std::ceil(m_view.pixelWidth()) * scrollSpeed;
  double x = direction > 0 ? xCursorPosition + step:
    xCursorPosition -  step;
  if (x < 0.0) {
    return false;
  }
  Sequence * s = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor()));
  double y = s->evaluateXYAtParameter(x, textFieldDelegateApp()->localContext()).x2();
  m_cursor->moveTo(x, x, y);
  return true;
}

double GraphController::defaultCursorT(Ion::Storage::Record record) {
  return std::fmax(0.0, std::round(Shared::FunctionGraphController::defaultCursorT(record)));
}

InteractiveCurveViewRangeDelegate::Range GraphController::computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  float min = FLT_MAX;
  float max = -FLT_MAX;
  float xMin = interactiveCurveViewRange->xMin();
  float xMax = interactiveCurveViewRange->xMax();
  assert(functionStore()->numberOfActiveFunctions() > 0);
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    ExpiringPointer<Shared::Function> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    /* Scan x-range from the middle to the extrema in order to get balanced
     * y-range for even functions (y = 1/x). */
    double tMin = f->tMin();
    if (std::isnan(tMin)) {
      tMin = xMin;
    } else if (f->shouldClipTRangeToXRange()) {
      tMin = std::max<double>(tMin, xMin);
    }
    double tMax = f->tMax();
    if (std::isnan(tMax)) {
      tMax = xMax;
    } else if (f->shouldClipTRangeToXRange()) {
      tMax = std::min<double>(tMax, xMax);
    }
  /* In practice, a step smaller than a pixel's width is needed for sampling
   * the values of a function. Otherwise some relevant extremal values may be
   * missed. */
    float rangeStep = f->rangeStep();
    const float step = std::isnan(rangeStep) ? curveView()->pixelWidth() / 2.0f : rangeStep;
    const int balancedBound = std::floor((tMax-tMin)/2/step);
    for (int j = -balancedBound; j <= balancedBound ; j++) {
      float t = (tMin+tMax)/2 + step * j;
      Coordinate2D<float> xy = f->evaluateXYAtParameter(t, context);
      float x = xy.x1();
      if (!std::isnan(x) && !std::isinf(x) && x >= xMin && x <= xMax) {
        float y = xy.x2();
        if (!std::isnan(y) && !std::isinf(y)) {
          min = std::min(min, y);
          max = std::max(max, y);
        }
      }
    }
  }
  InteractiveCurveViewRangeDelegate::Range range;
  range.min = min;
  range.max = max;
  return range;
}

}
