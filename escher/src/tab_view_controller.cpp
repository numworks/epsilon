extern "C" {
#include <assert.h>
}
#include <escher/tab_view_controller.h>

TabViewController::ContentView::ContentView(TabViewController * vc) :
  View(),
  m_tabView(TabView(vc)),
  m_activeView(nullptr)
{
};

void TabViewController::ContentView::setActiveView(View * view) {
  setSubview(view, 1);
}

void TabViewController::ContentView::layoutSubviews() {
  if (m_activeView) {
    m_activeView->setFrame(this->bounds());
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

void TabViewController::ContentView::ContentView::storeSubviewAtIndex(View * view, int index) {
  assert(index == 1);
  m_activeView = view;
}

TabViewController::TabViewController(ViewController ** children) :
  m_children(children),
  m_activeChildIndex(0),
  m_view(ContentView(this))
{
  setActiveTab(0);
}

void TabViewController::setActiveTab(uint8_t i) {
  if (i == m_activeChildIndex) {
    return;
  }
  //TODO assert(i <= m_numberOfchildren);
  ViewController * activeVC = m_children[i];
  m_view.setActiveView(activeVC->view());
  m_activeChildIndex = i;
  m_view.redraw();
  //m_tabView.redraw();
}

View * TabViewController::view() {
  return &m_view;
}

void TabViewController::handleKeyEvent(int key) {
  // Switch tabs!
}
