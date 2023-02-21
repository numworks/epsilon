#include "cobweb_controller.h"

#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include "../../shared/text_field_delegate.h"
#include "../app.h"
#include "apps/sequence/graph/cobweb_graph_view.h"
#include "apps/sequence/graph/graph_controller.h"
#include "apps/shared/continuous_function.h"
#include "apps/shared/sequence_context.h"
#include "apps/shared/sequence_store.h"
#include "ion/events.h"
#include "ion/unicode/code_point.h"
#include "poincare/preferences.h"
#include "poincare/print.h"
#include "poincare/sequence.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

CobwebController::CobwebController(
    Responder* parentResponder,
    Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
    GraphController* graphController, GraphView* graphView,
    CurveViewRange* graphRange, CurveViewCursor* cursor,
    XYBannerView* bannerView, CursorView* cursorView,
    SequenceStore* sequenceStore)
    : Shared::SimpleInteractiveCurveViewController(parentResponder, cursor),
      m_graphView(&m_graphRange, cursor, bannerView, cursorView),
      m_graphController(graphController),
      m_cursor(cursor),
      m_bannerView(bannerView),
      m_graphRange(),
      m_sequenceStore(sequenceStore) {}

const char* CobwebController::title() {
  return I18n::translate(I18n::Message::CobwebPlot);
}

bool CobwebController::handleLeftRightEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Right && updateStep(1)) {
    return true;
  }
  if (event == Ion::Events::Left && updateStep(-1)) {
    return true;
  }
  return false;
}

void CobwebController::viewWillAppear() {
  setupRange();
  m_step = 0;
  m_initialZoom = true;
  m_graphView.setRecord(m_record);
  m_graphView.resetCachedStep();
  m_graphView.setStep(0);
  m_graphView.setCursorView(nullptr);
  m_graphView.setFocus(true);
  reloadBannerView();
}

void CobwebController::viewDidDisappear() {
  m_graphController->moveToRank(rankAtCurrentStep());
}

void CobwebController::setupRange() {
  /* Compute the bounding rect of the maximum number of values we will want to
   * draw such that we never need to move the view. */
  SequenceContext* sequenceContext = App::app()->localContext();
  Zoom zoom(0.f, INFINITY, InteractiveCurveViewRange::NormalYXRatio(),
            sequenceContext, InteractiveCurveViewRange::k_maxFloat);
  for (int step = 0; step < CobwebGraphView::k_maximumNumberOfSteps; step++) {
    float value = sequence()
                      ->evaluateXYAtParameter(
                          static_cast<float>(rankAtStep(step)), sequenceContext)
                      .x2();
    zoom.fitPoint(Coordinate2D<float>(value, step == 0 ? 0.f : value), false,
                  k_margin, k_margin, k_margin, k_margin);
  }
  Range2D zoomRange = zoom.range(false, false);
  InteractiveCurveViewRange range;
  range.setXMin(zoomRange.xMin());
  range.setXMax(zoomRange.xMax());
  range.setYMin(zoomRange.yMin());
  range.setYMax(zoomRange.yMax());
  m_graphRange = range;
  m_graphView.reload();
}

void CobwebController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  m_isSuitable = sequence()->isSimplyRecursive(App::app()->localContext());
}

ExpiringPointer<Shared::Sequence> CobwebController::sequence() const {
  return m_sequenceStore->modelForRecord(m_record);
}

bool CobwebController::isRecordSuitable() const { return m_isSuitable; }

void CobwebController::reloadBannerView() {
  m_bannerView->abscissaSymbol()->setText("n=");
  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%i", rankAtCurrentStep());
  m_bannerView->abscissaValue()->setText(buffer);
  int nameLength = sequence()->nameWithArgument(buffer, bufferSize);
  double u_n = sequence()->valueAtRank<double>(rankAtCurrentStep(),
                                               App::app()->localContext());
  Poincare::Print::CustomPrintf(buffer + nameLength, bufferSize - nameLength,
                                "=%*.*ef", u_n,
                                Preferences::PrintFloatMode::Decimal,
                                Preferences::LargeNumberOfSignificantDigits);
  m_bannerView->ordinateView()->setText(buffer);
  m_bannerView->reload();
}

bool CobwebController::handleEnter() { return updateStep(1); }

bool CobwebController::handleZoom(Ion::Events::Event event) {
  m_initialZoom = false;
  float ratio =
      event == Ion::Events::Plus ? 1.f / k_zoomOutRatio : k_zoomOutRatio;
  float value =
      sequence()
          ->evaluateXYAtParameter(static_cast<float>(rankAtCurrentStep()),
                                  App::app()->localContext())
          .x2();
  interactiveCurveViewRange()->zoom(ratio, value, m_step ? value : 0.f);
  m_graphView.resetCachedStep();
  curveView()->reload(true);
  return true;
}

bool CobwebController::updateStep(int delta) {
  if (m_step + delta < 0 ||
      m_step + delta >= CobwebGraphView::k_maximumNumberOfSteps) {
    return false;
  }
  m_step += delta;
  double u_n = sequence()->valueAtRank<double>(rankAtCurrentStep(),
                                               App::app()->localContext());
  double x = u_n;
  double y = m_step == 0 ? 0.f : u_n;
  if (!m_initialZoom && interactiveCurveViewRange()->panToMakePointVisible(
                            x, y, k_margin, k_margin, k_margin, k_margin,
                            m_graphView.pixelWidth())) {
    m_graphView.resetCachedStep();
  }
  m_graphView.setStep(m_step);
  m_graphView.reload(false, true);
  reloadBannerView();
  return true;
}

}  // namespace Sequence
