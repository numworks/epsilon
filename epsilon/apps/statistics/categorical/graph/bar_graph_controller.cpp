#include "bar_graph_controller.h"

#include <apps/global_preferences.h>
#include <escher/container.h>

namespace Statistics::Categorical {

void BarGraphController::initView() {
  m_view.toggleSelection(true);
  if (!m_view.m_barGraphView.isSelectedActive()) {
    // Current bar is inactive, select next one
    m_view.selectNextActiveValue(1);
  } else {
    m_view.updateBannerView();
  }
}

void BarGraphController::viewWillAppear() { m_view.toggleSelection(true); }

void BarGraphController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (!m_view.isBarGraphSelected()) {
      header()->setSelectedButton(0);
    }
  }
}

bool BarGraphController::handleEvent(Ion::Events::Event event) {
  if (header()->selectedButton() >= 0) {
    if (event == Ion::Events::Up || event == Ion::Events::Back) {
      header()->setSelectedButton(-1);
      m_tabController->selectTab();
      return true;
    }
    if (event == Ion::Events::Down) {
      header()->setSelectedButton(-1);
      m_view.toggleSelection(true);
      Escher::App::app()->setFirstResponder(this);
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Up || event == Ion::Events::Back) {
    m_view.toggleSelection(false);
    if (event == Ion::Events::Up) {
      header()->setSelectedButton(0);
    } else {
      m_tabController->selectTab();
    }
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    m_view.selectNextActiveValue(event == Ion::Events::Right ? 1 : -1);
    return true;
  }
  return false;
}

/* ContentView */

BarGraphController::ContentView::ContentView(Store* store)
    : m_store(store),
      m_barGraphView(store),
      m_bannerView(),
      m_isBarGraphSelected(false) {}

void BarGraphController::ContentView::selectNextActiveValue(int direction) {
  assert(m_isBarGraphSelected);
  int currentCategory = m_barGraphView.selectedCategory();
  int currentGroup = m_barGraphView.selectedGroup();
  TableDimension dimensions = m_store->currentDimension();
  int nextCategory = currentCategory;
  int nextGroup = currentGroup;

  do {
    if (direction > 0) {
      // Next group to the right
      if (nextGroup < dimensions.col - 1) {
        nextGroup = nextGroup + 1;
      } else if (nextCategory < dimensions.row - 1) {
        nextCategory = nextCategory + 1;
        nextGroup = 0;
      } else {
        /* No active group and category in that direction, stay on the current
         * one */
        nextCategory = currentCategory;
        nextGroup = currentGroup;
        return;
      }
    } else {
      // Next group to the left
      if (nextGroup > 0) {
        nextGroup = nextGroup - 1;
      } else if (nextCategory > 0) {
        nextCategory = nextCategory - 1;
        nextGroup = dimensions.col - 1;
      } else {
        /* No active group and category in that direction, stay on the current
         * one */
        nextCategory = currentCategory;
        nextGroup = currentGroup;
        return;
      }
    }
  } while (!m_store->isGroupActive(nextGroup) ||
           !std::isfinite(m_store->getValue(nextGroup, nextCategory)));

  m_barGraphView.updateSelectedBar(nextCategory, nextGroup);
  updateBannerView();
}

void BarGraphController::ContentView::updateBannerView() {
  int group = m_barGraphView.selectedGroup();
  int category = m_barGraphView.selectedCategory();

  constexpr static int k_precision =
      Escher::AbstractEvenOddBufferTextCell::k_defaultPrecision;
  Poincare::Preferences::PrintFloatMode displayMode =
      GlobalPreferences::SharedGlobalPreferences()->displayMode();
  constexpr static int k_bufferSize =
      1 + Ion::Display::Width / KDFont::GlyphWidth(KDFont::Size::Small);
  char buffer[k_bufferSize];

  // Category name
  m_store->getCategoryName(category, buffer, k_bufferSize);
  m_bannerView.categoryNameView()->setText(buffer);

  // Group name
  m_store->getGroupName(group, buffer, k_bufferSize);
  m_bannerView.groupNameView()->setText(buffer);

  // Frequency
  float freq = m_store->getValue(group, category);
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "%s%s%*.*ef",
                                I18n::translate(I18n::Message::Frequency),
                                I18n::translate(I18n::Message::ColonConvention),
                                freq, displayMode, k_precision);
  m_bannerView.frequencyView()->setText(buffer);

  // Relative frequency
  float relativeFreq = m_store->getRelativeFrequency(group, category);
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "%s%s%*.*ef",
                                I18n::translate(I18n::Message::Relative),
                                I18n::translate(I18n::Message::ColonConvention),
                                relativeFreq, displayMode, k_precision);
  m_bannerView.relativeFrequencyView()->setText(buffer);

  m_bannerView.reload();
}

void BarGraphController::ContentView::layoutSubviews(bool force) {
  const KDSize bannerSize = m_bannerView.minimalSizeForOptimalDisplay();
  setChildFrame(
      &m_bannerView,
      KDRect(KDPoint(0, bounds().height() - bannerSize.height()), bannerSize),
      force);
  const KDSize barGraphSize(bounds().width(),
                            bounds().height() - bannerSize.height());
  setChildFrame(&m_barGraphView, KDRect(KDPointZero, barGraphSize), force);
}

Escher::View* BarGraphController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_barGraphView;
    default:
      assert(index == 1);
      return &m_bannerView;
  }
}

}  // namespace Statistics::Categorical
