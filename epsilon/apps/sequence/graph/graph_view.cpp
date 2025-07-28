#include "graph_view.h"

#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Sequence {

GraphView::GraphView(SequenceStore* sequenceStore,
                     InteractiveCurveViewRange* graphRange,
                     CurveViewCursor* cursor, BannerView* bannerView,
                     CursorView* cursorView)
    : FunctionGraphView(graphRange, cursor, bannerView, cursorView),
      m_sequenceStore(sequenceStore) {}

void GraphView::drawRecord(Ion::Storage::Record record, int index,
                           KDContext* ctx, KDRect rect,
                           bool firstDrawnRecord) const {
  Shared::Sequence* s = m_sequenceStore->modelForRecord(record);

  int xMin = static_cast<int>(std::ceil(range()->xMin())) - 1;
  int xMax = static_cast<int>(std::floor(range()->xMax()));
  int x = xMin;
  while ((x = nextDotIndex(s, x)) <= xMax) {
    float y = s->evaluateXYAtParameter(static_cast<float>(x), context()).y();
    if (std::isnan(y)) {
      continue;
    }
    drawDot(ctx, rect, Dots::Size::Tiny, Coordinate2D<float>(x, y), s->color());
    if (x >= m_highlightedStart && x <= m_highlightedEnd &&
        record == m_selectedRecord) {
      KDColor color = m_shouldColorHighlighted ? s->color() : KDColorBlack;
      drawStraightSegment(ctx, rect, OMG::Axis::Vertical, x, y, 0.f, color);
    }
  }
}

int GraphView::nextDotIndex(Shared::Sequence* sequence, int currentIndex,
                            OMG::HorizontalDirection direction,
                            int scrollSpeed) const {
  assert(scrollSpeed > 0);
  int initialRank = sequence->initialRank();
  if (currentIndex < initialRank) {
    return initialRank;
  }
  int step = static_cast<int>(std::ceil(pixelWidth()));
  assert(step > 0);
  int result = currentIndex;
  if ((result - initialRank) % step != 0) {
    result -= (result - initialRank) % step;
    scrollSpeed -= direction.isLeft();
  }
  result += scrollSpeed * step * (direction.isRight() ? 1 : -1);
  assert((result - initialRank) % step == 0);
  if (result < initialRank) {
    assert(direction.isLeft());
    return initialRank;
  }
  return result;
}

}  // namespace Sequence
