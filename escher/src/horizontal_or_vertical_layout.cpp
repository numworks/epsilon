#include <escher/horizontal_or_vertical_layout.h>
#include <algorithm>

using namespace Escher;

KDSize OrientedLayout::minimalSizeForOptimalDisplay() const {
  int requiredSecondaryDirectionLength = 0, requiredMainDirectionLength = 0;
  KDCoordinate proposedSecondaryDirection = secondaryDirectionLength(bounds().size()) -
                                            2 * secondaryDirectionMargin();
  int n = numberOfSubviews();
  for (int i = 0; i < n; i++) {
    View * subview = const_cast<OrientedLayout *>(this)->subviewAtIndex(i);
    subview->setSize(
        sizeFromMainAndSecondaryDirection(mainDirectionLength(subview->bounds().size()), proposedSecondaryDirection));
    KDSize subviewSize = subview->minimalSizeForOptimalDisplay();
    requiredMainDirectionLength += mainDirectionLength(subviewSize);
    requiredSecondaryDirectionLength = std::max<int>(requiredSecondaryDirectionLength,
                                                     secondaryDirectionLength(subviewSize));
  }
  // Add margins
  requiredMainDirectionLength += 2 * mainDirectionMargin();
  requiredSecondaryDirectionLength += 2 * secondaryDirectionMargin();
  return sizeFromMainAndSecondaryDirection(requiredMainDirectionLength, requiredSecondaryDirectionLength);
}

void OrientedLayout::layoutSubviews(bool force) {
  const KDSize boundsSize = bounds().size();
  const KDCoordinate availableSecondaryDirection = secondaryDirectionLength(boundsSize) -
                                             2 * secondaryDirectionMargin();
  const KDCoordinate availableMainDirection = mainDirectionLength(boundsSize) - 2 * mainDirectionMargin();
  KDCoordinate offsetMainDirection = mainDirectionMargin();
  int n = numberOfSubviews();
  for (int i = 0; i < n; i++) {
    View * subview = subviewAtIndex(i);
    /* Temporarily give subview all available space in secondary direction
     * before calling minimalSizeForOptimalDisplay. */
    subview->setSize(
        sizeFromMainAndSecondaryDirection(mainDirectionLength(subview->bounds().size()), availableSecondaryDirection));
    const KDCoordinate requiredMainDirectionLength = mainDirectionLength(
        subview->minimalSizeForOptimalDisplay());
    assert(availableMainDirection >= offsetMainDirection + requiredMainDirectionLength);
    const KDRect proposedFrame = rectFromMainAndSecondaryDirection(
        KDRect(KDPoint(offsetMainDirection, secondaryDirectionMargin()),
               requiredMainDirectionLength,
               availableSecondaryDirection));
    subview->setFrame(proposedFrame, false);
    offsetMainDirection += requiredMainDirectionLength;
  }
}

void OrientedLayout::drawRect(KDContext * ctx, KDRect rect) const {
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

KDCoordinate OrientedLayout::secondaryDirectionMargin() const {
  return pointFromMainAndSecondaryDirection(m_mainDirectionMargin, m_secondaryDirectionMargin).x();
}

KDCoordinate OrientedLayout::mainDirectionMargin() const {
  return pointFromMainAndSecondaryDirection(m_mainDirectionMargin, m_secondaryDirectionMargin).y();
}

KDPoint OrientedLayout::pointFromMainAndSecondaryDirection(KDCoordinate mainDirection,
                                                    KDCoordinate secondaryDirection) const {
  KDSize s = sizeFromMainAndSecondaryDirection(mainDirection, secondaryDirection);
  return KDPoint(s.width(), s.height());
}

KDRect OrientedLayout::rectFromMainAndSecondaryDirection(KDRect rect) const {
  return KDRect(pointFromMainAndSecondaryDirection(rect.x(), rect.y()), sizeFromMainAndSecondaryDirection(rect.width(), rect.height()));
}
