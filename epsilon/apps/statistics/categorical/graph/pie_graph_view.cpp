#include "pie_graph_view.h"

#include <cmath>

namespace Statistics::Categorical {

PieGraphViewDataSource::PieGraphViewDataSource(Store* store)
    : m_store(store),
      m_numberOfActiveCategories(0),
      m_isSelectionActive(true) {}

void PieGraphViewDataSource::toggleSelection(bool select) {
  if (m_isSelectionActive != select) {
    m_isSelectionActive = select;
    if (m_isSelectionActive) {
      m_selectedCategoryOriginalColor = m_insideColors[m_selectedCategory];
      m_insideColors[m_selectedCategory] = k_selectedColor;
    } else {
      m_insideColors[m_selectedCategory] = m_selectedCategoryOriginalColor;
    }
  }
}

int PieGraphViewDataSource::nextCategory(int direction) {
  assert(m_isSelectionActive);  // Cannot change category if not selected
  m_insideColors[m_selectedCategory] = m_selectedCategoryOriginalColor;
  if (direction < 0 && m_selectedCategory == 0) {
    m_selectedCategory = m_numberOfActiveCategories - 1;
  } else {
    m_selectedCategory += direction < 0 ? -1 : 1;
    m_selectedCategory %= m_numberOfActiveCategories;
  }
  m_selectedCategoryOriginalColor = m_insideColors[m_selectedCategory];
  m_insideColors[m_selectedCategory] = k_selectedColor;
  return m_toGlobalCategories[m_selectedCategory];
}

int PieGraphViewDataSource::setGroup(int group) {
  m_numberOfActiveCategories = 0;
  m_selectedCategory = UINT8_MAX;
  float cumulatedAngle = 0;
  static_assert(Store::k_maxNumberOfCategory <=
                Escher::Palette::numberOfLightDataColors());
  static_assert(Store::k_maxNumberOfCategory <=
                Escher::Palette::numberOfDataColors());
  for (int globalCatIndex = 0; globalCatIndex < Store::k_maxNumberOfCategory;
       ++globalCatIndex) {
    float rf = m_store->getRelativeFrequency(group, globalCatIndex);
    if (std::isnan(rf)) {
      continue;
    }
    int localCatIndex = m_numberOfActiveCategories;
    m_insideColors[localCatIndex] =
        Escher::Palette::DataColorLight[globalCatIndex];
    m_borderColors[localCatIndex] = Escher::Palette::DataColor[globalCatIndex];
    m_toGlobalCategories[localCatIndex] = globalCatIndex;
    cumulatedAngle += 2 * M_PI * rf;
    m_cumulatedAngles[localCatIndex] = cumulatedAngle;
    if (m_selectedCategory == UINT8_MAX) {
      // Set the selectedCategory to the first active category
      m_selectedCategory = localCatIndex;
      if (m_isSelectionActive) {
        m_selectedCategoryOriginalColor = m_insideColors[localCatIndex];
        m_insideColors[localCatIndex] = k_selectedColor;
      }
    }
    ++m_numberOfActiveCategories;
  }
  return m_toGlobalCategories[m_selectedCategory];
}

void PieGraphView::drawRect(KDContext* ctx, KDRect rect) const {
  /* TODO: improve this code to only redraw dirty areas.
   * See flicker in N0110/N0115 */
  assert(m_numberOfActiveCategories > 0);
  constexpr int k_framebufferHeight = 149;
  assert(bounds().width() % k_framebufferWidth == 0);
  assert(bounds().height() == k_framebufferHeight);

  KDColor framebuffer[k_framebufferHeight][k_framebufferWidth];

  const KDPoint center = KDPoint(bounds().width() / 2, bounds().height() / 2);

  for (int layerStart = bounds().width() - k_framebufferWidth; layerStart >= 0;
       layerStart -= k_framebufferWidth) {
    KDColor* pixels = &framebuffer[0][0];
    for (int j = 0; j < k_framebufferHeight; ++j) {
      for (int i = 0; i < k_framebufferWidth; ++i) {
        *(pixels++) = pointColor(i + layerStart, j, center);
      }
    }
    KDRect layerRect =
        KDRect(layerStart, 0, k_framebufferWidth, k_framebufferHeight);
    ctx->fillRectWithPixels(layerRect, &framebuffer[0][0], &framebuffer[0][0]);
  }
  if (m_numberOfActiveCategories > 1) {
    /* NOTE: Drawing the anti-aliased separation lines in reverse order
     * because it looks better.
     * Otherwise the vertical line separating category 0 and N-1 is too
     * dominant in the center */
    for (int i = m_numberOfActiveCategories - 1; i >= 0; --i) {
      float sin = std::sin(m_cumulatedAngles[i]);
      float cos = std::cos(m_cumulatedAngles[i]);
      float edgeX = center.x() + (k_fradius - .7) * sin,
            edgeY = center.y() - (k_fradius - .7) * cos;
      KDColor blend = KDColor::Blend(
          m_borderColors[i],
          m_borderColors[(i + 1) % m_numberOfActiveCategories], 128);
      ctx->drawAntialiasedLineAutoBackground(center.x(), center.y(), edgeX,
                                             edgeY, blend);
    }
  }
}

static float Norm(float centeredX, float centeredY) {
  return std::sqrt(centeredX * centeredX + centeredY * centeredY);
}

KDColor PieGraphView::pointColor(KDCoordinate x, KDCoordinate y,
                                 KDPoint center) const {
  KDCoordinate centeredX = x - center.x();
  KDCoordinate centeredY =
      center.y() - y;  // inverted because Y grows downwards on the screen
  float distanceToCenter = Norm(centeredX, centeredY);
  // Negative means inside
  float distanceToRing = distanceToCenter - k_fradius;
  if (distanceToRing >= k_border) {
    // Outside the disk and outside anti-aliased border range
    return k_outsideColor;
  }
  float isCloseToRing = std::fabs(distanceToRing) < k_border;

  // Clockwise angle: 0° is Up, 90° is Right
  float angle =
      std::atan2(static_cast<float>(centeredX), static_cast<float>(centeredY));
  angle = (angle < 0) ? angle + 2 * M_PI : angle;

  for (int i = 0; i < m_numberOfActiveCategories; ++i) {
    if (angle < m_cumulatedAngles[i]) {
      // In the i-th section
      if (!isCloseToRing) {
        // Fully inside the disk
        return m_insideColors[i];
      }
      /* Close to the anti-aliased border.
       * Depending on sign, blending with inside or outside color */
      uint8_t alpha = std::fabs(distanceToRing) / k_border * UINT8_MAX;
      return KDColor::Blend(
          distanceToRing < 0 ? m_insideColors[i] : k_outsideColor,
          m_borderColors[i], alpha);
    }
  }
  OMG::unreachable();
}

}  // namespace Statistics::Categorical
