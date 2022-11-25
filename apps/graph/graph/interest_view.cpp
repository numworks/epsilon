#include "interest_view.h"
#include "../app.h"
#include "graph_controller.h"
#include "graph_view.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

void InterestView::drawRect(KDContext * ctx, KDRect rect) const {
  if (!m_parentView->hasFocus()) {
    return;
  }
  ContinuousFunctionStore * functionStore = App::app()->functionStore();
  ExpiringPointer<ContinuousFunction> f = functionStore->modelForRecord(m_parentView->selectedRecord());
  if (!f->properties().isCartesian()) {
    return;
  }

  AbstractPlotView::Axis axis = f->isAlongY() ? AbstractPlotView::Axis::Vertical : AbstractPlotView::Axis::Horizontal;
  PointsOfInterestCache * pointsOfInterest = App::app()->graphController()->pointsOfInterest();
  PointOfInterest p = pointsOfInterest->computePointAtIndex(0);
  int i = 0;
  while (!p.isUninitialized()) {
    if (m_interest == Poincare::Solver<double>::Interest::None || m_interest == p.interest()) {
      Coordinate2D<float> xy = axis == AbstractPlotView::Axis::Horizontal ? static_cast<Coordinate2D<float>>(p.xy()) : Coordinate2D<float>(p.y(), p.x());
      m_parentView->drawDot(ctx, rect, Dots::Size::Tiny, xy, Escher::Palette::GrayDarkest);
    }
    p = pointsOfInterest->computePointAtIndex(++i);
  }
}

}