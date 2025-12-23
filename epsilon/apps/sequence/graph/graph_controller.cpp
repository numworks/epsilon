#include "graph_controller.h"

#include <apps/i18n.h>
#include <float.h>
#include <limits.h>
#include <poincare/numeric_solver/zoom.h>

#include <algorithm>
#include <cmath>

#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

GraphController::GraphController(Responder* parentResponder,
                                 Escher::ButtonRowController* header,
                                 CurveViewRange* interactiveRange,
                                 CurveViewCursor* cursor,
                                 int* selectedCurveIndex,
                                 SequenceStore* sequenceStore)
    : FunctionGraphController(parentResponder, header, interactiveRange,
                              &m_view, cursor, selectedCurveIndex),
      m_bannerView(this, this),
      m_view(sequenceStore, interactiveRange, m_cursor, &m_bannerView,
             &m_cursorView),
      m_graphRange(interactiveRange),
      m_curveParameterController(this, &m_cobwebController, interactiveRange,
                                 m_cursor),
      m_sequenceSelectionController(this),
      m_termSumController(this, &m_view, interactiveRange, m_cursor),
      m_cobwebController(this, &m_view, interactiveRange, m_cursor,
                         &m_bannerView, &m_cursorView, sequenceStore),
      m_sequenceStore(sequenceStore) {
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (functionOrSequenceContext().numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

void GraphController::viewWillAppear() {
  m_view.setCursorView(&m_cursorView);
  m_cursorView.resetMemoization();
  if (m_cobwebController.stepIsInitialized()) {
    moveToRank(m_cobwebController.rankAtCurrentStep());
    m_cobwebController.resetStep();
  }
  m_smallestRank = m_sequenceStore->smallestInitialRank();
  FunctionGraphController::viewWillAppear();
}

float GraphController::interestingXMin() const {
  int nmin = INT_MAX;
  int nbOfActiveModels = functionOrSequenceContext().numberOfActiveFunctions();
  for (int i = 0; i < nbOfActiveModels; i++) {
    const Shared::Sequence* s =
        functionOrSequenceContext().modelForRecord(recordAtCurveIndex(i));
    nmin = std::min(nmin, s->initialRank());
  }
  assert(nmin < INT_MAX);
  return nmin;
}

bool GraphController::textFieldDidFinishEditing(AbstractTextField* textField,
                                                Ion::Events::Event event) {
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  floatBody = std::fmax(0, std::round(floatBody));
  moveToRank(floatBody);
  return true;
}

void GraphController::moveToRank(int n) {
  double y = xyValues(selectedCurveIndex(), n).y();
  m_cursor->moveTo(n, n, y);
  panToMakeCursorVisible();
  reloadBannerView();
  m_view.reload();
}

Range2D<float> GraphController::optimalRange(
    bool computeX, bool computeY, Range2D<float> originalRange) const {
  Range2D<float> result;
  if (computeX) {
    float xMin = interestingXMin();
    *result.x() = Range1D<float>(xMin, xMin + k_defaultXHalfRange, k_maxFloat);
  } else {
    *result.x() = *originalRange.x();
  }
  if (computeY) {
    Zoom zoom(InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
    int nbOfActiveModels =
        functionOrSequenceContext().numberOfActiveFunctions();
    assert(nbOfActiveModels <= Shared::SequenceStore::k_maxNumberOfSequences);
    const Shared::Sequence*
        sequences[Shared::SequenceStore::k_maxNumberOfSequences];
    for (int i = 0; i < nbOfActiveModels; i++) {
      sequences[i] =
          functionOrSequenceContext().modelForRecord(recordAtCurveIndex(i));
    }
    int min = std::ceil(result.xMin());
    int max = std::floor(result.xMax());
    /* Loop first on abscissa so that sequences step ranks together. */
    for (int n = min; n <= max; n++) {
      for (int i = 0; i < nbOfActiveModels; i++) {
        zoom.fitPoint(
            sequences[i]->evaluateXYAtParameter(static_cast<float>(n)));
      }
    }
    *result.y() = *zoom.range(true, false).y();
  }
  return Zoom<float>::Sanitize(
      result, InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
}

const Layout GraphController::SequenceSelectionController::nameLayoutAtIndex(
    int j) const {
  GraphController* graphController =
      static_cast<GraphController*>(m_graphController);
  const SequenceStore* sequenceContext =
      &graphController->functionOrSequenceContext();
  OMG::ExpiringPointer<const Shared::Sequence> sequence =
      sequenceContext->modelForRecord(sequenceContext->activeRecordAtIndex(j));
  return sequence->definitionName();
}

void GraphController::openMenuForCurveAtIndex(int curveIndex) {
  m_termSumController.setRecord(recordAtCurveIndex(curveIndex));
  FunctionGraphController::openMenuForCurveAtIndex(curveIndex);
}

bool GraphController::moveCursorHorizontally(OMG::HorizontalDirection direction,
                                             int scrollSpeed) {
  int xCursorPosition = std::round(m_cursor->x());
  const Shared::Sequence* s =
      functionOrSequenceContext().modelForRecord(recordAtSelectedCurveIndex());
  int x = m_view.nextDotIndex(s, xCursorPosition, direction, scrollSpeed);
  if (x == xCursorPosition) {
    return false;
  }
  double y = s->evaluateXYAtParameter(static_cast<double>(x)).y();
  m_cursor->moveTo(x, x, y);
  return true;
}

double GraphController::defaultCursorT(Ion::Storage::Record record,
                                       bool ignoreMargins) {
  return std::fmax(0.0,
                   std::round(Shared::FunctionGraphController::defaultCursorT(
                       record, ignoreMargins)));
}

void GraphController::openMenuForSelectedCurve() {
  m_curveParameterController.setRecord(recordAtSelectedCurveIndex());
  stackController()->push(&m_curveParameterController);
}

}  // namespace Sequence
