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
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  reloadBannerView();
  m_view.reload();
  return true;
}

bool GraphController::handleEnter() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  m_termSumController.setRecord(record);
  return FunctionGraphController::handleEnter();
}

bool GraphController::moveCursorHorizontally(int direction, bool fast) {
  double xCursorPosition = std::round(m_cursor->x());
  if (direction < 0 && xCursorPosition <= 0) {
    return false;
  }
  // The cursor moves by step that is larger than 1 and than a pixel's width.
  const int step = std::ceil(m_view.pixelWidth());
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

}
