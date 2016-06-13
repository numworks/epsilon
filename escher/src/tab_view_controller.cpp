extern "C" {
#include <assert.h>
}
#include <escher/tab_view_controller.h>

TabViewController::ContentView::ContentView() :
  View(),
  m_activeView(nullptr)
{
  setSubview(&m_tabView, 0);
};

void TabViewController::ContentView::setActiveView(View * view) {
  setSubview(view, 1);
  layoutSubviews();
}

void TabViewController::ContentView::layoutSubviews() {
  KDCoordinate tabHeight = 20;

  KDRect tabViewFrame;
  tabViewFrame.origin = KDPointZero;
  tabViewFrame.width = m_frame.width,
  tabViewFrame.height = tabHeight;
  m_tabView.setFrame(tabViewFrame);
  if (m_activeView) {
    KDRect activeViewFrame;
    activeViewFrame.x = 0;
    activeViewFrame.y = tabHeight;
    activeViewFrame.width = m_frame.width;
    activeViewFrame.height = m_frame.height - tabHeight;
    m_activeView->setFrame(activeViewFrame);
  }
}

int TabViewController::ContentView::numberOfSubviews() const {
  return 2;
}

View * TabViewController::ContentView::subview(int index) {
  if (index == 0) {
    return &m_tabView;
  } else {
    assert(index == 1);
    return m_activeView;
  }
}

void TabViewController::ContentView::storeSubviewAtIndex(View * view, int index) {
  if (index == 1) {
    m_activeView = view;
  }
}

#if ESCHER_VIEW_LOGGING
const char * TabViewController::ContentView::className() const {
  return "TabViewController::ContentView";
}
#endif

TabViewController::TabViewController(ViewController * one, ViewController * two) :
  ViewController(),
  m_numberOfChildren(2),
  m_activeChildIndex(-1)
{
  m_children[0] = one;
  m_children[1] = two;

  // TODO: This should be lazy loaded!
  // So this code should live in view()
  for (int i=0; i<m_numberOfChildren; i++) {
    m_view.m_tabView.addTabNamed(m_children[i]->title());
  }
}

bool TabViewController::handleEvent(ion_event_t event) {
  switch(event) {
    case LEFT_ARROW:
      if (m_activeChildIndex > 0) {
        setActiveTab(m_activeChildIndex-1);
      }
      return true;
    case RIGHT_ARROW:
      if (m_activeChildIndex < m_numberOfChildren-1) {
        setActiveTab(m_activeChildIndex+1);
      }
      return true;
    default:
      return false;
  }
}

/*
TabViewController::TabViewController(ViewController ** children, uint8_t numberOfChildren) :
  m_children(children),
  m_numberOfChildren(numberOfChildren),
  m_activeChildIndex(-1)
{
  for (int i=0; i<numberOfChildren; i++) {
    m_view.m_tabView.addTabNamed(children[i]->title());
  }
}
*/

void TabViewController::setActiveTab(uint8_t i) {
  if (i == m_activeChildIndex) {
    return;
  }
  //TODO assert(i <= m_numberOfchildren);
  ViewController * activeVC = m_children[i];
  m_view.setActiveView(activeVC->view());
  m_view.m_tabView.setActiveIndex(i);
  m_activeChildIndex = i;
  m_view.markAsNeedingRedraw();
}

View * TabViewController::view() {
  // We're asked for a view!
  // Let's populate our tabview
  setActiveTab(0);

  return &m_view;
}

void TabViewController::handleKeyEvent(int key) {
  // Switch tabs!
}

uint8_t TabViewController::numberOfTabs() {
  return m_numberOfChildren;
}

const char * TabViewController::tabName(uint8_t index) {
  return m_children[index]->title();
}
