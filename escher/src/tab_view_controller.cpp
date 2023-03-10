extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/tab_view_controller.h>

namespace Escher {

TabViewController::ContentView::ContentView()
    : View(), m_activeView(nullptr), m_displayTabs(true){};

void TabViewController::ContentView::setActiveView(View* view) {
  m_activeView = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}

void TabViewController::ContentView::setDisplayTabs(bool display) {
  if (display != m_displayTabs) {
    m_displayTabs = display;
    layoutSubviews();
    markRectAsDirty(bounds());
  }
}

void TabViewController::ContentView::layoutSubviews(bool force) {
  KDRect tabViewFrame = m_displayTabs
                            ? KDRect(0, 0, m_frame.width(), Metric::TabHeight)
                            : KDRectZero;
  setChildFrame(&m_tabView, tabViewFrame, force);
  if (m_activeView) {
    KDRect activeViewFrame = KDRect(0, tabViewFrame.height(), m_frame.width(),
                                    m_frame.height() - tabViewFrame.height());
    setChildFrame(m_activeView, activeViewFrame, force);
  }
}

int TabViewController::ContentView::numberOfSubviews() const { return 2; }

View* TabViewController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_tabView;
  } else {
    assert(index == 1);
    return m_activeView;
  }
}

#if ESCHER_VIEW_LOGGING
const char* TabViewController::ContentView::className() const {
  return "TabViewController::ContentView";
}
#endif

TabViewController::TabViewController(Responder* parentResponder,
                                     TabViewDataSource* dataSource,
                                     ViewController* one, ViewController* two,
                                     ViewController* three,
                                     ViewController* four)
    : ViewController(parentResponder),
      m_dataSource(dataSource),
      m_isSelected(false) {
  assert(one != nullptr);
  assert(two != nullptr || (three == nullptr && four == nullptr));
  assert(three != nullptr || four == nullptr);
  m_children[0] = one;
  m_children[1] = two;
  m_children[2] = three;
  m_children[3] = four;

  m_numberOfChildren = 0;
  while (m_numberOfChildren < k_maxNumberOfChildren &&
         m_children[m_numberOfChildren] != nullptr) {
    m_numberOfChildren++;
  }
}

int TabViewController::activeTab() const { return m_dataSource->activeTab(); }

void TabViewController::selectTab() {
  m_isSelected = true;
  Container::activeApp()->setFirstResponder(this);
}

bool TabViewController::handleEvent(Ion::Events::Event event) {
  App* app = Container::activeApp();
  if (app->firstResponder() != this) {
    if (event == Ion::Events::Back) {
      selectTab();
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Back && m_dataSource->selectedTab() > 0) {
    setSelectedTab(0);
    setActiveTab(m_dataSource->selectedTab(), false);
    return true;
  }
  if (event == Ion::Events::Left) {
    if (m_dataSource->selectedTab() > 0) {
      setSelectedTab(m_dataSource->selectedTab() - 1);
    }
    return true;
  }
  if (event == Ion::Events::Right) {
    if (m_dataSource->selectedTab() < m_numberOfChildren - 1) {
      setSelectedTab(m_dataSource->selectedTab() + 1);
    }
    return true;
  }
  if (event == Ion::Events::Down) {
    setActiveTab(m_dataSource->activeTab());
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    setActiveTab(m_dataSource->selectedTab());
    return true;
  }
  return false;
}

void TabViewController::setActiveTab(int8_t i, bool enter) {
  assert(i >= 0 && i < m_numberOfChildren);
  ViewController* activeVC = m_children[i];
  if (i != m_dataSource->activeTab()) {
    m_view.setActiveView(activeVC->view());
    m_children[i]->viewWillAppear();
    m_view.m_tabView.setActiveIndex(i);
  }
  if (i != m_dataSource->activeTab()) {
    m_children[m_dataSource->activeTab()]->viewDidDisappear();
    m_dataSource->setActiveTab(i);
  }
  /* If enter is false, we switch to the ith tab but the focus stays on the tab
   * button. It is useful when pressing Back on a non-leftmost tab. */
  if (enter) {
    m_isSelected = false;
    Container::activeApp()->setFirstResponder(activeVC);
  }
}

void TabViewController::setSelectedTab(int8_t i) {
  if (i == m_dataSource->selectedTab()) {
    return;
  }
  m_view.m_tabView.setSelectedIndex(i);
  m_dataSource->setSelectedTab(i);
}

void TabViewController::didBecomeFirstResponder() {
  setSelectedTab(m_dataSource->activeTab());
  setActiveTab(m_dataSource->selectedTab(), !m_isSelected);
}

void TabViewController::willResignFirstResponder() { setSelectedTab(-1); }

View* TabViewController::view() { return &m_view; }

uint8_t TabViewController::numberOfTabs() { return m_numberOfChildren; }

const char* TabViewController::tabName(uint8_t index) {
  return m_children[index]->title();
}

void TabViewController::initView() {
  for (int i = 0; i < m_numberOfChildren; i++) {
    m_view.m_tabView.addTab(m_children[i]);
    m_children[i]->initView();
  }
}

void TabViewController::viewWillAppear() {
  if (m_dataSource->activeTab() < 0) {
    m_dataSource->setActiveTab(0);
  }
  m_view.setActiveView(m_children[m_dataSource->activeTab()]->view());
  activeViewController()->viewWillAppear();
  m_view.m_tabView.setActiveIndex(m_dataSource->activeTab());
}

void TabViewController::viewDidDisappear() {
  activeViewController()->viewDidDisappear();
}

ViewController* TabViewController::activeViewController() {
  assert(m_dataSource->activeTab() >= 0 &&
         m_dataSource->activeTab() < m_numberOfChildren);
  return m_children[m_dataSource->activeTab()];
}

}  // namespace Escher
