#include "horizontal_or_vertical_layout.h"

#include <algorithm>

using namespace Probability;

KDSize OrientedLayout::minimalSizeForOptimalDisplay() const {
  int requiredFirst = 0, requiredSecond = 2 * secondMargin();
  KDCoordinate proposedFirst = firstLength(bounds().size()) - 2 * firstMargin();
  int n = numberOfSubviews();
  for (int i = 0; i < n; i++) {
    Escher::View * subview = const_cast<OrientedLayout *>(this)->subviewAtIndex(i);
    subview->setSize(reorderedSize(proposedFirst, secondLength(subview->bounds().size())));
    KDSize subviewSize = subview->minimalSizeForOptimalDisplay();
    requiredSecond += secondLength(subviewSize);
    requiredFirst = std::max<int>(requiredFirst, firstLength(subviewSize));
  }
  return reorderedSize(requiredFirst + 2 * firstMargin(), requiredSecond);
}

void OrientedLayout::layoutSubviews(bool force) {
  KDSize boundsSize = bounds().size();
  KDCoordinate availableFirst = firstLength(boundsSize) - 2 * firstMargin();
  KDCoordinate availableSecond = secondLength(boundsSize) - 2 * secondMargin();
  KDCoordinate offsetSecond = secondMargin();
  int n = numberOfSubviews();
  for (int i = 0; i < n; i++) {
    Escher::View * subview = subviewAtIndex(i);
    subview->setSize(reorderedSize(availableFirst, secondLength(subview->bounds().size())));
    KDCoordinate requiredSecond = secondLength(subviewAtIndex(i)->minimalSizeForOptimalDisplay());
    KDRect proposedFrame = reorderedRect(
        KDRect(KDPoint(firstMargin(), offsetSecond), availableFirst, requiredSecond));
    subview->setFrame(proposedFrame, false);
    availableSecond -= requiredSecond;
    offsetSecond += requiredSecond;
    assert(availableSecond >= 0);
  }
}

void Probability::OrientedLayout::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw in margins
  ctx->fillRect(KDRect(0, 0, m_marginX, bounds().height()), m_backgroundColor);
  ctx->fillRect(KDRect(bounds().width() - m_marginX, 0, m_marginX, bounds().height()),
                m_backgroundColor);
  ctx->fillRect(KDRect(m_marginX, 0, bounds().width() - 2 * m_marginX, m_marginY),
                m_backgroundColor);
  ctx->fillRect(
      KDRect(m_marginX, bounds().height() - m_marginY, bounds().width() - 2 * m_marginX, m_marginY),
      m_backgroundColor);
}

KDCoordinate Probability::OrientedLayout::firstMargin() const {
  return reorderedPoint(m_marginX, m_marginY).x();
}

KDCoordinate Probability::OrientedLayout::secondMargin() const {
  return reorderedPoint(m_marginX, m_marginY).y();
}

KDPoint Probability::OrientedLayout::reorderedPoint(KDCoordinate first, KDCoordinate second) const {
  KDSize s = reorderedSize(first, second);
  return KDPoint(s.width(), s.height());
}

KDRect Probability::OrientedLayout::reorderedRect(KDRect rect) const {
  return KDRect(reorderedPoint(rect.x(), rect.y()), reorderedSize(rect.width(), rect.height()));
}
