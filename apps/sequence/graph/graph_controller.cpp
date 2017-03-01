#include "graph_controller.h"

using namespace Shared;

namespace Sequence {

GraphController::GraphController(Responder * parentResponder, SequenceStore * sequenceStore, ButtonRowController * header) :
  FunctionGraphController(parentResponder, header, &m_graphRange, &m_view),
  m_bannerView(BannerView()),
  m_view(GraphView(sequenceStore, &m_graphRange, &m_cursor, &m_bannerView, &m_cursorView)),
  m_graphRange(CurveViewRange(&m_cursor, this)),
  m_curveParameterController(CurveParameterController(this, &m_graphRange, &m_cursor)),
  m_termSumController(TermSumController(this, &m_view, &m_graphRange, &m_cursor)),
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

const char * GraphController::emptyMessage() {
  if (m_sequenceStore->numberOfDefinedFunctions() == 0) {
    return "Aucune suite";
  }
  return "Aucune suite activee";
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
  float x = direction > 0 ? xCursorPosition + 1.0f :
    xCursorPosition -  1.0f;
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
