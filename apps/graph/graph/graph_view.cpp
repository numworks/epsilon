#include "graph_view.h"
#include "../app.h"
#include <assert.h>
#include <algorithm>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/matrix.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

GraphView::GraphView(InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, Shared::BannerView * bannerView, CursorView * cursorView) :
  FunctionGraphView(graphRange, cursor, bannerView, cursorView),
  m_functionsInterrupted(0),
  m_tangent(false)
{
}

void GraphView::reload(bool resetInterrupted, bool force) {
  if (m_tangent) {
    KDRect dirtyZone(KDRect(0, 0, bounds().width(), bounds().height()-m_bannerView->bounds().height()));
    markRectAsDirty(dirtyZone);
  }
  if (force || resetInterrupted) {
    m_functionsInterrupted = 0;
  }
  return FunctionGraphView::reload(resetInterrupted, force);
}

void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ContinuousFunctionStore * functionStore = App::app()->functionStore();
  const int activeFunctionsCount = functionStore->numberOfActiveFunctions();
  if (allFunctionsInterrupted(activeFunctionsCount)) {
    // All functions were interrupted, do not draw anything.
    return;
  }

  FunctionGraphView::drawRect(ctx, rect);
  int areaIndex = 0;
  for (int i = 0; i < activeFunctionsCount ; i++) {
    if (functionWasInterrupted(i)) {
      continue;
    }
    Ion::Storage::Record record = functionStore->activeRecordAtIndex(i);
    ExpiringPointer<ContinuousFunction> f = functionStore->modelForRecord(record);
    Poincare::UserCircuitBreakerCheckpoint checkpoint;
    if (CircuitBreakerRun(checkpoint)) {
      /* Calling ExpressionReduced here so that plot type and numberOfSubCurves
       * are properly updated, even if e isn't used. */
      Poincare::Expression e = f->expressionReduced(context());
      ContinuousFunction::AreaType area = f->areaType();
      ContinuousFunction::PlotType type = f->plotType();
      assert(f->numberOfSubCurves() <= 2);
      bool hasTwoCurves = (f->numberOfSubCurves() == 2);
      if (area == ContinuousFunction::AreaType::None) {
        if (e.type() == Poincare::ExpressionNode::Type::Dependency) {
          e = e.childAtIndex(0);
        }
        bool isUndefined = e.isUndefined();
        if (!isUndefined && (type == ContinuousFunction::PlotType::Parametric || hasTwoCurves)) {
          assert(e.type() == Poincare::ExpressionNode::Type::Matrix);
          assert(static_cast<Poincare::Matrix&>(e).numberOfRows() == 2);
          assert(static_cast<Poincare::Matrix&>(e).numberOfColumns() == 1);
          isUndefined = e.childAtIndex(0).isUndefined() && e.childAtIndex(1).isUndefined();
        }
        if (isUndefined) {
          // There is no need to plot anything.
          continue;
        }
      }
      ContinuousFunctionCache * cch = functionStore->cacheAtIndex(i);
      float tmin = f->tMin();
      float tmax = f->tMax();
      Axis axis = f->hasVerticalLines() ? Axis::Vertical : Axis::Horizontal;
      KDCoordinate rectMin = axis == Axis::Horizontal ? rect.left() - k_externRectMargin : rect.bottom() + k_externRectMargin;
      KDCoordinate rectMax = axis == Axis::Horizontal ? rect.right() + k_externRectMargin : rect.top() - k_externRectMargin;

      float tCacheMin, tCacheStep;
      float tStepNonCartesian = NAN;
      if (f->isAlongX()) {
        float rectLimit = pixelToFloat(axis, rectMin);
        /* Here, tCacheMin can depend on rect (and change as the user move)
         * because cache can be panned for cartesian curves, instead of being
         * entirely invalidated. */
        tCacheMin = std::isnan(rectLimit) ? tmin : std::max(tmin, rectLimit);
        tCacheStep = axis == Axis::Horizontal ? pixelWidth() : pixelHeight();
      } else {
        tCacheMin = tmin;
        // Compute tCacheStep and tStepNonCartesian
        ContinuousFunctionCache::ComputeNonCartesianSteps(&tStepNonCartesian, &tCacheStep, tmax, tmin);
      }
      ContinuousFunctionCache::PrepareForCaching(f.operator->(), cch, tCacheMin, tCacheStep);

      if (type == ContinuousFunction::PlotType::Polar) {
        // Polar
        assert(!std::isnan(tStepNonCartesian));
        drawPolarCurve(ctx, rect, tCacheMin, tmax, tStepNonCartesian, [](float t, void * model, void * context) {
            ContinuousFunction * f = (ContinuousFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c);
          }, f.operator->(), context(), false, f->color());
      } else if (type == ContinuousFunction::PlotType::Parametric) {
        assert(!std::isnan(tStepNonCartesian));
        // Parametric
        drawCurve(ctx, rect, tCacheMin, tmax, tStepNonCartesian, [](float t, void * model, void * context) {
            ContinuousFunction * f = (ContinuousFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c);
          }, f.operator->(), context(), false, f->color());
      } else if (f->hasVerticalLines() && area == ContinuousFunction::AreaType::None) {
        // Optimize plot by only drawing the expected segment.
        float minOrdinate = pixelToFloat(axis, rectMax);
        float maxOrdinate = pixelToFloat(axis, rectMin);
        for (int subCurveIndex = 0; subCurveIndex < f->numberOfSubCurves(); subCurveIndex++) {
          float abscissa = f->evaluateXYAtParameter(0.0, context(), subCurveIndex).x1();
          drawSegment(ctx, rect, abscissa, minOrdinate, abscissa, maxOrdinate, f->color());
        }
      } else {
        // Cartesian.
        // 1 - Define the evaluation functions for curve and area
        // Return nan or inf on booth coordinates to accommodate for any axis.
        bool shouldColorAreaWhenNan = false;
        /* A true shouldColorAreaWhenNan with a xyNan as xyAreaBound plots the
         * area from -inf to inf. We need it for curves such as y^2>-1. */
        Shared::CurveView::EvaluateXYForFloatParameter xyNan =
            [](float t, void *, void *) {
              return Poincare::Coordinate2D<float>(NAN, NAN);
            };
        Shared::CurveView::EvaluateXYForFloatParameter xyInfinite =
            [](float t, void *, void *) {
              return Poincare::Coordinate2D<float>(INFINITY, INFINITY);
            };
        Shared::CurveView::EvaluateXYForFloatParameter xyMinusInfinite =
            [](float t, void *, void *) {
              return Poincare::Coordinate2D<float>(-INFINITY, -INFINITY);
            };
        Shared::CurveView::EvaluateXYForFloatParameter xyAreaBound = nullptr;
        // Evaluations for the first curve
        Shared::CurveView::EvaluateXYForDoubleParameter xyDoubleEvaluation =
            [](double t, void * model, void * context) {
              ContinuousFunction * f = (ContinuousFunction *)model;
              Poincare::Context * c = (Poincare::Context *)context;
              return f->evaluateXYAtParameter(t, c, 0);
            };
        Shared::CurveView::EvaluateXYForFloatParameter xyFloatEvaluation =
            [](float t, void * model, void * context) {
              ContinuousFunction * f = (ContinuousFunction *)model;
              Poincare::Context * c = (Poincare::Context *)context;
              return f->evaluateXYAtParameter(t, c, 0);
            };
        if (area != ContinuousFunction::AreaType::None) {
          if (area == ContinuousFunction::AreaType::Outside) {
            // Either plot the area above the first curve, or everywhere.
            xyAreaBound = hasTwoCurves ? xyInfinite : xyNan;
            /* On equations such as x^2+y^2>1 or y^2>-1, area must be plotted
             * when subcurves evaluate to nan. */
            shouldColorAreaWhenNan = true;
          } else if (area == ContinuousFunction::AreaType::Above) {
            assert(!hasTwoCurves);
            xyAreaBound = xyInfinite;
          } else if (area == ContinuousFunction::AreaType::Below) {
            assert(!hasTwoCurves);
            xyAreaBound = xyMinusInfinite;
          } else if (hasTwoCurves) {
            assert(area == ContinuousFunction::AreaType::Inside);
            // Plot the area inside : Between first and second curve evaluation
            xyAreaBound = [](float t, void * model, void * context) {
              ContinuousFunction * f = (ContinuousFunction *)model;
              Poincare::Context * c = (Poincare::Context *)context;
              return f->evaluateXYAtParameter(t, c, 1);
            };
          }
        }
        bool isIntegral = f->color() && area == ContinuousFunction::AreaType::None;
        // 2 - Draw the first curve
        drawCartesianCurve(ctx, rect, tCacheMin, tmax, xyFloatEvaluation,
                           f.operator->(), context(), f->color(), true,
                           record == m_selectedRecord, f->color(), m_highlightedStart,
                           m_highlightedEnd, xyDoubleEvaluation,
                           f->drawDottedCurve(), xyAreaBound,
                           shouldColorAreaWhenNan, isIntegral ? -1 : 1 << areaIndex, tCacheStep, axis);
        if (hasTwoCurves) {
          /* Evaluations for the second cartesian curve, which is lesser than
           * the first one */
          xyDoubleEvaluation = [](double t, void * model, void * context) {
            ContinuousFunction * f = (ContinuousFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c, 1);
          };
          xyFloatEvaluation = [](float t, void * model, void * context) {
            ContinuousFunction * f = (ContinuousFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c, 1);
          };
          // Reset the area plot constraints
          xyAreaBound = nullptr;
          shouldColorAreaWhenNan = false;
          if (area == ContinuousFunction::AreaType::Outside && hasTwoCurves) {
            // Only case where a second area is to be plot : Below second curve.
            xyAreaBound = xyMinusInfinite;
          }
          // 3 - Draw the second curve
          drawCartesianCurve(
              ctx, rect, tCacheMin, tmax, xyFloatEvaluation, f.operator->(),
              context(), f->color(), true, record == m_selectedRecord,
              f->color(), m_highlightedStart, m_highlightedEnd, xyDoubleEvaluation,
              f->drawDottedCurve(), xyAreaBound, shouldColorAreaWhenNan,
              1 << areaIndex, tCacheStep, axis);
        }
        if (area != ContinuousFunction::AreaType::None) {
          // We can properly display the superposition of up to 4 areas
          areaIndex = (areaIndex + 1) % CurveView::k_numberOfPatternAreas;
        }
        // 4 - Draw tangent
        if (m_tangent && record == m_selectedRecord) {
          assert(f->canDisplayDerivative());
          /* TODO : We could handle tangent on second curve here by finding out
           * which of the two curves is selected. */
          float tangentParameterA = f->approximateDerivative(m_curveViewCursor->x(), context(), 0);
          float tangentParameterB = -tangentParameterA*m_curveViewCursor->x()+f->evaluateXYAtParameter(m_curveViewCursor->x(), context(), 0).x2();
          // To represent the tangent, we draw segment from and to abscissas at the extremity of the drawn rect
          float minAbscissa = pixelToFloat(axis, rectMin);
          float maxAbscissa = pixelToFloat(axis, rectMax);
          drawSegment(ctx, rect, minAbscissa, tangentParameterA*minAbscissa+tangentParameterB, maxAbscissa, tangentParameterA*maxAbscissa+tangentParameterB, Palette::GrayVeryDark, false);
        }
      }
    } else {
      setFunctionInterrupted(i);
      f->tidyDownstreamPoolFrom();
      m_context->tidyDownstreamPoolFrom();
    }
  }
}

bool GraphView::allFunctionsInterrupted(int numberOfFunctions) const {
  /* The number of functions displayed at the same time is theoretically unbounded, but we only store the status of 32 functions. */
  if (numberOfFunctions <= 0 || static_cast<size_t>(numberOfFunctions) > 8 * sizeof(m_functionsInterrupted)) {
    return false;
  }
  return m_functionsInterrupted == static_cast<uint32_t>((1 << numberOfFunctions) - 1);
}

bool GraphView::functionWasInterrupted(int index) const {
  if (index < 0 || static_cast<size_t>(index) >= 8 * sizeof(m_functionsInterrupted)) {
    return false;
  }
  return (1 << index) & m_functionsInterrupted;
}

void GraphView::setFunctionInterrupted(int index) const {
  if (index >= 0 && static_cast<size_t>(index) < 8 * sizeof(m_functionsInterrupted)) {
    m_functionsInterrupted |= 1 << index;
  }
}

}
