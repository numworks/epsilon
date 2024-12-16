#include "function_graph_cell.h"

#include <poincare/expression.h>
#include <poincare/print.h>

#include "escher/palette.h"
#include "kandinsky/color.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

template <size_t N>
void FunctionAxis<N>::reloadAxis(AbstractPlotView* plotView, OMG::Axis axis) {
  PlotPolicy::LabeledAxis<N>::reloadAxis(plotView, axis);
  const FunctionModel* model =
      static_cast<const FunctionModel*>(plotView->range());
  float t =
      axis == OMG::Axis::Horizontal ? model->abscissa() : model->ordinate();
  // Compute special labels content and position
  Print::CustomPrintf(
      m_specialLabel, PlotPolicy::AbstractLabeledAxis::k_labelBufferMaxSize,
      "%*.*ef", t, Preferences::PrintFloatMode::Decimal, k_labelsPrecision);
  m_specialLabelRect = KDRectZero;
}

template <size_t N>
void FunctionAxis<N>::drawAxis(const AbstractPlotView* plotView, KDContext* ctx,
                               KDRect rect, OMG::Axis axis) const {
  const FunctionModel* model =
      static_cast<const FunctionModel*>(plotView->range());
  float t =
      axis == OMG::Axis::Horizontal ? model->abscissa() : model->ordinate();
  OMG::Axis otherAxis = axis == OMG::Axis::Horizontal ? OMG::Axis::Vertical
                                                      : OMG::Axis::Horizontal;
  float other = otherAxis == OMG::Axis::Horizontal ? model->abscissa()
                                                   : model->ordinate();
  // Compute the special label position needed by labelWillBeDisplayed
  if (m_specialLabelRect == KDRectZero) {
    PlotPolicy::AbstractLabeledAxis::computeLabelsRelativePosition(plotView,
                                                                   axis);
    m_specialLabelRect =
        PlotPolicy::AbstractLabeledAxis::labelRect(N, t, plotView, axis)
            .paddedWith(k_labelAvoidanceMargin);
  }
  // Draw the usual graduations
  PlotPolicy::SimpleAxis::drawAxis(plotView, ctx, rect, axis);
  // Draw the dashed lines since they are the ticks of the special labels
  plotView->drawDashedStraightSegment(ctx, rect, otherAxis, t, 0.0f, other,
                                      k_specialLabelsColor);
  // Draw the special label
  PlotPolicy::AbstractLabeledAxis::drawLabel(N, t, plotView, ctx, rect, axis,
                                             k_specialLabelsColor);
}

template <size_t N>
bool FunctionAxis<N>::labelWillBeDisplayed(int i, KDRect labelRect) const {
  return i == N ||
         (!labelRect.intersects(m_specialLabelRect) &&
          PlotPolicy::LabeledAxis<N>::labelWillBeDisplayed(i, labelRect));
}

void FunctionGraphPolicy::drawPlot(const Shared::AbstractPlotView* plotView,
                                   KDContext* ctx, KDRect rect) const {
  SystemFunctionScalar function = m_model->function();
  assert(!function.recursivelyMatches(&NewExpression::isSequence));

  Curve2DEvaluation<float> evaluateFunction = [](float t, void* model, void*) {
    SystemFunctionScalar* e = (Expression*)model;
    return Coordinate2D<float>(t, e->approximateToScalarWithValue(t));
  };

  // Draw the curve
  CurveDrawing plot(Curve2D(evaluateFunction, &function), nullptr,
                    m_model->xMin(), m_model->xMax(), plotView->pixelWidth(),
                    k_color);
  plot.draw(plotView, ctx, rect);

  // Since exp(-2.5) is generalized as exp(-x), x cannot be negative
  float x = m_model->abscissa();
  assert(x >= 0);
  float y = m_model->ordinate();

  // Draw the dot
  plotView->drawDot(ctx, rect, Dots::Size::Large, {x, y}, k_color);
}

template bool
FunctionAxis<PlotPolicy::AbstractLabeledAxis::k_maxNumberOfYLabels>::
    labelWillBeDisplayed(int i, KDRect labelRect) const;
template bool
FunctionAxis<PlotPolicy::AbstractLabeledAxis::k_maxNumberOfXLabels>::
    labelWillBeDisplayed(int i, KDRect labelRect) const;
template void
FunctionAxis<PlotPolicy::AbstractLabeledAxis::k_maxNumberOfXLabels>::reloadAxis(
    AbstractPlotView* plotView, OMG::Axis axis);
template void
FunctionAxis<PlotPolicy::AbstractLabeledAxis::k_maxNumberOfYLabels>::reloadAxis(
    AbstractPlotView* plotView, OMG::Axis axis);
template void
FunctionAxis<PlotPolicy::AbstractLabeledAxis::k_maxNumberOfXLabels>::drawAxis(
    const AbstractPlotView* plotView, KDContext* ctx, KDRect rect,
    OMG::Axis axis) const;
template void
FunctionAxis<PlotPolicy::AbstractLabeledAxis::k_maxNumberOfYLabels>::drawAxis(
    const AbstractPlotView* plotView, KDContext* ctx, KDRect rect,
    OMG::Axis axis) const;

}  // namespace Calculation
