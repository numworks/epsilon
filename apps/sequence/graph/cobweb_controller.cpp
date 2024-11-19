#include "cobweb_controller.h"

#include <poincare/layout.h>
#include <poincare/numeric/zoom.h>
#include <poincare/old/sequence.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include "../app.h"
#include "apps/sequence/graph/cobweb_graph_view.h"
#include "apps/sequence/graph/graph_controller.h"
#include "apps/shared/continuous_function.h"
#include "apps/shared/sequence_context.h"
#include "apps/shared/sequence_store.h"
#include "ion/events.h"
#include "omg/code_point.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

CobwebController::CobwebController(Responder* parentResponder,
                                   GraphView* graphView,
                                   CurveViewRange* graphRange,
                                   CurveViewCursor* cursor,
                                   XYBannerView* bannerView,
                                   CursorView* cursorView,
                                   SequenceStore* sequenceStore)
    : Shared::SimpleInteractiveCurveViewController(parentResponder, cursor),
      m_graphView(&m_graphRange, cursor, bannerView, cursorView),
      m_bannerView(bannerView),
      m_graphRange(),
      m_step(-1),
      m_sequenceStore(sequenceStore) {}

const char* CobwebController::title() {
  return I18n::translate(I18n::Message::CobwebPlot);
}

bool CobwebController::handleLeftRightEvent(Ion::Events::Event event) {
  return (event == Ion::Events::Right && updateStep(1)) ||
         (event == Ion::Events::Left && updateStep(-1));
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

void CobwebController::setupRange() {
  /* Compute the bounding rect of the maximum number of values we will want to
   * draw such that we never need to move the view. */
  SequenceContext* sequenceContext = App::app()->localContext();
  Zoom zoom(0.f, INFINITY, InteractiveCurveViewRange::NormalYXRatio(),
            InteractiveCurveViewRange::k_maxFloat);
  for (int step = 0; step < CobwebGraphView::k_maximumNumberOfSteps; step++) {
    float value = sequence()
                      ->evaluateXYAtParameter(
                          static_cast<float>(rankAtStep(step)), sequenceContext)
                      .y();
    zoom.fitPoint(Coordinate2D<float>(value, step == 0 ? 0.f : value), false,
                  k_margin, k_margin, k_margin, k_margin);
  }
  Range2D<float> zoomRange = zoom.range(false, false);
  InteractiveCurveViewRange range;
  range.setXRange(zoomRange.xMin(), zoomRange.xMax());
  range.setYRange(zoomRange.yMin(), zoomRange.yMax());
  m_graphRange = range;
  m_graphView.reload();
}

void CobwebController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  m_isSuitable = sequence()->isSuitableForCobweb(App::app()->localContext());
}

ExpiringPointer<Shared::Sequence> CobwebController::sequence() const {
  return m_sequenceStore->modelForRecord(m_record);
}

bool CobwebController::isRecordSuitable() const { return m_isSuitable; }

void CobwebController::reloadBannerView() {
  m_bannerView->abscissaSymbol()->setText("n=");
  constexpr size_t bufferSize = 20;
  char buffer[bufferSize];
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%i", rankAtCurrentStep());
  m_bannerView->abscissaValue()->setText(buffer);
  size_t nameLength = sequence()->nameWithArgument(buffer, bufferSize);
  double u_n =
      sequence()
          ->evaluateXYAtParameter(static_cast<double>(rankAtCurrentStep()),
                                  App::app()->localContext())
          .y();
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
          .y();
  interactiveCurveViewRange()->zoom(ratio, value, m_step ? value : 0.f);
  m_graphView.resetCachedStep();
  curveView()->reload();
  return true;
}

bool CobwebController::updateStep(int delta) {
  if (m_step + delta < 0 ||
      m_step + delta >= CobwebGraphView::k_maximumNumberOfSteps) {
    return false;
  }
  m_step += delta;
  double u_n =
      sequence()
          ->evaluateXYAtParameter(static_cast<double>(rankAtCurrentStep()),
                                  App::app()->localContext())
          .y();
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

void CobwebController::privateModalViewAltersFirstResponder(
    FirstResponderAlteration alteration) {
  if (alteration == FirstResponderAlteration::DidRestore) {
    /* This avoids that previous gray dotted line is saved in a buffer when the
     * same gray dotted line is drawn over it.*/
    m_graphView.resetCachedStep();
    m_graphView.reload(false, true);
  }
}

}  // namespace Sequence
