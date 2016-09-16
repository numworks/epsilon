extern "C" {
#include <assert.h>
}
#include <escher/tab_view_controller.h>
#include <escher/app.h>

TabViewController::ContentView::ContentView() :
  View(),
  m_activeView(nullptr)
{
};

void TabViewController::ContentView::setActiveView(View * view) {
  m_activeView = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}

void TabViewController::ContentView::layoutSubviews() {
  KDCoordinate tabHeight = 20;

  KDRect tabViewFrame = KDRect(
      0, 0,
      m_frame.width(), tabHeight
      );
  m_tabView.setFrame(tabViewFrame);
  if (m_activeView) {
    KDRect activeViewFrame = KDRect(
        0, tabHeight,
        m_frame.width(),
        m_frame.height() - tabHeight
        );
    m_activeView->setFrame(activeViewFrame);
  }
}

int TabViewController::ContentView::numberOfSubviews() const {
  return 2;
}

View * TabViewController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_tabView;
  } else {
    assert(index == 1);
    return m_activeView;
  }
}

#if ESCHER_VIEW_LOGGING
const char * TabViewController::ContentView::className() const {
  return "TabViewController::ContentView";
}
#endif

TabViewController::TabViewController(Responder * parentResponder, ViewController * one, ViewController * two) :
  ViewController(parentResponder),
  m_numberOfChildren(2),
  m_activeChildIndex(-1),
  m_selectedChildIndex(-1)
{
  m_children[0] = one;
  m_children[1] = two;

  one->setParentResponder(this);
  two->setParentResponder(this);

  // TODO: This should be lazy loaded!
  // So this code should live in view()
  for (int i=0; i<m_numberOfChildren; i++) {
    m_view.m_tabView.addTabNamed(m_children[i]->title());
  }
}

bool TabViewController::handleEvent(Ion::Events::Event event) {
  if (app()->firstResponder() != this) {
    return false;
  }
  switch(event) {
    case Ion::Events::Event::LEFT_ARROW:
      if (m_selectedChildIndex > 0) {
        setSelectedTab(m_selectedChildIndex-1);
      }
      return true;
    case Ion::Events::Event::RIGHT_ARROW:
      if (m_selectedChildIndex < m_numberOfChildren-1) {
        setSelectedTab(m_selectedChildIndex+1);
      }
      return true;
    case Ion::Events::Event::ENTER:
      if (m_selectedChildIndex != m_activeChildIndex) {
        setActiveTab(m_selectedChildIndex);
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

void TabViewController::setActiveTab(int8_t i) {
  if (i == m_activeChildIndex) {
    return;
  }
  //TODO assert(i <= m_numberOfchildren);
  ViewController * activeVC = m_children[i];
  m_view.setActiveView(activeVC->view());
  m_view.m_tabView.setActiveIndex(i);
  m_activeChildIndex = i;

  app()->setFirstResponder(activeVC);
}

void TabViewController::setSelectedTab(int8_t i) {
  if (i == m_selectedChildIndex) {
    return;
  }
  m_view.m_tabView.setSelectedIndex(i);
  m_selectedChildIndex = i;
}

void TabViewController::didBecomeFirstResponder() {
  setSelectedTab(m_activeChildIndex);
}

void TabViewController::didResignFirstResponder() {
  setSelectedTab(-1);
}


View * TabViewController::view() {
  // We're asked for a view!
  // Let's populate our tabview
  if (m_activeChildIndex < 0) {
    setActiveTab(0);
  }

  return &m_view;
}

uint8_t TabViewController::numberOfTabs() {
  return m_numberOfChildren;
}

const char * TabViewController::tabName(uint8_t index) {
  return m_children[index]->title();
}
