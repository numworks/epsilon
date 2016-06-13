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
  markAsNeedingRedraw();
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

View * TabView::subview(int index) {
  assert(index < m_numberOfTabs);
  return &m_cells[index];
}

void TabView::layoutSubviews() {
  // Simple layout: all tabs have the same length
  KDCoordinate tabLength = m_frame.width/m_numberOfTabs;
  for (int i=0; i<m_numberOfTabs; i++) {
    KDRect cellFrame;
    cellFrame.x = i*tabLength;
    cellFrame.y = 0;
    cellFrame.width = tabLength;
    cellFrame.height = m_frame.height;
    m_cells[i].setFrame(cellFrame);
  }
}

void TabView::storeSubviewAtIndex(View * view, int index) {
  // We're not doing anything here, because we already store all the subviews we ever wanna have
  assert(&m_cells[index] == view);
}

#if ESCHER_VIEW_LOGGING
const char * TabView::className() const {
  return "TabView";
}

void TabView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " numberOfTabs=\"" << (int)m_numberOfTabs << "\"";
  os << " activeTabIndex=\"" << (int)m_activeTabIndex << "\"";
}
#endif
