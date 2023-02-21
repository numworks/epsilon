#include <escher/tab_view.h>
#include <escher/tab_view_controller.h>
extern "C" {
#include <assert.h>
}

namespace Escher {

TabView::TabView()
    : View(), m_numberOfTabs(0), m_activeTabIndex(-1), m_selectedTabIndex(-1) {}

int TabView::numberOfTabs() const { return m_numberOfTabs; }

void TabView::drawRect(KDContext *ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  // Draw a separator with the content
  ctx->fillRect(KDRect(0, height - k_activeTabHeight, width, k_activeTabHeight),
                KDColorWhite);
}

void TabView::addTab(ViewController *controller) {
  assert(m_numberOfTabs < k_maxNumberOfTabs);
  uint8_t tabIndex = m_numberOfTabs;
  m_cells[tabIndex].setNamedController(controller);
  m_numberOfTabs++;
  markRectAsDirty(bounds());
}

void TabView::setActiveIndex(int index) {
  assert(index < m_numberOfTabs);
  if (m_activeTabIndex == index) {
    return;
  }
  if (m_activeTabIndex >= 0) {
    m_cells[m_activeTabIndex].setActive(false);
  }
  m_activeTabIndex = index;
  m_cells[m_activeTabIndex].setActive(true);
}

void TabView::setSelectedIndex(int index) {
  assert(index < m_numberOfTabs);
  if (m_selectedTabIndex == index) {
    return;
  }
  if (m_selectedTabIndex >= 0) {
    m_cells[m_selectedTabIndex].setSelected(false);
  }
  m_selectedTabIndex = index;
  if (m_selectedTabIndex >= 0) {
    m_cells[m_selectedTabIndex].setSelected(true);
  }
}

int TabView::numberOfSubviews() const { return m_numberOfTabs; }

View *TabView::subviewAtIndex(int index) {
  assert(index < m_numberOfTabs);
  return &m_cells[index];
}

void TabView::layoutSubviews(bool force) {
  KDCoordinate emptyWidth = bounds().width();
  for (int i = 0; i < m_numberOfTabs; i++) {
    emptyWidth -= m_cells[i].minimalSizeForOptimalDisplay().width();
  }
  KDCoordinate widthUsed = 0;
  for (int i = 0; i < m_numberOfTabs; i++) {
    KDCoordinate tabWidth = m_cells[i].minimalSizeForOptimalDisplay().width() +
                            emptyWidth / m_numberOfTabs;
    /* Avoid a unused one-pixel-width vertical on the left due to rounding error
     */
    if (i == m_numberOfTabs - 1) {
      tabWidth = bounds().width() - widthUsed;
    }
    KDRect cellFrame =
        KDRect(widthUsed, 0, tabWidth, m_frame.height() - k_activeTabHeight);
    m_cells[i].setFrame(cellFrame, force);
    widthUsed += tabWidth;
  }
}

#if ESCHER_VIEW_LOGGING
const char *TabView::className() const { return "TabView"; }

void TabView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " numberOfTabs=\"" << (int)m_numberOfTabs << "\"";
  os << " activeTabIndex=\"" << (int)m_activeTabIndex << "\"";
}
#endif

}  // namespace Escher
