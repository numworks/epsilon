#include <escher/horizontal_or_vertical_layout.h>

#include <algorithm>

using namespace Escher;

KDSize OrientedLayout::minimalSizeForOptimalDisplay() const {
  int requiredSecondaryDirectionLength = 0, requiredMainDirectionLength = 0;
  int n = numberOfSubviews();
  for (int i = 0; i < n; i++) {
    View *subview = const_cast<OrientedLayout *>(this)->subviewAtIndex(i);
    KDSize subviewSize = adaptSize(subview->minimalSizeForOptimalDisplay());
    requiredMainDirectionLength += subviewSize.height();
    requiredSecondaryDirectionLength =
        std::max<int>(requiredSecondaryDirectionLength, subviewSize.width());
  }
  // Add margins
  requiredMainDirectionLength += 2 * m_mainDirectionMargin;
  requiredSecondaryDirectionLength += 2 * m_secondaryDirectionMargin;
  return adaptSize(
      KDSize(requiredSecondaryDirectionLength, requiredMainDirectionLength));
}

void OrientedLayout::layoutSubviews(bool force) {
  const KDSize boundsSize = adaptSize(bounds().size());
  const KDCoordinate availableSecondaryDirection =
      boundsSize.width() - 2 * m_secondaryDirectionMargin;
  const KDCoordinate availableMainDirection =
      boundsSize.height() - 2 * m_mainDirectionMargin;
  KDCoordinate offsetMainDirection = m_mainDirectionMargin;
  int n = numberOfSubviews();
  for (int i = 0; i < n; i++) {
    View *subview = subviewAtIndex(i);
    const KDCoordinate requiredMainDirectionLength =
        adaptSize(subview->minimalSizeForOptimalDisplay()).height();
    assert(availableMainDirection >=
           offsetMainDirection + requiredMainDirectionLength);
    // Silent unused variable warning
    (void)availableMainDirection;
    const KDRect suggestedFrame =
        KDRect(KDPoint(m_secondaryDirectionMargin, offsetMainDirection),
               availableSecondaryDirection, requiredMainDirectionLength);
    setChildFrame(subview, adaptRect(suggestedFrame), false);
    offsetMainDirection += requiredMainDirectionLength;
  }
}

void OrientedLayout::drawRect(KDContext *ctx, KDRect rect) const {
  // Draw in margins
  const KDSize boundsSize = adaptSize(bounds().size());
  // Left margin
  ctx->fillRect(
      adaptRect(KDRect(0, 0, m_secondaryDirectionMargin, boundsSize.height())),
      m_backgroundColor);
  // Right margin
  ctx->fillRect(
      adaptRect(KDRect(boundsSize.width() - m_secondaryDirectionMargin, 0,
                       m_secondaryDirectionMargin, boundsSize.height())),
      m_backgroundColor);
  // Top margin
  ctx->fillRect(
      adaptRect(KDRect(m_secondaryDirectionMargin, 0,
                       boundsSize.width() - 2 * m_secondaryDirectionMargin,
                       m_mainDirectionMargin)),
      m_backgroundColor);
  // Bottom margin
  ctx->fillRect(
      adaptRect(KDRect(m_secondaryDirectionMargin,
                       boundsSize.height() - m_mainDirectionMargin,
                       boundsSize.width() - 2 * m_secondaryDirectionMargin,
                       m_mainDirectionMargin)),
      m_backgroundColor);
}
