#include "vertical_layout.h"

using namespace Probability;

void VerticalLayout::layoutSubviews(bool force) {
  KDRect frame = bounds();
  KDCoordinate availableHeight = frame.height();
  KDRect proposedFrame = KDRect(0, 0, frame.width(), 0);
  int height;
  for (int i = 0; i < numberOfSubviews(); i++) {
    subviewAtIndex(i)->setFrame(
        KDRect(0, proposedFrame.y() + proposedFrame.height(), frame.width(), availableHeight),
        false);
    height = subviewAtIndex(i)->minimalSizeForOptimalDisplay().height();
    proposedFrame = KDRect(0, proposedFrame.y() + proposedFrame.height(), frame.width(), height);
    subviewAtIndex(i)->setFrame(proposedFrame, false);
    availableHeight -= height;
    if (availableHeight < 0) {
      assert(false);
      availableHeight = 0;
    }
  }
}

void VerticalLayout::drawRect(KDContext * ctx, KDRect rectToRedraw) const {
  ctx->fillRect(rectToRedraw, m_backgroundColor);
}
