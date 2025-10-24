#pragma once

#include <apps/i18n.h>
#include <escher/palette.h>
#include <escher/view.h>
#include <ion/display.h>
#include <kandinsky/color.h>
#include <omg/unreachable.h>

#include "../data/store.h"

namespace Statistics::Categorical {

/* DataSource for the PieGraph.
 * Computes and stores the cumulated angles for each section, as well as their
 * color */
class PieGraphViewDataSource {
 public:
  PieGraphViewDataSource(Store* store);

  void toggleSelection(bool select);
  int nextCategory(int direction);
  int setGroup(int group);

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

/* A pie chart: given a data source, produces a pie graph representing the
 * data.
 * TODO: extract this class into a standalone shared class if necessary */
class PieGraphView : public Escher::View, public PieGraphViewDataSource {
 public:
  PieGraphView(Store* store) : PieGraphViewDataSource(store) {}

  void reload() { markWholeFrameAsDirty(); }
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  static constexpr KDCoordinate k_radius = 70;
  static constexpr float k_fradius = static_cast<float>(k_radius);
  static constexpr float k_border = 2.;

  static float DistToCenter(float centeredX, float centeredY) {
    return (centeredX * centeredX + centeredY * centeredY) / k_fradius;
  }
  KDColor pointColor(KDCoordinate x, KDCoordinate y, KDPoint center) const;
};

}  // namespace Statistics::Categorical
