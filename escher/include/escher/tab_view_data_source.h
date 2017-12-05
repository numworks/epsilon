#ifndef ESCHER_TAB_VIEW_DATA_SOURCE_H
#define ESCHER_TAB_VIEW_DATA_SOURCE_H

extern "C" {
#include <stdint.h>
}

class TabViewDataSource {
public:
  TabViewDataSource() : m_activeChildIndex(-1), m_selectedChildIndex(-1) {}
  int activeTab() const {
    return m_activeChildIndex;
  }
  int selectedTab() const {
    return m_selectedChildIndex;
  }
  void setSelectedTab(int index) {
    m_selectedChildIndex = index;
  }
  void setActiveTab(int index) {
    m_activeChildIndex = index;
  }
private:
  int8_t m_activeChildIndex;
  int8_t m_selectedChildIndex;
};

#endif
