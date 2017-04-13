extern "C" {
#include <assert.h>
}
#include <escher/tab_view_controller.h>
#include <escher/metric.h>
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
  KDRect tabViewFrame = KDRect(
      0, 0,
      m_frame.width(), Metric::TabHeight
      );
  m_tabView.setFrame(tabViewFrame);
  if (m_activeView) {
    KDRect activeViewFrame = KDRect(
        0, Metric::TabHeight,
        m_frame.width(),
        m_frame.height() - Metric::TabHeight
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

TabViewController::TabViewController(Responder * parentResponder, ViewController * one, ViewController * two, ViewController * three, ViewController * four) :
  ViewController(parentResponder),
  m_activeChildIndex(-1),
  m_selectedChildIndex(-1)
{
  m_children[0] = one;
  m_children[1] = two;
  m_children[2] = three;
  m_children[3] = four;

  for (int i = 0; i < k_maxNumberOfChildren; i++) {
    if (m_children[i] != nullptr) {
      m_numberOfChildren ++;
    }
  }
}

int TabViewController::activeTab() const {
  return m_activeChildIndex;
}

bool TabViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    if (app()->firstResponder() != this) {
      app()->setFirstResponder(this);
      return true;
    }
    return false;
  }
  if (app()->firstResponder() != this) {
    return false;
  }
  if (event == Ion::Events::Left) {
    if (m_selectedChildIndex > 0) {
      setSelectedTab(m_selectedChildIndex-1);
    }
    return true;
  }
  if (event == Ion::Events::Right) {
    if (m_selectedChildIndex < m_numberOfChildren-1) {
      setSelectedTab(m_selectedChildIndex+1);
    }
    return true;
  }
  if (event == Ion::Events::Down) {
    setActiveTab(m_activeChildIndex, false);
    return true;
  }
  if (event == Ion::Events::OK) {
    setActiveTab(m_selectedChildIndex, true);
    return true;
  }
  return false;
}

void TabViewController::setActiveTab(int8_t i, bool forceReactive) {
  ViewController * activeVC = m_children[i];
  if (i  != m_activeChildIndex || forceReactive) {
    assert(i <= m_numberOfChildren);
    m_view.setActiveView(activeVC->view());
    m_view.m_tabView.setActiveIndex(i);
    if (m_activeChildIndex >= 0) {
      m_children[m_activeChildIndex]->viewWillDisappear();
    }
    m_activeChildIndex = i;
    if (i >= 0) {
      m_children[i]->viewWillAppear();
    }
  }
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

void TabViewController::willResignFirstResponder() {
  setSelectedTab(-1);
}


View * TabViewController::view() {
  // We're asked for a view!
  // Let's populate our tabview
  if (m_view.m_tabView.numberOfTabs() != m_numberOfChildren) {
    for (int i=0; i<m_numberOfChildren; i++) {
      m_view.m_tabView.addTab(m_children[i]);
    }
  }
  return &m_view;
}

uint8_t TabViewController::numberOfTabs() {
  return m_numberOfChildren;
}

const char * TabViewController::tabName(uint8_t index) {
  return m_children[index]->title();
}

void TabViewController::viewWillAppear() {
  if (m_activeChildIndex < 0) {
    setActiveTab(0, true);
  } else {
    setActiveTab(m_activeChildIndex, true);
  }
  ViewController * activeVC = m_children[m_activeChildIndex];
  activeVC->viewWillAppear();
}

void TabViewController::viewWillDisappear() {
  ViewController * activeVC = m_children[m_activeChildIndex];
  activeVC->viewWillDisappear();
}
