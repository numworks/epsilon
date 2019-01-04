#include "graph_controller.h"
#include <cmath>
#include <limits.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

static inline int minInt(int x, int y) { return (x < y ? x : y); }
static inline int maxInt(int x, int y) { return (x > y ? x : y); }

GraphController::GraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, SequenceStore * sequenceStore, CurveViewRange * graphRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Preferences::AngleUnit * angleUnitVersion, ButtonRowController * header) :
  FunctionGraphController(parentResponder, inputEventHandlerDelegate, header, graphRange, &m_view, cursor, indexFunctionSelectedByCursor, modelVersion, rangeVersion, angleUnitVersion),
  m_bannerView(),
  m_view(sequenceStore, graphRange, m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(graphRange),
  m_curveParameterController(inputEventHandlerDelegate, this, graphRange, m_cursor),
  m_termSumController(this, inputEventHandlerDelegate, &m_view, graphRange, m_cursor),
  m_sequenceStore(sequenceStore)
{
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (m_sequenceStore->numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

float GraphController::interestingXMin() const {
  int nmin = INT_MAX;
  for (int i = 0; i < m_sequenceStore->numberOfModels(); i++) {
    Sequence * s = m_sequenceStore->modelAtIndex(i);
    if (s->isDefined() && s->isActive()) {
      nmin = minInt(nmin, s->initialRank());
    }
  }
  assert(nmin < INT_MAX);
  return nmin;
}

float GraphController::interestingXHalfRange() const {
  float standardRange = Shared::FunctionGraphController::interestingXHalfRange();
  int nmin = INT_MAX;
  int nmax = 0;
  for (int i = 0; i < m_sequenceStore->numberOfModels(); i++) {
    Sequence * s = m_sequenceStore->modelAtIndex(i);
    if (s->isDefined() && s->isActive()) {
      int firstInterestingIndex = s->initialRank();
      nmin = minInt(nmin, firstInterestingIndex);
      nmax = maxInt(nmax, firstInterestingIndex + standardRange);
    }
  }
  assert(nmax - nmin >= standardRange);
  return nmax - nmin;
}

bool GraphController::handleEnter() {
  m_termSumController.setFunction(m_sequenceStore->activeFunctionAtIndex(indexFunctionSelectedByCursor()));
  return FunctionGraphController::handleEnter();
}

bool GraphController::moveCursorHorizontally(int direction) {
  double xCursorPosition = std::round(m_cursor->x());
  if (direction < 0 && xCursorPosition <= 0) {
    return false;
  }
  /* The cursor moves by step of at minimum 1. If the windowRange is to large
   * compared to the resolution, the cursor takes bigger round step to cross
   * the window in approximatively resolution steps. */
  double step = std::ceil((interactiveCurveViewRange()->xMax()-interactiveCurveViewRange()->xMin())/m_view.resolution());
  step = step < 1.0 ? 1.0 : step;
  double x = direction > 0 ? xCursorPosition + step:
    xCursorPosition -  step;
  if (x < 0.0) {
    return false;
  }
  Sequence * s = m_sequenceStore->activeFunctionAtIndex(indexFunctionSelectedByCursor());
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  double y = s->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor->moveTo(x, y);
  m_graphRange->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

double GraphController::defaultCursorAbscissa() {
  return std::round(Shared::FunctionGraphController::defaultCursorAbscissa());
}

}
