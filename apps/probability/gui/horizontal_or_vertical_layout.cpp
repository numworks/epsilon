#include "horizontal_or_vertical_layout.h"

#include <algorithm>

using namespace Probability;

KDSize OrientedLayout::minimalSizeForOptimalDisplay() const {
  int requiredSecondaryDirectionLength = 0, requiredMainDirectionLength = 2 * mainDirectionMargin();
  KDCoordinate proposedSecondaryDirection = secondaryDirectionLength(bounds().size()) -
                                            2 * secondaryDirectionMargin();
  int n = numberOfSubviews();
  for (int i = 0; i < n; i++) {
    Escher::View * subview = const_cast<OrientedLayout *>(this)->subviewAtIndex(i);
    subview->setSize(
        reorderedSize(mainDirectionLength(subview->bounds().size()), proposedSecondaryDirection));
    KDSize subviewSize = subview->minimalSizeForOptimalDisplay();
    requiredMainDirectionLength += mainDirectionLength(subviewSize);
    requiredSecondaryDirectionLength = std::max<int>(requiredSecondaryDirectionLength,
                                                     secondaryDirectionLength(subviewSize));
  }
  return reorderedSize(requiredMainDirectionLength,
                       requiredSecondaryDirectionLength + 2 * secondaryDirectionMargin());
}

void OrientedLayout::layoutSubviews(bool force) {
  KDSize boundsSize = bounds().size();
  KDCoordinate availableSecondaryDirection = secondaryDirectionLength(boundsSize) -
                                             2 * secondaryDirectionMargin();
  KDCoordinate availableMainDirection = mainDirectionLength(boundsSize) - 2 * mainDirectionMargin();
  KDCoordinate offsetMainDirection = mainDirectionMargin();
  int n = numberOfSubviews();
  for (int i = 0; i < n; i++) {
    Escher::View * subview = subviewAtIndex(i);
    subview->setSize(
        reorderedSize(mainDirectionLength(subview->bounds().size()), availableSecondaryDirection));
    KDCoordinate requiredMainDirectionLength = mainDirectionLength(
        subviewAtIndex(i)->minimalSizeForOptimalDisplay());
    KDRect proposedFrame = reorderedRect(
        KDRect(KDPoint(offsetMainDirection, secondaryDirectionMargin()),
               requiredMainDirectionLength,
               availableSecondaryDirection));
    subview->setFrame(proposedFrame, false);
    availableMainDirection -= requiredMainDirectionLength;
    offsetMainDirection += requiredMainDirectionLength;
    assert(availableMainDirection >= 0);
  }
}

void Probability::OrientedLayout::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw in margins
  ctx->fillRect(KDRect(0, 0, m_secondaryDirectionMargin, bounds().height()), m_backgroundColor);
  ctx->fillRect(KDRect(bounds().width() - m_secondaryDirectionMargin,
                       0,
                       m_secondaryDirectionMargin,
                       bounds().height()),
                m_backgroundColor);
  ctx->fillRect(KDRect(m_secondaryDirectionMargin,
                       0,
                       bounds().width() - 2 * m_secondaryDirectionMargin,
                       m_mainDirectionMargin),
                m_backgroundColor);
  ctx->fillRect(KDRect(m_secondaryDirectionMargin,
                       bounds().height() - m_mainDirectionMargin,
                       bounds().width() - 2 * m_secondaryDirectionMargin,
                       m_mainDirectionMargin),
                m_backgroundColor);
}

KDCoordinate Probability::OrientedLayout::secondaryDirectionMargin() const {
  return reorderedPoint(m_mainDirectionMargin, m_secondaryDirectionMargin).x();
}

KDCoordinate Probability::OrientedLayout::mainDirectionMargin() const {
  return reorderedPoint(m_mainDirectionMargin, m_secondaryDirectionMargin).y();
}

KDPoint Probability::OrientedLayout::reorderedPoint(KDCoordinate mainDirection,
                                                    KDCoordinate secondaryDirection) const {
  KDSize s = reorderedSize(mainDirection, secondaryDirection);
  return KDPoint(s.width(), s.height());
}

KDRect Probability::OrientedLayout::reorderedRect(KDRect rect) const {
  return KDRect(reorderedPoint(rect.x(), rect.y()), reorderedSize(rect.width(), rect.height()));
}
