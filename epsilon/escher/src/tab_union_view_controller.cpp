extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/tab_union_view_controller.h>

namespace Escher {

TabUnionViewController::TabUnionViewController(
    Responder* parentResponder, TabViewDataSource* dataSource,
    AbstractTabUnion* tabs, std::initializer_list<I18n::Message> titles)
    : TabViewController(parentResponder, dataSource, {}), m_tabs(tabs) {
  for (I18n::Message title : titles) {
    m_titles[m_numberOfChildren++] = title;
  }
  assert(m_numberOfChildren <= k_maxNumberOfChildren);
}

void TabUnionViewController::initView() {
  for (int i = 0; i < m_numberOfChildren; i++) {
    m_view.m_tabView.addTab(this);
  }
}

void TabUnionViewController::setActiveTab(int8_t i, bool enter) {
  assert(i >= 0 && i < m_numberOfChildren);

  bool dataSourceChangeActiveTab = i != m_dataSource->activeTab();

  if (dataSourceChangeActiveTab) {
    if (!m_isSelected) {
      App::app()->setFirstResponder(nullptr);
    }
    activeViewController()->viewDidDisappear();
    m_dataSource->setActiveTab(i);
    m_tabs->setActiveTab(i);
    setActiveChildren(i);
  }
  ViewController* activeVC = children(i);

  /* If enter is false, we switch to the ith tab but the focus stays on the tab
   * button. It is useful when pressing Back on a non-leftmost tab. */
  if (enter) {
    m_isSelected = false;
    App::app()->setFirstResponder(activeVC);
  }
}

void TabUnionViewController::updateUnionActiveTab() {
  m_tabs->setActiveTab(m_dataSource->activeTab());
}

}  // namespace Escher
