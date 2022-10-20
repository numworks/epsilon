#include "cobweb_controller.h"
#include "../../shared/text_field_delegate.h"
#include "apps/sequence/graph/cobweb_graph_view.h"
#include "apps/sequence/graph/graph_controller.h"
#include "apps/shared/continuous_function.h"
#include "apps/shared/sequence_context.h"
#include "apps/shared/sequence_store.h"
#include "ion/events.h"
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include "../app.h"
#include "ion/unicode/code_point.h"
#include "poincare/preferences.h"
#include "poincare/print.h"
#include "poincare/sequence.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Sequence {

CobwebController::CobwebController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphController * graphController, GraphView * graphView, CurveViewRange * graphRange, CurveViewCursor * cursor, XYBannerView * bannerView, CursorView * cursorView, SequenceStore * sequenceStore) :
  Shared::SimpleInteractiveCurveViewController(parentResponder, cursor),
  m_graphView(&m_graphRange, cursor, bannerView, cursorView),
  m_graphController(graphController),
  m_cursor(cursor),
  m_bannerView(bannerView),
  m_graphRange(),
  m_sequenceStore(sequenceStore)
{
}

const char * CobwebController::title() {
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
  setupZoom();
  m_step = 0;
  m_graphView.setSequence(sequence().pointer());
  m_graphView.resetCachedStep();
  m_graphView.setStep(0);
  m_graphView.setCursorView(nullptr);
  m_graphView.setFocus(true);
  reloadBannerView();
}

void CobwebController::viewDidDisappear() {
  m_graphController->moveToRank(m_step);
}

void CobwebController::setupZoom() {
  /* compute the bounding rect of the maximum number of value we will want to
   * draw such that we never need to move the view */
  SequenceContext * sequenceContext = App::app()->localContext();
  // We need the x-axis in the view
  float xMin = 0.f, xMax = 0.f;
  for (int step = 0; step < CobwebGraphView::k_maximumNumberOfSteps; step++) {
    float value = sequence()->evaluateXYAtParameter(static_cast<float>(step + sequence()->initialRank()), sequenceContext).x2();
    xMin = std::min(xMin, value);
    xMax = std::max(xMax, value);
  }
  float length = xMax-xMin;
  xMin -= k_margin * length;
  xMax += k_margin * length;

  float yMin = xMin;
  float yMax = xMax;
  if (std::abs(yMin)/(yMax-yMin) < k_bottomMargin) {
    yMin = -(yMax-yMin) * k_bottomMargin;
  }
  // panTomakepointvisible ?
  m_graphRange.setXAuto(false);
  m_graphRange.setXMin(xMin);
  m_graphRange.setXMax(xMax);
  m_graphRange.setYAuto(false);
  m_graphRange.setYMin(yMin);
  m_graphRange.setYMax(yMax);
  m_graphView.reload();
}

void CobwebController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  m_isSuitable = sequence()->isSimplyRecursive(App::app()->localContext());
}

ExpiringPointer<Shared::Sequence> CobwebController::sequence() const {
  return m_sequenceStore->modelForRecord(m_record);
}

bool CobwebController::isRecordSuitable() const {
  return m_isSuitable;
}

void CobwebController::reloadBannerView() {
  m_bannerView->abscissaSymbol()->setText("n=");
  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%i", m_step);
  m_bannerView->abscissaValue()->setText(buffer);
  int nameLength = sequence()->nameWithArgument(buffer, bufferSize);
  double u_n = sequence()->valueAtRank<double>(m_step, App::app()->localContext());
  Poincare::Print::CustomPrintf(buffer + nameLength, bufferSize - nameLength, "=%*.*ef", u_n, Preferences::PrintFloatMode::Decimal, Preferences::LargeNumberOfSignificantDigits);
  m_bannerView->ordinateView()->setText(buffer);
  m_bannerView->reload();
}

bool CobwebController::handleEnter() {
  return updateStep(1);
}

bool CobwebController::handleZoom(Ion::Events::Event event) {
  float ratio = event == Ion::Events::Plus ? 1.f / k_zoomOutRatio : k_zoomOutRatio;
  float value = sequence()->evaluateXYAtParameter(static_cast<float>(m_step + sequence()->initialRank()), App::app()->localContext()).x2();
  interactiveCurveViewRange()->zoom(ratio, value, m_step ? value : 0.f);
  m_graphView.resetCachedStep();
  curveView()->reload(true);
  return true;
}

bool CobwebController::updateStep(int delta) {
  if (m_step + delta < 0 || m_step + delta >= CobwebGraphView::k_maximumNumberOfSteps) {
    return false;
  }
  m_step += delta;
  double u_n = sequence()->valueAtRank<double>(m_step, App::app()->localContext());
  double x = u_n;
  double y = m_step == 0 ? u_n : 0.f;
  // TODO: For some reason panToMakePointVisible is stricter that our computation
  constexpr float marginDelta = 0.02;
  float margin = k_margin - marginDelta;
  if (interactiveCurveViewRange()->panToMakePointVisible(x, y, margin, margin, k_bottomMargin - marginDelta, margin, m_graphView.pixelWidth())) {
    m_graphView.resetCachedStep();
  }
  m_graphView.setStep(m_step);
  m_graphView.reload(false, true);
  reloadBannerView();
  return true;
}

}
