#pragma once

extern "C" {
#include <stdint.h>
}

namespace Escher {

class TabViewDataSource {
 public:
  TabViewDataSource();
  int activeTab() const;
  int selectedTab() const;
  void setSelectedTab(int index);
  void setActiveTab(int index);

 private:
  int8_t m_activeChildIndex;
  int8_t m_selectedChildIndex;
};

}  // namespace Escher
