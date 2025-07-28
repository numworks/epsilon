#include <escher/tab_view_data_source.h>

namespace Escher {

TabViewDataSource::TabViewDataSource()
    : m_activeChildIndex(-1), m_selectedChildIndex(-1) {}

int TabViewDataSource::activeTab() const { return m_activeChildIndex; }

int TabViewDataSource::selectedTab() const { return m_selectedChildIndex; }
void TabViewDataSource::setActiveTab(int i) { m_activeChildIndex = i; }

void TabViewDataSource::setSelectedTab(int i) { m_selectedChildIndex = i; }

}  // namespace Escher
