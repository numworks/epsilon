extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/tab_union_view_controller.h>

namespace Escher {

TabUnionViewController::TabUnionViewController(Responder* parentResponder,
                                               TabViewDataSource* dataSource,
                                               AbstractTabUnion* tabs,
                                               I18n::Message titleOne,
                                               I18n::Message titleTwo,
                                               I18n::Message titleThree)
    : TabViewController(parentResponder, dataSource, nullptr, nullptr, nullptr),
      m_tabs(tabs) {
  m_titles[0] = titleOne;
  m_titles[1] = titleTwo;
  m_titles[2] = titleThree;
  m_numberOfChildren = 3;
}

void TabUnionViewController::initView() {
  for (int i = 0; i < m_numberOfChildren; i++) {
    m_view.m_tabView.addTab(tabName(i));
  }
}

void TabUnionViewController::setActiveTab(int8_t i, bool enter) {
  assert(i >= 0 && i < m_numberOfChildren);
  if (i != m_dataSource->activeTab()) {
    if (!m_isSelected) {
      Container::activeApp()->setFirstResponder(nullptr);
    }
    children(m_dataSource->activeTab())->viewDidDisappear();
    m_tabs->setActiveTab(i);
  }
  ViewController* activeVC = children(i);
  if (i != m_dataSource->activeTab()) {
    m_dataSource->setActiveTab(i);
    m_view.setActiveView(activeVC->view());
    children(i)->viewWillAppear();
    m_view.m_tabView.setActiveIndex(i);
  }
  /* If enter is false, we switch to the ith tab but the focus stays on the tab
   * button. It is useful when pressing Back on a non-leftmost tab. */
  if (enter) {
    m_isSelected = false;
    Container::activeApp()->setFirstResponder(activeVC);
  }
}

void TabUnionViewController::viewWillAppear() {
  if (m_dataSource->activeTab() < 0) {
    m_dataSource->setActiveTab(0);
  }
  m_tabs->setActiveTab(m_dataSource->activeTab());
  m_view.setActiveView(children(m_dataSource->activeTab())->view());
  activeViewController()->viewWillAppear();
  m_view.m_tabView.setActiveIndex(m_dataSource->activeTab());
}

}  // namespace Escher
