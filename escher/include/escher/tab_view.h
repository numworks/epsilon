#ifndef ESCHER_TAB_VIEW_H
#define ESCHER_TAB_VIEW_H

#include <escher/view.h>
#include <escher/tab_view_cell.h>

class TabViewController;

class TabView : public View {
public:
  // We'll ask the TabViewController for its controllers
  // and each controller know its name!
  // and also the active index !
  TabView(TabViewController * tabViewController);
  //void drawRect(KDRect rect) override;
  int numberOfSubviews() override;
  View * subview(int index) override;
  void layoutSubviews() override;
protected:
  void storeSubviewAtIndex(View * view, int index) override;
private:
  TabViewController * m_tabViewController;
  /*
  static constexpr uint8_t k_maxNumberOfTabs = 4;
  uint8_t m_numberOfTabs;
  TabViewCell m_cells[k_maxNumberOfTabs];
  */
};

#endif
