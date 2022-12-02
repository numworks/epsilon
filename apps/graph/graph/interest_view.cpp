#include "interest_view.h"
#include "../app.h"
#include "graph_controller.h"
#include "graph_view.h"
#include <poincare/circuit_breaker_checkpoint.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

void InterestView::drawRect(KDContext * ctx, KDRect rect) const {
  if (!m_parentView->hasFocus()) {
    return;
  }
#if PLATFORM_DEVICE
  bool shouldComputePoints = m_computePoints;
  m_computePoints = false;
#else
  /* Always compute interest when the view is drawn if not on device since
   * there is no slowness issue and it makes the fuzzing more reliable. */
  bool shouldComputePoints = true;
  (void) m_computePoints; // Silence compilator
#endif

  ContinuousFunctionStore * functionStore = App::app()->functionStore();
  Ion::Storage::Record selectedRecord = m_parentView->selectedRecord();
  ExpiringPointer<ContinuousFunction> f = functionStore->modelForRecord(selectedRecord);
  if (!f->properties().isCartesian() || m_parentView->recordWasInterrupted(selectedRecord)) {
    return;
  }

  AbstractPlotView::Axis axis = f->isAlongY() ? AbstractPlotView::Axis::Vertical : AbstractPlotView::Axis::Horizontal;
  PointsOfInterestCache * pointsOfInterestCache = App::app()->graphController()->pointsOfInterestForRecord(m_parentView->selectedRecord());
  PointOfInterest p;
  int i = 0;
  do {
    // Compute more points of interest if necessary
    while (shouldComputePoints && i >= pointsOfInterestCache->numberOfPoints() && !pointsOfInterestCache->isFullyComputed()) {
      /* Use a checkpoint each time a step is computed so that plot can be
       * navigated in parallel of computation. */
      UserCircuitBreakerCheckpoint checkpoint;
      // Clone the cache to prevent modifying the pool before the checkpoint
      PointsOfInterestCache pointsOfInterestCacheClone = pointsOfInterestCache->clone();
      if (AnyKeyCircuitBreakerRun(checkpoint)) {
        pointsOfInterestCacheClone.computeNextStep();
      } else {
        return;
      }
      checkpoint.discard();
      *pointsOfInterestCache = pointsOfInterestCacheClone;
    }

    if (i >= pointsOfInterestCache->numberOfPoints()) {
      return;
    }

    p = pointsOfInterestCache->pointAtIndex(i);
    bool wasAlreadyDrawn = i < m_nextPointIndex;
    i++;
    if (!wasAlreadyDrawn) {
      m_nextPointIndex = i;
    }

    if (m_interest != Poincare::Solver<double>::Interest::None && m_interest != p.interest()) {
      continue;
    }

    // Draw the dot
    Coordinate2D<float> dotCoordinates = axis == AbstractPlotView::Axis::Horizontal ? static_cast<Coordinate2D<float>>(p.xy()) : Coordinate2D<float>(p.y(), p.x());
    KDRect dotRect = m_parentView->dotRect(k_dotsSize, dotCoordinates);
    // If the dot interescts the parent dirty rect, force the redraw
    if (!dotRect.intersects(m_parentView->dirtyRect()) && wasAlreadyDrawn) {
      continue;
    }
    // If the dot is below the cursor, erase the cursor and redraw it
    MemoizedCursorView * cursor = static_cast<MemoizedCursorView *>(m_parentView->cursorView());
    KDRect cursorFrame = cursor ? cursor->frame() : KDRectZero;
    bool redrawCursor = cursorFrame.intersects(dotRect);
    if (redrawCursor) {
      // Erase cursor and make rect dirty
      assert(cursor);
      cursor->setCursorFrame(cursorFrame, true);
    }
    m_parentView->drawDot(ctx, rect, k_dotsSize, dotCoordinates, Escher::Palette::GrayDarkest);
    if (redrawCursor) {
      assert(cursor);
      cursor->redrawCursor(rect);
    }

  } while (1);
}

void InterestView::resumePointsOfInterestDrawing() {
#if PLATFORM_DEVICE
  markRectAsDirty(bounds());
  m_computePoints = true;
#endif
}

}
