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
    ExpiringPointer<NewFunction> f = functionStore->modelForRecord(record);
    Poincare::UserCircuitBreakerCheckpoint checkpoint;
    if (CircuitBreakerRun(checkpoint)) {
      // TODO Hugo : Restore cache
      // ContinuousFunctionCache * cch = functionStore->cacheAtIndex(i);
      NewFunction::PlotType type = f->plotType();
      Poincare::Expression e = f->expressionReduced(context());
      // TODO Hugo ::fix it
      if (e.isUndefined() || (
            (type == NewFunction::PlotType::Parametric || f->hasTwoCurves()) &&
            e.childAtIndex(0).isUndefined() &&
            e.childAtIndex(1).isUndefined())) {
        continue;
      }
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
        // TODO Hugo : Properly compute tCacheStep and tStepNonCartesian
        tStepNonCartesian = (tmax - tmin) / 10.0938275501223f;
        // ContinuousFunctionCache::ComputeNonCartesianSteps(&tStepNonCartesian, &tCacheStep, tmax, tmin);
      }
      // ContinuousFunctionCache::PrepareForCaching(f.operator->(), cch, tCacheMin, tCacheStep);

      if (type == NewFunction::PlotType::Polar) {
        // Polar
        drawPolarCurve(ctx, rect, tmin, tmax, tStepNonCartesian, [](float t, void * model, void * context) {
            NewFunction * f = (NewFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c);
          }, f.operator->(), context(), false, f->color());
      } else if (type == NewFunction::PlotType::Parametric) {
        // Parametric
        drawCurve(ctx, rect, tmin, tmax, tStepNonCartesian, [](float t, void * model, void * context) {
            NewFunction * f = (NewFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c);
          }, f.operator->(), context(), false, f->color());
      } else {
        // Cartesian.
        bool superiorArea = f->drawSuperiorArea();
        bool inferiorArea = f->drawInferiorArea();
        assert(!(superiorArea && inferiorArea));
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
              NewFunction * f = (NewFunction *)model;
              Poincare::Context * c = (Poincare::Context *)context;
              return f->evaluateXYAtParameter(t, c, 0);
            };
        Shared::CurveView::EvaluateXYForFloatParameter xyFloatEvaluation =
            [](float t, void * model, void * context) {
              NewFunction * f = (NewFunction *)model;
              Poincare::Context * c = (Poincare::Context *)context;
              return f->evaluateXYAtParameter(t, c, 0);
            };
        if (superiorArea || inferiorArea) {
          // The drawn area goes from the xyFloatEvaluation to xyAreaBound.
          if (superiorArea) {
            // Superior area ends at +inf
            xyAreaBound = xyInfinite;
          } else if (!f->hasTwoCurves()) {
            // Inferior area ends at -inf
            xyAreaBound = xyMinusInfinite;
          } else {
            // Inferior area ends at the second curve
            xyAreaBound = [](float t, void * model, void * context) {
              NewFunction * f = (NewFunction *)model;
              Poincare::Context * c = (Poincare::Context *)context;
              return f->evaluateXYAtParameter(t, c, 1);
            };
          }
        }
        drawCartesianCurve(ctx, rect, tmin, tmax, xyFloatEvaluation,
                           f.operator->(), context(), f->color(), true,
                           record == m_selectedRecord, m_highlightedStart,
                           m_highlightedEnd, xyDoubleEvaluation,
                           !f->drawCurve(), xyAreaBound, false, areaIndex);
        if (f->hasTwoCurves()) {
          // Draw the second cartesian curve, which is lesser than the first
          xyDoubleEvaluation = [](double t, void * model, void * context) {
            NewFunction * f = (NewFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c, 1);
          };
          xyFloatEvaluation = [](float t, void * model, void * context) {
            NewFunction * f = (NewFunction *)model;
            Poincare::Context * c = (Poincare::Context *)context;
            return f->evaluateXYAtParameter(t, c, 1);
          };
          xyAreaBound = nullptr;
          if (superiorArea) {
            // Inferior area ends at -inf
            xyAreaBound = xyMinusInfinite;
          }
          drawCartesianCurve(
              ctx, rect, tmin, tmax, xyFloatEvaluation, f.operator->(),
              context(), f->color(), true, record == m_selectedRecord,
              m_highlightedStart, m_highlightedEnd, xyDoubleEvaluation,
              !f->drawCurve(), xyAreaBound, true, areaIndex);
        }
        if (superiorArea || inferiorArea) {
          // We display the superposition of up to 4 areas
          areaIndex++;
        }
        // TODO Hugo : Draw vertical line with drawSegment ?
        /* Draw tangent */
        if (m_tangent && record == m_selectedRecord) {
          // TODO Hugo : Use right cursor
          float tangentParameterA = f->approximateDerivative(m_curveViewCursor->x(), context());
          float tangentParameterB = -tangentParameterA*m_curveViewCursor->x()+f->evaluateXYAtParameter(m_curveViewCursor->x(), context()).x2();
          // To represent the tangent, we draw segment from and to abscissas at the extremity of the drawn rect
          float minAbscissa = pixelToFloat(Axis::Horizontal, rect.left());
          float maxAbscissa = pixelToFloat(Axis::Horizontal, rect.right());
          drawSegment(ctx, rect, minAbscissa, tangentParameterA*minAbscissa+tangentParameterB, maxAbscissa, tangentParameterA*maxAbscissa+tangentParameterB, Palette::GrayVeryDark, false);
        }
      }
    } else {
      setFunctionInterrupted(i);
      f->tidy();
      m_context->tidy();
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
