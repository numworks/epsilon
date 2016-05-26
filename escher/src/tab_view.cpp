#include <escher/tab_view.h>
#include <escher/tab_view_controller.h>
extern "C" {
#include <assert.h>
}

TabView::TabView() :
  View(),
  m_numberOfTabs(0),
  m_activeTabIndex(0)
{
}

void TabView::addTabNamed(const char * name) {
  assert(m_numberOfTabs < k_maxNumberOfTabs);
  uint8_t tabIndex = m_numberOfTabs;
  m_cells[tabIndex].setName(name);
  m_numberOfTabs++;
  setSubview(&m_cells[tabIndex], tabIndex);
  redraw();
}

void TabView::setActiveIndex(int index) {
  assert(index < m_numberOfTabs);
  if (m_activeTabIndex == index) {
    return;
  }
  m_cells[m_activeTabIndex].setActive(false);
  m_activeTabIndex = index;
  m_cells[m_activeTabIndex].setActive(true);
}

int TabView::numberOfSubviews() const {
  return m_numberOfTabs;
}

const View * TabView::subview(int index) const {
  assert(index < m_numberOfTabs);
  return &m_cells[index];
}

void TabView::layoutSubviews() {
  // Simple layout: all tabs have the same length
  KDCoordinate tabLength = bounds().width/m_numberOfTabs;
  for (int i=0; i<m_numberOfTabs; i++) {
    m_cells[i].setFrame({
        (KDCoordinate)(i*tabLength),
        0,
        tabLength,
        bounds().height
      });
  }
}

void TabView::storeSubviewAtIndex(View * view, int index) {
  // We're not doing anything here, because we already store all the subviews we ever wanna have
  assert(&m_cells[index] == view);
}
