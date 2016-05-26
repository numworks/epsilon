#ifndef ESCHER_TAB_VIEW_H
#define ESCHER_TAB_VIEW_H

#include <escher/view.h>
#include <escher/tab_view_cell.h>

class TabViewController;

class TabView : public View {
public:
  TabView();
  int numberOfSubviews() override;
  View * subview(int index) override;
  void layoutSubviews() override;

  void addTabNamed(const char * name);
  //TODO: void removeLastTab();
  void setActiveIndex(int index);
protected:
  void storeSubviewAtIndex(View * view, int index) override;
private:
  static constexpr uint8_t k_maxNumberOfTabs = 4;
  TabViewCell m_cells[k_maxNumberOfTabs];
  uint8_t m_numberOfTabs;
  uint8_t m_activeTabIndex;
};

#endif
