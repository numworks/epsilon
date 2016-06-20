#ifndef ESCHER_TAB_VIEW_H
#define ESCHER_TAB_VIEW_H

#include <escher/view.h>
#include <escher/tab_view_cell.h>

class TabViewController;

class TabView : public View {
public:
  TabView();

  void drawRect(KDRect rect) const override;

  void addTabNamed(const char * name);
  //TODO: void removeLastTab();
  void setActiveIndex(int index);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  static constexpr uint8_t k_maxNumberOfTabs = 4;
  TabViewCell m_cells[k_maxNumberOfTabs];
  uint8_t m_numberOfTabs;
  uint8_t m_activeTabIndex;
};

#endif
