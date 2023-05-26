#include "graph_controller.h"

#include <apps/i18n.h>
#include <float.h>
#include <limits.h>

#include <algorithm>
#include <cmath>

#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

GraphController::GraphController(
    Responder *parentResponder,
    Escher::InputEventHandlerDelegate *inputEventHandlerDelegate,
    Escher::ButtonRowController *header, CurveViewRange *interactiveRange,
    CurveViewCursor *cursor, int *selectedCurveIndex,
    SequenceStore *sequenceStore)
    : FunctionGraphController(parentResponder, inputEventHandlerDelegate,
                              header, interactiveRange, &m_view, cursor,
                              selectedCurveIndex),
      m_bannerView(this, inputEventHandlerDelegate, this),
      m_view(sequenceStore, interactiveRange, m_cursor, &m_bannerView,
             &m_cursorView),
      m_graphRange(interactiveRange),
      m_curveParameterController(inputEventHandlerDelegate, this,
                                 &m_cobwebController, interactiveRange,
                                 m_cursor),
      m_sequenceSelectionController(this),
      m_termSumController(this, inputEventHandlerDelegate, &m_view,
                          interactiveRange, m_cursor),
      m_cobwebController(this, inputEventHandlerDelegate, &m_view,
                         interactiveRange, m_cursor, &m_bannerView,
                         &m_cursorView, sequenceStore),
      m_sequenceStore(sequenceStore) {
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

void GraphController::viewWillAppear() {
  if (m_cobwebController.stepIsInitialized()) {
    moveToRank(m_cobwebController.rankAtCurrentStep());
    m_cobwebController.resetStep();
  }
  m_view.setCursorView(&m_cursorView);
  m_cursorView.resetMemoization();
  m_smallestRank = m_sequenceStore->smallestInitialRank();
  FunctionGraphController::viewWillAppear();
}

float GraphController::interestingXMin() const {
  int nmin = INT_MAX;
  int nbOfActiveModels = functionStore()->numberOfActiveFunctions();
  for (int i = 0; i < nbOfActiveModels; i++) {
    Shared::Sequence *s =
        functionStore()->modelForRecord(recordAtCurveIndex(i));
    nmin = std::min(nmin, s->initialRank());
  }
  assert(nmin < INT_MAX);
  return nmin;
}

bool GraphController::textFieldDidFinishEditing(AbstractTextField *textField,
                                                const char *text,
                                                Ion::Events::Event event) {
  Shared::TextFieldDelegateApp *myApp = App::app();
  double floatBody = ParseInputtedFloatValue<double>(text);
  if (myApp->hasUndefinedValue(floatBody)) {
    return false;
  }
  floatBody = std::fmax(0, std::round(floatBody));
  moveToRank(floatBody);
  return true;
}

void GraphController::moveToRank(int n) {
  double y =
      xyValues(selectedCurveIndex(), n, App::app()->localContext())
          .y();
  m_cursor->moveTo(n, n, y);
  panToMakeCursorVisible();
  reloadBannerView();
  m_view.reload();
}

Range2D GraphController::optimalRange(bool computeX, bool computeY,
                                      Range2D originalRange) const {
  Zoom::Function2DWithContext<float> evaluator = [](float x, const void *model,
                                                    Context *context) {
    const Shared::Sequence *s = static_cast<const Shared::Sequence *>(model);
    return s->evaluateXYAtParameter(x, context);
  };

  Range2D result;
  if (computeX) {
    float xMin = interestingXMin();
    *result.x() = Range1D(xMin, xMin + k_defaultXHalfRange, k_maxFloat);
  } else {
    *result.x() = *originalRange.x();
  }
  if (computeY) {
    Zoom zoom(result.xMin(), result.xMax(),
              InteractiveCurveViewRange::NormalYXRatio(),
              App::app()->localContext(), k_maxFloat);
    int nbOfActiveModels = functionStore()->numberOfActiveFunctions();
    for (int i = 0; i < nbOfActiveModels; i++) {
      Shared::Sequence *s =
          functionStore()->modelForRecord(recordAtCurveIndex(i));
      zoom.fitFullFunction(evaluator, s);
    }
    *result.y() = *zoom.range(true, false).y();
  }
  return Zoom::Sanitize(result, InteractiveCurveViewRange::NormalYXRatio(),
                        k_maxFloat);
}

Layout GraphController::SequenceSelectionController::nameLayoutAtIndex(
    int j) const {
  GraphController *graphController =
      static_cast<GraphController *>(m_graphController);
  SequenceStore *store = graphController->functionStore();
  ExpiringPointer<Shared::Sequence> sequence =
      store->modelForRecord(store->activeRecordAtIndex(j));
  return sequence->definitionName().clone();
}

void GraphController::openMenuForCurveAtIndex(int curveIndex) {
  m_termSumController.setRecord(recordAtCurveIndex(curveIndex));
  FunctionGraphController::openMenuForCurveAtIndex(curveIndex);
}

bool GraphController::moveCursorHorizontally(OMG::HorizontalDirection direction,
                                             int scrollSpeed) {
  int xCursorPosition = std::round(m_cursor->x());
  Shared::Sequence *s =
      functionStore()->modelForRecord(recordAtSelectedCurveIndex());
  int x = m_view.nextDotIndex(s, xCursorPosition, direction, scrollSpeed);
  if (x == xCursorPosition) {
    return false;
  }
  double y = s->evaluateXYAtParameter(static_cast<double>(x),
                                      App::app()->localContext())
                 .y();
  m_cursor->moveTo(x, x, y);
  return true;
}

double GraphController::defaultCursorT(Ion::Storage::Record record,
                                       bool ignoreMargins) {
  return std::fmax(0.0,
                   std::round(Shared::FunctionGraphController::defaultCursorT(
                       record, ignoreMargins)));
}

}  // namespace Sequence
