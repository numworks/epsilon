#ifndef ESCHER_TAB_VIEW_CONTROLLER_H
#define ESCHER_TAB_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/tab_view.h>

class TabViewController : public ViewController {
public:
  TabViewController(ViewController ** children, uint8_t numberOfChildren);
  View * view() override;
  void handleKeyEvent(int key) override;

  void setActiveTab(uint8_t index);
  uint8_t numberOfTabs();

  const char * tabName(uint8_t index);
private:
  class ContentView : public View {
  public:
    ContentView();

    int numberOfSubviews() const override;
    const View * subview(int index) const override;
    void storeSubviewAtIndex(View * view, int index) override;
    void layoutSubviews() override;

    void setActiveView(View * view);
    TabView m_tabView;
  private:
    View * m_activeView;
  };

  ContentView m_view;

  ViewController ** m_children;
  uint8_t m_numberOfChildren;
  uint8_t m_activeChildIndex;
};

#endif
