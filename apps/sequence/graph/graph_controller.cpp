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
    nmin = minInt(nmin, s->initialRank());
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
    nmin = minInt(nmin, firstInterestingIndex);
    nmax = maxInt(nmax, firstInterestingIndex + standardRange);
  }
  assert(nmax - nmin >= standardRange);
  return nmax - nmin;
}

bool GraphController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  floatBody = std::round(floatBody);
  double y = yValue(selectedCurveIndex(), floatBody, textFieldDelegateApp()->localContext());
  m_cursor->moveTo(floatBody, y);
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
  Sequence * s = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor()));
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  double y = s->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor->moveTo(x, y);
  return true;
}

double GraphController::defaultCursorAbscissa() {
  return std::round(Shared::FunctionGraphController::defaultCursorAbscissa());
}

}
