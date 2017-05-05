#include "graph_controller.h"

using namespace Shared;

namespace Sequence {

GraphController::GraphController(Responder * parentResponder, SequenceStore * sequenceStore, ButtonRowController * header) :
  FunctionGraphController(parentResponder, header, &m_graphRange, &m_view),
  m_bannerView(),
  m_view(sequenceStore, &m_graphRange, &m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(&m_cursor, this),
  m_curveParameterController(this, &m_graphRange, &m_cursor),
  m_termSumController(this, &m_view, &m_graphRange, &m_cursor),
  m_sequenceStore(sequenceStore)
{
}

void GraphController::viewWillAppear() {
  m_view.setVerticalCursor(false);
  m_view.setCursorView(&m_cursorView);
  m_view.setBannerView(&m_bannerView);
  m_view.setHighlight(-1.0f, -1.0f);
  FunctionGraphController::viewWillAppear();
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
  m_termSumController.setSequence(m_sequenceStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor));
  return FunctionGraphController::handleEnter();
}

bool GraphController::moveCursorHorizontally(int direction) {
  float xCursorPosition = roundf(m_cursor.x());
  if (direction < 0 && xCursorPosition <= 0) {
    return false;
  }
  /* The cursor moves by step of at minimum 1. If the windowRange is to large
   * compared to the resolution, the cursor takes bigger round step to cross
   * the window in approximatively resolution steps. */
  float step = ceilf((interactiveCurveViewRange()->xMax()-interactiveCurveViewRange()->xMin())/m_view.resolution());
  step = step < 1.0f ? 1.0f : step;
  float x = direction > 0 ? xCursorPosition + step:
    xCursorPosition -  step;
  if (x < 0.0f) {
    return false;
  }
  Sequence * s = m_sequenceStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = s->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor.moveTo(x, y);
  m_graphRange.panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

void GraphController::initCursorParameters() {
  float x = roundf((interactiveCurveViewRange()->xMin()+interactiveCurveViewRange()->xMax())/2.0f);
  m_indexFunctionSelectedByCursor = 0;
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  int functionIndex = 0;
  float y = 0;
  do {
    Sequence * firstFunction = m_sequenceStore->activeFunctionAtIndex(functionIndex++);
    y = firstFunction->evaluateAtAbscissa(x, myApp->localContext());
  } while (isnan(y) && functionIndex < m_sequenceStore->numberOfActiveFunctions());
  m_cursor.moveTo(x, y);
  m_graphRange.panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

CurveViewRange * GraphController::interactiveCurveViewRange() {
  return &m_graphRange;
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
