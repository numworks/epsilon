#pragma once

#include <apps/i18n.h>
#include <escher/palette.h>
#include <escher/view.h>
#include <ion/display.h>
#include <kandinsky/color.h>
#include <omg/unreachable.h>

#include "../data/store.h"

namespace Statistics::Categorical {

class PieGraphViewDataSource {
 public:
  PieGraphViewDataSource(Store* store)
      : m_store(store),
        m_numberOfActiveCategories(0),
        m_isSelectionActive(true) {}

  void toggleSelection(bool select) {
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

  int nextCategory(int direction) {
    if (m_isSelectionActive) {
      m_insideColors[m_selectedCategory] = m_selectedCategoryOriginalColor;
    }
    if (direction < 0 && m_selectedCategory == 0) {
      m_selectedCategory = m_numberOfActiveCategories - 1;
    } else {
      m_selectedCategory += direction < 0 ? -1 : 1;
      m_selectedCategory %= m_numberOfActiveCategories;
    }
    if (m_isSelectionActive) {
      m_selectedCategoryOriginalColor = m_insideColors[m_selectedCategory];
      m_insideColors[m_selectedCategory] = k_selectedColor;
    }
    return m_toGlobalCategories[m_selectedCategory];
  }

  int setGroup(int group) {
    m_numberOfActiveCategories = 0;
    m_selectedCategory = UINT8_MAX;
    float cumulatedAngle = 0;
    // static_assert(Store::k_maxNumberOfCategory <=
    // Escher::Palette::numberOfLightDataColors());
    // static_assert(Store::k_maxNumberOfCategory <=
    // Escher::Palette::numberOfDataColors());
    for (int globalCatIndex = 0; globalCatIndex < Store::k_maxNumberOfCategory;
         ++globalCatIndex) {
      float rf = m_store->getRelativeFrequency(group, globalCatIndex);
      if (std::isnan(rf)) {
        continue;
      }
      int localCatIndex = m_numberOfActiveCategories;
      m_insideColors[localCatIndex] =
          Escher::Palette::DataColorLight[globalCatIndex];
      m_borderColors[localCatIndex] =
          Escher::Palette::DataColor[globalCatIndex];
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

 protected:
  static constexpr KDColor k_selectedColor = Escher::Palette::YellowDark;
  static constexpr KDColor k_outsideColor = KDColorWhite;
  float m_cumulatedAngles[Store::k_maxNumberOfCategory];
  uint8_t m_toGlobalCategories[Store::k_maxNumberOfCategory];
  KDColor m_borderColors[Store::k_maxNumberOfCategory];
  KDColor m_insideColors[Store::k_maxNumberOfCategory];
  KDColor m_selectedCategoryOriginalColor;
  Store* m_store;
  uint8_t m_selectedCategory;
  uint8_t m_numberOfActiveCategories;
  bool m_isSelectionActive;
};

// TODO: this class is very linked to the Categorical store atm, it shouldn't be
class PieGraphView : public Escher::View, public PieGraphViewDataSource {
 public:
  PieGraphView(Store* store) : PieGraphViewDataSource(store) {}

  void reload() { markWholeFrameAsDirty(); }

  void drawRect(KDContext* ctx, KDRect rect) const override {
    assert(m_numberOfActiveCategories > 0);
    constexpr int k_frambufferHeight = 149;
    constexpr int k_frambufferWidth = 5;
    assert(bounds().width() % k_frambufferWidth == 0);
    assert(bounds().height() == k_frambufferHeight);

    KDColor framebuffer[k_frambufferHeight][k_frambufferWidth];

    const KDPoint center = KDPoint(bounds().width() / 2, bounds().height() / 2);

    Ion::Display::waitForVBlank();  // Not sure it's useful
    for (int layerStart = bounds().width() - k_frambufferWidth; layerStart >= 0;
         layerStart -= k_frambufferWidth) {
      KDColor* pixels = &framebuffer[0][0];
      for (int j = 0; j < k_frambufferHeight; ++j) {
        for (int i = 0; i < k_frambufferWidth; ++i) {
          *(pixels++) = pointColor(i + layerStart, j, center);
        }
      }
      KDRect layerRect =
          KDRect(layerStart, 0, k_frambufferWidth, k_frambufferHeight);
      ctx->fillRectWithPixels(layerRect, &framebuffer[0][0],
                              &framebuffer[0][0]);
    }
    if (m_numberOfActiveCategories > 1) {
      /* NOTE: Drawing the anti-aliased separation lines in reverse order
       * because it looks better.
       * Otherwise the vertical line separating category 0 and N-1 is too
       * dominant in the center */
      for (int i = m_numberOfActiveCategories - 1; i >= 0; --i) {
        float sin = std::sin(m_cumulatedAngles[i]);
        float cos = std::cos(m_cumulatedAngles[i]);
        float edgeX = center.x() + k_fradius * sin,
              edgeY = center.y() - k_fradius * cos;
        KDColor blend = KDColor::Blend(
            m_borderColors[i],
            m_borderColors[(i + 1) % m_numberOfActiveCategories], 128);
        ctx->drawAntialiasedLineAutoBackground(center.x(), center.y(), edgeX,
                                               edgeY, blend);
      }
    }
  }

 private:
  static constexpr KDCoordinate k_radius = 70;
  static constexpr float k_border = 2.;
  static constexpr float k_fradius = static_cast<float>(k_radius);

  static float DistToCenter(float centeredX, float centeredY) {
    return (centeredX * centeredX + centeredY * centeredY) / k_fradius;
  }

  KDColor pointColor(KDCoordinate x, KDCoordinate y, KDPoint center) const {
    KDCoordinate centeredX = x - center.x();
    KDCoordinate centeredY =
        center.y() - y;  // inverted because Y grows downwards on the screen
    float distanceToCenter = DistToCenter(centeredX, centeredY);
    if (distanceToCenter >= k_fradius + k_border) {
      return k_outsideColor;
    }
    // Negative means inside
    float distanceToRing = distanceToCenter - k_fradius;
    float isCloseToRing = std::fabs(distanceToRing) < k_border;

    // Clockwise angle: 0° is Up, 90° is Right
    float angle = std::atan2(static_cast<float>(centeredX),
                             static_cast<float>(centeredY));
    angle = (angle < 0) ? angle + 2 * M_PI : angle;

    for (int i = 0; i < m_numberOfActiveCategories; ++i) {
      if (angle < m_cumulatedAngles[i]) {
        if (!isCloseToRing) {
          return m_insideColors[i];
        }
        uint8_t alpha = std::fabs(distanceToRing) / k_border * UINT8_MAX;
        return KDColor::Blend(
            distanceToRing < 0 ? m_insideColors[i] : k_outsideColor,
            m_borderColors[i], alpha);
      }
    }
    return KDColorBlack;
  }
};

}  // namespace Statistics::Categorical
