#pragma once

#include <apps/i18n.h>
#include <escher/palette.h>
#include <escher/view.h>
#include <ion/display.h>
#include <kandinsky/color.h>
#include <omg/unreachable.h>

#include "../data/store.h"

namespace Statistics::Categorical {

// TODO: this class is very linked to the Categorical store atm, it shouldn't be
class PieGraphView : public Escher::View {
 public:
  PieGraphView(Store* store) : m_store(store) {
    for (float& f : m_cumulatedAngles) {
      f = k_inactiveCategory;
    }
  }

  void toggleSelection(bool select) {
    if (m_isSelectionActive != select) {
      m_isSelectionActive = select;
      markWholeFrameAsDirty();
    }
  }

  void nextCategory(int direction) {
    if (direction < 0) {
      direction = -1;
    } else {
      direction = 1;
    }
    for (int i = 0; i < Store::k_maxNumberOfCategory; ++i) {
      m_selectedCategory += direction;
      m_selectedCategory %= Store::k_maxNumberOfCategory;
      assert(0 <= m_selectedCategory &&
             m_selectedCategory < Store::k_maxNumberOfCategory);
      if (m_cumulatedAngles[m_selectedCategory] != k_inactiveCategory) {
        markWholeFrameAsDirty();
        return;
      }
    }
    OMG::unreachable();  // No active category
  }

  void setGroup(int group) {
    m_selectedCategory = UINT8_MAX;
    float cumulatedAngle = 0;
    for (int cat = 0; cat < Store::k_maxNumberOfCategory; ++cat) {
      float rf = m_store->getRelativeFrequency(group, cat);
      if (std::isnan(rf)) {
        m_cumulatedAngles[cat] = k_inactiveCategory;
        continue;
      }
      cumulatedAngle += 2 * M_PI * rf;
      m_cumulatedAngles[cat] = cumulatedAngle;
      if (m_selectedCategory == UINT8_MAX) {
        // Set the selectedCategory to the first active category
        m_selectedCategory = cat;
      }
    }
  }

  void drawRect(KDContext* ctx, KDRect rect) const override {
    constexpr int k_frambufferHeight = 149;
    constexpr int k_frambufferWidth = 5;
    assert(bounds().width() % k_frambufferWidth == 0);
    assert(bounds().height() == k_frambufferHeight);

    KDColor framebuffer[k_frambufferHeight][k_frambufferWidth];

    const KDPoint center = KDPoint(bounds().width() / 2, bounds().height() / 2);

    // NOTE: memoize the color array for faster access
    KDColor colors[Escher::Palette::numberOfLightDataColors()];
    memcpy(colors, Escher::Palette::DataColorLight, sizeof(colors));
    if (m_isSelectionActive) {
      colors[m_selectedCategory] = k_selectedColor;
    }

    Ion::Display::waitForVBlank();  // Not sure it's useful
    for (int layerStart = bounds().width() - k_frambufferWidth; layerStart >= 0;
         layerStart -= k_frambufferWidth) {
      KDColor* pixels = &framebuffer[0][0];
      for (int j = 0; j < k_frambufferHeight; ++j) {
        for (int i = 0; i < k_frambufferWidth; ++i) {
          *(pixels++) = pointColor(i + layerStart, j, center, colors);
        }
      }
      KDRect layerRect =
          KDRect(layerStart, 0, k_frambufferWidth, k_frambufferHeight);
      ctx->fillRectWithPixels(layerRect, &framebuffer[0][0],
                              &framebuffer[0][0]);
    }
  }

 private:
  static constexpr KDCoordinate k_radius = 70;
  static constexpr float k_border = 3.;
  static constexpr float k_fradius = static_cast<float>(k_radius);
  static constexpr float k_inactiveCategory = -1.;
  static constexpr KDColor k_selectedColor = Escher::Palette::YellowDark;
  static constexpr KDColor k_outsideColor = KDColorWhite;

  static float DistToCenter(float centeredX, float centeredY) {
    return (centeredX * centeredX + centeredY * centeredY) / k_fradius;
  }

  // TODO handle single category pie chart (no border)
  KDColor pointColor(KDCoordinate x, KDCoordinate y, KDPoint center,
                     KDColor* mainColors) const {
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

    for (int i = 0; i < Store::k_maxNumberOfCategory; ++i) {
      if (m_cumulatedAngles[i] == k_inactiveCategory) {
        continue;
      }
      if (angle < m_cumulatedAngles[i]) {
        if (!isCloseToRing) {
          return mainColors[i];
        }
        uint8_t alpha = std::fabs(distanceToRing) / k_border * UINT8_MAX;
        return KDColor::Blend(
            distanceToRing < 0 ? mainColors[i] : k_outsideColor,
            Escher::Palette::DataColor[i], alpha);
      }
    }
    return KDColorBlack;
  }

  float m_cumulatedAngles[Store::k_maxNumberOfCategory];
  Store* m_store;
  uint8_t m_selectedCategory;
  bool m_isSelectionActive;
};

}  // namespace Statistics::Categorical
