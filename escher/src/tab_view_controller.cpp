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
  m_tabView.setFrame({
    0,
    0,
    this->bounds().width,
    tabHeight
  });
  if (m_activeView) {
    m_activeView->setFrame({
      0,
      tabHeight,
      this->bounds().width,
      (KDCoordinate)(this->bounds().height - tabHeight)
    });
  }
}

int TabViewController::ContentView::numberOfSubviews() {
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

TabViewController::TabViewController(ViewController ** children, uint8_t numberOfChildren) :
  m_children(children),
  m_numberOfChildren(numberOfChildren),
  m_activeChildIndex(-1)
{
  for (int i=0; i<numberOfChildren; i++) {
    m_view.m_tabView.addTabNamed(children[i]->title());
  }
}

void TabViewController::setActiveTab(uint8_t i) {
  if (i == m_activeChildIndex) {
    return;
  }
  //TODO assert(i <= m_numberOfchildren);
  ViewController * activeVC = m_children[i];
  m_view.setActiveView(activeVC->view());
  m_view.m_tabView.setActiveIndex(i);
  m_activeChildIndex = i;
  m_view.redraw();
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
