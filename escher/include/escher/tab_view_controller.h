#ifndef ESCHER_TAB_VIEW_CONTROLLER_H
#define ESCHER_TAB_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/tab_view.h>

class TabViewController : public ViewController {
public:
  TabViewController(ViewController ** children);
  View * view() override;
  void handleKeyEvent(int key) override;

  void setActiveTab(uint8_t index);
  uint8_t numberOfTabs();

private:
  class ContentView : public View {
  public:
    ContentView(TabViewController * vc);

    int numberOfSubviews() override;
    View * subview(int index) override;
    void storeSubviewAtIndex(View * view, int index) override;
    void layoutSubviews() override;

    void setActiveView(View * view);
  private:
    TabView m_tabView;
    View * m_activeView;
  };

  ContentView m_view;

  ViewController ** m_children;
  uint8_t m_activeChildIndex;
};

#endif
