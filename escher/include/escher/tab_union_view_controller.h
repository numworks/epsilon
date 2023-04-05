#ifndef ESCHER_TAB_UNION_VIEW_CONTROLLER_H
#define ESCHER_TAB_UNION_VIEW_CONTROLLER_H

#include <escher/tab_view_controller.h>

namespace Escher {

class Tab {
 public:
  virtual ViewController* top() = 0;
  virtual ~Tab() = default;
};

class AbstractTabUnion {
 public:
  virtual void setActiveTab(int index) = 0;
  virtual Tab* tab(int index) = 0;
};

// this is a simplified std::variant-like class to hold three tabs
template <class T0, class T1, class T2>
class TabUnion : public AbstractTabUnion {
 public:
  TabUnion() : m_activeTab(-1) {}
  ~TabUnion() {
    if (m_activeTab != -1) {
      TabUnion::tab(m_activeTab)->~Tab();
    }
  }

  int activeTab() const { return m_activeTab; }

  void setActiveTab(int index) override {
    if (m_activeTab == index) {
      return;
    }
    if (m_activeTab != -1) {
      TabUnion::tab(m_activeTab)->~Tab();
    }
    m_activeTab = index;
    switch (index) {
      case 0:
        new (&m_0) T0();
        return;
      case 1:
        new (&m_1) T1();
        return;
      default:
        assert(index == 2);
        new (&m_2) T2();
        return;
    }
  }

  Tab* tab(int index) override final {
    assert(m_activeTab == index);
    switch (index) {
      case 0:
        return static_cast<Tab*>(&m_0);
      case 1:
        return static_cast<Tab*>(&m_1);
      default:
        assert(index == 2);
        return static_cast<Tab*>(&m_2);
    }
  }

  template <class T>
  T* tab() {
    if constexpr (std::is_same_v<T, T0>) {
      assert(m_activeTab == 0);
      return &m_0;
    } else if constexpr (std::is_same_v<T, T1>) {
      assert(m_activeTab == 1);
      return &m_1;
    } else {
      static_assert(std::is_same_v<T, T2>);
      assert(m_activeTab == 2);
      return &m_2;
    }
  }

 private:
  union {
    T0 m_0;
    T1 m_1;
    T2 m_2;
  };
  int8_t m_activeTab;
};

class TabUnionViewController : public TabViewController {
 public:
  TabUnionViewController(Responder* parentResponder,
                         TabViewDataSource* dataSource, AbstractTabUnion* tabs,
                         I18n::Message titleOne, I18n::Message titleTwo,
                         I18n::Message titleThree);

  template <class T0, class T1, class T2>
  TabUnionViewController(Responder* parentResponder,
                         TabViewDataSource* dataSource,
                         TabUnion<T0, T1, T2>* tabs)
      : TabUnionViewController(parentResponder, dataSource, tabs, T0::k_title,
                               T1::k_title, T2::k_title) {}

  void setActiveTab(int8_t index, bool enter = true) override;
  void initView() override;
  const char* tabName(uint8_t index) override {
    return I18n::translate(m_titles[index]);
  };
  void viewWillAppear() override;

 private:
  ViewController* children(uint8_t index) override {
    return m_tabs->tab(index)->top();
  }
  I18n::Message m_titles[3];
  AbstractTabUnion* m_tabs;
};

}  // namespace Escher
#endif
