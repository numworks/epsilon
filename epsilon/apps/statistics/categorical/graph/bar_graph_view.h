#pragma once

#include <escher/scroll_view.h>

#include "../data/store.h"
#include "kandinsky/coordinate.h"

namespace Statistics::Categorical {

/* Displays the bar graph, keeps track of the selected bar and handles
 * scrolling. */
class BarGraphView : public Escher::ScrollView,
                     public Escher::ScrollViewDataSource {
 public:
  BarGraphView(Store* store)
      : ScrollView(&m_contentView, this), m_contentView(store) {
    setBackgroundColor(KDColorWhite);
    m_decorator.setVisibility(true);
  }

  // ScrollView
  Decorator* decorator() override { return &m_decorator; }

  // ContentView accessors
  int selectedGroup() const { return m_contentView.m_selectedGroup; }
  int selectedCategory() const { return m_contentView.m_selectedCategory; }
  bool isSelectedActive() const {
    return m_contentView.m_store->isCategoryActive(
               m_contentView.m_selectedCategory) &&
           m_contentView.m_store->isGroupActive(m_contentView.m_selectedGroup);
  }

  // Handle selection
  void toggleSelection(bool isSelected) {
    m_contentView.m_isSelected = isSelected;
    reloadSelectedBar();
  }
  void updateSelectedBar(int category, int group);
  void scrollToSelectedBar();

  // Redraw
  void reloadSelectedBar();

  class ContentView : public Escher::View {
    friend class BarGraphView;

   public:
    ContentView(Store* store)
        : m_store(store),
          m_selectedCategory(0),
          m_selectedGroup(0),
          m_isSelected(false) {}
    void drawRect(KDContext* ctx, KDRect rect) const override;
    void drawCategory(KDContext* ctx, KDRect rect, int category,
                      float maxValue) const;
    KDCoordinate categoriesWidth() const;
    KDRect frameForActiveCategory(int category) const;
    KDRect frameForActiveBar(int category, int group,
                             bool padWithOffset = false) const;
    KDSize minimalSizeForOptimalDisplay() const override;

   private:
    constexpr static KDCoordinate k_totalHeight = 137;
    constexpr static KDCoordinate k_verticalMargin = 10;
    constexpr static KDCoordinate k_maxBarHeight = 100;
    constexpr static KDCoordinate k_axisHeight = 2;
    constexpr static KDCoordinate k_legendHeight = 25;
    static_assert(k_totalHeight == k_legendHeight + k_axisHeight +
                                       k_maxBarHeight + k_verticalMargin,
                  "Total height is incorrectly split.");
    constexpr static KDCoordinate k_horizontalMargin = 20;
    constexpr static KDCoordinate k_barWidth = 20;
    constexpr static KDCoordinate k_border = 1;
    constexpr static KDColor k_selectedColor = Escher::Palette::YellowDark;
    Store* m_store;
    int m_selectedCategory;
    int m_selectedGroup;
    bool m_isSelected;
  };

 private:
  ContentView m_contentView;
  BarDecorator m_decorator;
};

}  // namespace Statistics::Categorical
