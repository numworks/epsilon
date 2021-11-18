#include "graph_view.h"
#include "../app.h"
#include <assert.h>
#include <algorithm>
#include <poincare/circuit_breaker_checkpoint.h>

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
      ContinuousFunctionCache * cch = functionStore->cacheAtIndex(i);
      ContinuousFunction::PlotType type = f->plotType();
      Poincare::Expression e = f->expressionReduced(context());
      if (e.isUndefined() || (
            (type == ContinuousFunction::PlotType::Parametric || f->numberOfSubCurves() == 2) &&
            e.childAtIndex(0).isUndefined() &&
            e.childAtIndex(1).isUndefined())) {
        continue;
      }
      assert(f->numberOfSubCurves() <= 2);
      float tmin = f->tMin();
      float tmax = f->tMax();

      float tCacheMin, tCacheStep, tStepNonCartesian;
      if (f->isAlongX()) {
        float rectLeft = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
        /* Here, tCacheMin can depend on rect (and change as the user move)
         * because cache can be panned for cartesian curves, instead of being
         * entirely invalidated. */
        tCacheMin = std::isnan(rectLeft) ? tmin : std::max(tmin, rectLeft);
        tCacheStep = pixelWidth();
      } else {
        tCacheMin = tmin;
        // Compute tCacheStep and tStepNonCartesian
        ContinuousFunctionCache::ComputeNonCartesianSteps(&tStepNonCartesian, &tCacheStep, tmax, tmin);
      }
      ContinuousFunctionCache::PrepareForCaching(f.operator->(), cch, tCacheMin, tCacheStep);

      if (type == ContinuousFunction::PlotType::Polar) {
        // Polar
        drawPolarCurve(ctx, rect, tCacheMin, tmax, tStepNonCartesian, [](float t, void * model, void * context) {
            ContinuousFunction * f = (ContinuousFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c);
          }, f.operator->(), context(), false, f->color());
      } else if (type == ContinuousFunction::PlotType::Parametric) {
        // Parametric
        drawCurve(ctx, rect, tCacheMin, tmax, tStepNonCartesian, [](float t, void * model, void * context) {
            ContinuousFunction * f = (ContinuousFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c);
          }, f.operator->(), context(), false, f->color());
      } else if (type == ContinuousFunction::PlotType::VerticalLine || type == ContinuousFunction::PlotType::VerticalLines) {
        for (int subCurveIndex = 0; subCurveIndex < f->numberOfSubCurves(); subCurveIndex++) {
          float abscissa = f->evaluateXYAtParameter(0.0, context(), subCurveIndex).x1();
          float minOrdinate = pixelToFloat(Axis::Vertical, rect.top());
          float maxOrdinate = pixelToFloat(Axis::Vertical, rect.bottom());
          drawSegment(ctx, rect, abscissa, minOrdinate, abscissa, maxOrdinate, f->color(), false);
        }
      } else {
        // Cartesian.
        // 1 - Define the evaluation functions for curve and area
        ContinuousFunction::AreaType area = f->areaType();
        Shared::CurveView::EvaluateXYForFloatParameter xyInfinite =
            [](float t, void *, void *) {
              return Poincare::Coordinate2D<float>(t, INFINITY);
            };
        Shared::CurveView::EvaluateXYForFloatParameter xyMinusInfinite =
            [](float t, void *, void *) {
              return Poincare::Coordinate2D<float>(t, -INFINITY);
            };
        Shared::CurveView::EvaluateXYForFloatParameter xyAreaBound = nullptr;
        // Draw the first cartesian curve
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
          /* The drawn area goes from the xyFloatEvaluation to xyAreaBound.
           * With a single curve, the drawn area is either above or below the
           * curve. With two curves, the equation was of the form "a*y^2_(...)"
           * with a strictly positive and _ being either <, <=, > or >=.
           * With an inferior comparison, the area to color is between both
           * curves. With a superior comparison, the area is the opposite : both
           * above the first curve and under the second curve. */
          if (area == ContinuousFunction::AreaType::Superior) {
            // Superior area ends at +inf
            xyAreaBound = xyInfinite;
          } else if (f->numberOfSubCurves() == 1) {
            // Inferior area ends at -inf
            xyAreaBound = xyMinusInfinite;
          } else {
            // Inferior area ends at the second curve
            xyAreaBound = [](float t, void * model, void * context) {
              ContinuousFunction * f = (ContinuousFunction *)model;
              Poincare::Context * c = (Poincare::Context *)context;
              return f->evaluateXYAtParameter(t, c, 1);
            };
          }
        }
        // 2 - Draw the first curve
        drawCartesianCurve(
            ctx, rect, tCacheMin, tmax, xyFloatEvaluation, f.operator->(),
            context(), f->color(), true, record == m_selectedRecord,
            m_highlightedStart, m_highlightedEnd, xyDoubleEvaluation,
            f->drawDottedCurve(), xyAreaBound, false, areaIndex, tCacheStep);
        if (f->numberOfSubCurves() == 2) {
          // Draw the second cartesian curve, which is lesser than the first
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
          xyAreaBound = nullptr;
          if (area == ContinuousFunction::AreaType::Superior) {
            // Superior area starts from the second curve and ends at -inf
            xyAreaBound = xyMinusInfinite;
            /* Note : If the second curve evaluation is NAN, the
             * superior area must stretch from -inf to inf. Inequations are
             * currently supported only between polynoms of x and y. Therfore, a
             * NAN second curve evaluation means the first curve evaluated to
             * NAN as well, and there is no need to check that the equation is
             * defined at this value of x. */
          }
          // 3 - Draw the second curve
          drawCartesianCurve(
              ctx, rect, tCacheMin, tmax, xyFloatEvaluation, f.operator->(),
              context(), f->color(), true, record == m_selectedRecord,
              m_highlightedStart, m_highlightedEnd, xyDoubleEvaluation,
              f->drawDottedCurve(), xyAreaBound, true, areaIndex, tCacheStep);
        }
        if (area != ContinuousFunction::AreaType::None) {
          // We can properly display the superposition of up to 4 areas
          areaIndex++;
        }
        // 4 - Draw tangent
        if (m_tangent && record == m_selectedRecord) {
          assert(f->numberOfSubCurves() == 1);
          /* TODO : We could handle tangent on second curve here by finding out
           * which of the two curves is selected. */
          float tangentParameterA = f->approximateDerivative(m_curveViewCursor->x(), context(), 0);
          float tangentParameterB = -tangentParameterA*m_curveViewCursor->x()+f->evaluateXYAtParameter(m_curveViewCursor->x(), context(), 0).x2();
          // To represent the tangent, we draw segment from and to abscissas at the extremity of the drawn rect
          float minAbscissa = pixelToFloat(Axis::Horizontal, rect.left());
          float maxAbscissa = pixelToFloat(Axis::Horizontal, rect.right());
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
