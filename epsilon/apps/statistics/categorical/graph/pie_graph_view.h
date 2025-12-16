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
 * color. */
class PieGraphViewDataSource {
 public:
  PieGraphViewDataSource(Store* store);

  void toggleSelection(bool select);
  /* Select the next non-empty category in the given [direction]
   * (>0 clockwise and <0 counterclockwise) and
   * return the selected category store index */
  int nextCategory(int direction);
  /* Set the group to display, select the first non-empty category and
   * return the selected category store index */
  int setGroup(int group);

 protected:
  constexpr static KDColor k_outsideColor = KDColorWhite;
  float m_cumulatedAngles[Store::k_maxNumberOfCategory];
  uint8_t m_toGlobalCategories[Store::k_maxNumberOfCategory];
  /* Note: Now that colors do not change depending on selection, having
   * m_borderColors and m_insideColors might not be worth it anymore and we
   * could use Escher::Palette::DataColor directly. */
  KDColor m_borderColors[Store::k_maxNumberOfCategory];
  KDColor m_insideColors[Store::k_maxNumberOfCategory];
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

  constexpr static int k_framebufferWidth = 5;
  void reload() { markWholeFrameAsDirty(); }
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  constexpr static KDCoordinate k_radius = 70;
  constexpr static float k_fradius = static_cast<float>(k_radius);
  constexpr static float k_border = 1.f;
  constexpr static float k_selectedBorder = 6.f;

  KDColor pointColor(KDCoordinate x, KDCoordinate y, KDPoint center) const;
};

}  // namespace Statistics::Categorical
