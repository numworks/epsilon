#include "graph_controller.h"
#include <cmath>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

GraphController::GraphController(Responder * parentResponder, SequenceStore * sequenceStore, CurveViewRange * graphRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Expression::AngleUnit * angleUnitVersion, ButtonRowController * header) :
  FunctionGraphController(parentResponder, header, graphRange, &m_view, cursor, indexFunctionSelectedByCursor, modelVersion, rangeVersion, angleUnitVersion),
  m_bannerView(),
  m_view(sequenceStore, graphRange, m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(graphRange),
  m_curveParameterController(this, graphRange, m_cursor),
  m_termSumController(this, &m_view, graphRange, m_cursor),
  m_sequenceStore(sequenceStore)
{
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (m_sequenceStore->numberOfDefinedFunctions() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

TermSumController * GraphController::termSumController() {
  return &m_termSumController;
}

BannerView * GraphController::bannerView() {
  return &m_bannerView;
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

void GraphController::initCursorParameters() {
  double x = std::round((interactiveCurveViewRange()->xMin()+interactiveCurveViewRange()->xMax())/2.0);
  selectFunctionWithCursor(0);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  int functionIndex = 0;
  double y = 0;
  do {
    Sequence * firstFunction = m_sequenceStore->activeFunctionAtIndex(functionIndex++);
    y = firstFunction->evaluateAtAbscissa(x, myApp->localContext());
  } while (std::isnan(y) && functionIndex < m_sequenceStore->numberOfActiveFunctions());
  m_cursor->moveTo(x, y);
  m_graphRange->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

CurveViewRange * GraphController::interactiveCurveViewRange() {
  return m_graphRange;
}

SequenceStore * GraphController::functionStore() const {
  return m_sequenceStore;
}

GraphView * GraphController::functionGraphView() {
  return &m_view;
}

CurveParameterController * GraphController::curveParameterController() {
  return &m_curveParameterController;
}

}
