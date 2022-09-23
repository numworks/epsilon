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
  m_graphView.setStart(m_graphRange.xMin());
  m_graphView.setEnd(m_graphRange.xMax());
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
  for (int step = 0; step <= k_maximumNumberOfSteps; step++) {
    float value = sequence()->evaluateXYAtParameter(static_cast<float>(step + sequence()->initialRank()), sequenceContext).x2();
    xMin = std::min(xMin, value);
    xMax = std::max(xMax, value);
  }
  // bottomRatio takes the part hidden by the banner into account
  constexpr float bottomRatio = 0.14;
  constexpr float margin = 0.10;
  float length = xMax-xMin;
  xMin -= margin*length;
  xMax += margin*length;

  float yMin = xMin;
  float yMax = xMax;
  if (std::abs(yMin)/(yMax-yMin) < bottomRatio) {
    yMin = -(yMax-yMin)*bottomRatio;
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
  char buffer[10];
  sequence()->name(buffer, 10);
  // u(n+1) must depend on u(n) only not n nor v(n) nor u(n-1)
  m_isSuitable = sequence()->type() == Shared::Sequence::Type::SingleRecurrence && !sequence()->expressionClone().recursivelyMatches([](const Expression e, Context * context, void * arg) {
    if (e.type() == ExpressionNode::Type::Symbol) {
      const Poincare::Symbol symbol = static_cast<const Poincare::Symbol&>(e);
      return symbol.isSystemSymbol() ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
    }
    if (e.type() != ExpressionNode::Type::Sequence) {
      return TrinaryBoolean::Unknown;
    }
    const Poincare::Sequence seq = static_cast<const Poincare::Sequence&>(e);
    char * buffer = static_cast<char*>(arg);
    if (strcmp(seq.name(), buffer) != 0) {
      return TrinaryBoolean::True;
    }
    if (seq.childAtIndex(0).type() != ExpressionNode::Type::Symbol) {
      return TrinaryBoolean::True;
    }
    Expression child = seq.childAtIndex(0);
    const Poincare::Symbol symbol = static_cast<const Poincare::Symbol&>(child);
    return symbol.isSystemSymbol() ? TrinaryBoolean::False : TrinaryBoolean::True;
  }, App::app()->localContext(), ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, static_cast<void*>(buffer));
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
  double u_n = sequence()->valueAtRank<double>(m_step, App::app()->localContext());
  Poincare::Print::CustomPrintf(buffer, bufferSize, "u(n)=%*.*ef", u_n, Preferences::PrintFloatMode::Decimal, Preferences::LargeNumberOfSignificantDigits);
  m_bannerView->ordinateView()->setText(buffer);
  m_bannerView->reload();
}

bool CobwebController::handleEnter() {
  return updateStep(1);
}

bool CobwebController::updateStep(int delta) {
  if (m_step + delta < 0 || m_step + delta >= k_maximumNumberOfSteps) {
    return false;
  }
  m_step += delta;
  m_graphView.setStep(m_step);
  m_graphView.reload(false, true);
  reloadBannerView();
  return true;
}

}
