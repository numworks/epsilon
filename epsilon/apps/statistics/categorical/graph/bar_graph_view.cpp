#include "bar_graph_view.h"

namespace Statistics::Categorical {

void BarGraphView::updateSelectedBar(int category, int group) {
  reloadSelectedBar();
  m_contentView.m_selectedCategory = category;
  m_contentView.m_selectedGroup = group;
  reloadSelectedBar();
  scrollToSelectedBar();
}

void BarGraphView::reloadSelectedBar() {
  if (isSelectedActive()) {
    KDRect barFrame =
        m_contentView.frameForActiveBar(selectedCategory(), selectedGroup());
    markRectAsDirty(barFrame.relativeTo(contentOffset()));
  }
}

void BarGraphView::scrollToSelectedBar() {
  KDPoint previousContentOffset = contentOffset();
  KDRect barFrame = m_contentView.frameForActiveBar(selectedCategory(),
                                                    selectedGroup(), true);
  // Scroll to the bar with margin
  scrollToContentRect(barFrame.paddedWith(ContentView::k_horizontalMargin));
  if (previousContentOffset != contentOffset()) {
    markWholeFrameAsDirty();
  }
}

/* ContentView */

void BarGraphView::ContentView::drawRect(KDContext* ctx, KDRect rect) const {
  // Draw background
  ctx->fillRect(rect, KDColorWhite);
  // Draw axis
  ctx->drawLine(KDPoint(0, bounds().height() - k_legendHeight - k_axisHeight),
                KDPoint(bounds().width(),
                        bounds().height() - k_legendHeight - k_axisHeight),
                KDColorBlack);
  // Draw categories
  float maxBarValue = m_store->getMaxTableValue();
  for (int category = 0; category < m_store->currentDimension().row;
       ++category) {
    if (m_store->isCategoryActive(category)) {
      drawCategory(ctx, frameForActiveCategory(category), category,
                   maxBarValue);
    }
  }
}

void BarGraphView::ContentView::drawCategory(KDContext* ctx, KDRect rect,
                                             int category,
                                             float maxValue) const {
  KDCoordinate xOffset = rect.x();
  KDCoordinate yOffset = rect.y();

  // Draw legend
  char buffer[sizeof(Store::Label)];
  m_store->getCategoryName(category, buffer, sizeof(buffer));
  ctx->alignAndDrawString(buffer, KDPoint(xOffset, yOffset + k_maxBarHeight),
                          KDSize(rect.width(), k_legendHeight),
                          {.style = {.font = KDFont::Size::Small},
                           .horizontalAlignment = KDGlyph::k_alignCenter,
                           .verticalAlignment = KDGlyph::k_alignCenter});

  // Draw bars
  if (maxValue == 0.0f) {
    // All bars have a height of 0
    return;
  }
  KDCoordinate centerOffset =
      (rect.width() - m_store->numberOfActiveGroups() * k_barWidth) / 2;
  xOffset += centerOffset;
  for (int group = 0; group < m_store->currentDimension().col; ++group) {
    if (!m_store->isGroupActive(group)) {
      continue;
    }
    // Draw bar
    float value = m_store->getValue(group, category);
    KDCoordinate barHeight = (value / maxValue) * k_maxBarHeight;
    KDRect barRect(xOffset, yOffset + k_maxBarHeight - barHeight, k_barWidth,
                   barHeight);
    ctx->fillRect(barRect, Escher::Palette::DataColorLight[group]);
    // Draw borders
    KDColor borderColor = Escher::Palette::DataColor[group];
    KDCoordinate borderWidth =
        m_isSelected &&
                (category == m_selectedCategory && group == m_selectedGroup)
            ? k_selectedBorder
            : k_border;
    if (barRect.width() > 0 && barRect.height() > 0) {
      // Left
      ctx->fillRect(KDRect(xOffset, yOffset + k_maxBarHeight - barHeight,
                           borderWidth, barHeight),
                    borderColor);
      // Top
      ctx->fillRect(KDRect(xOffset, yOffset + k_maxBarHeight - barHeight,
                           k_barWidth, borderWidth),
                    borderColor);
      // Right
      ctx->fillRect(
          KDRect(xOffset + k_barWidth - borderWidth,
                 yOffset + k_maxBarHeight - barHeight, borderWidth, barHeight),
          borderColor);
    }
    xOffset += k_barWidth;
  }
}

KDRect BarGraphView::ContentView::frameForActiveCategory(int category) const {
  assert(m_store->isCategoryActive(category));
  KDCoordinate categoryWidth = categoriesWidth();
  /* Add an horizontal margin on the left, between categories and on the right.
   * Add a vertical margin at the top. */
  return KDRect(
      k_horizontalMargin + m_store->indexInActiveCategories(category) *
                               (k_horizontalMargin + categoryWidth),
      k_verticalMargin, categoryWidth, bounds().height() - k_verticalMargin);
}

KDRect BarGraphView::ContentView::frameForActiveBar(int category, int group,
                                                    bool padWithOffset) const {
  assert(m_store->isGroupActive(group));
  KDRect categoryFrame = frameForActiveCategory(category);
  KDCoordinate centerOffset =
      (categoryFrame.width() - m_store->numberOfActiveGroups() * k_barWidth) /
      2;
  KDRect barFrame = categoryFrame.translatedBy(
      KDPoint(centerOffset +
                  ContentView::k_barWidth * m_store->indexInActiveGroups(group),
              0));
  barFrame.setSize(KDSize(k_barWidth, barFrame.height()));
  return padWithOffset ? barFrame.paddedWith(centerOffset) : barFrame;
}

KDCoordinate BarGraphView::ContentView::categoriesWidth() const {
  // Compute the max width of category names
  KDCoordinate maxNameWidth = 0;
  for (int category = 0; category < m_store->currentDimension().row;
       ++category) {
    if (!m_store->isCategoryActive(category)) {
      continue;
    }
    char buffer[sizeof(Store::Label)];
    m_store->getCategoryName(category, buffer, sizeof(buffer));
    KDCoordinate nameWidth =
        KDFont::Font(KDFont::Size::Small)->stringSize(buffer).width();
    if (nameWidth > maxNameWidth) {
      maxNameWidth = nameWidth;
    }
  }

  // Compute the total width of all group bars
  KDCoordinate barsWidth = k_barWidth * m_store->numberOfActiveGroups();

  return std::max(maxNameWidth, barsWidth);
}

KDSize BarGraphView::ContentView::minimalSizeForOptimalDisplay() const {
  /* There is an horizontal margin on the left of each category and one at the
   * end.
   * Width should be at least as long as the display width so that the content
   * takes all the available space. */
  KDCoordinate width = (categoriesWidth() + k_horizontalMargin) *
                           m_store->numberOfActiveCategories() +
                       k_horizontalMargin;
  KDCoordinate displayWidth = Ion::Display::Width;
  return KDSize(std::max(width, displayWidth), k_totalHeight);
}

}  // namespace Statistics::Categorical
