#include "interest_view.h"

#include "graph_view.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

void InterestView::drawRect(KDContext* ctx, KDRect rect) const {
  m_parentView->drawPointsOfInterest(ctx, rect);
}

}  // namespace Graph
