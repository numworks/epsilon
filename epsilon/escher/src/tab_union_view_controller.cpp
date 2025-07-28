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
