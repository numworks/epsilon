#ifndef ESCHER_TAB_VIEW_CONTROLLER_H
#define ESCHER_TAB_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include <escher/tab_view.h>

class TabViewController : public ViewController {
public:
  TabViewController(Responder * parentResponder, ViewController * one, ViewController * two);
  //TabViewController(ViewController ** children, uint8_t numberOfChildren);
  View * view() override;

  void setActiveTab(uint8_t index);
  uint8_t numberOfTabs();

  const char * tabName(uint8_t index);
  bool handleEvent(Ion::Events::Event event) override;
private:
  class ContentView : public View {
  public:
    ContentView();

    void setActiveView(View * view);
    TabView m_tabView;
  protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;

    View * m_activeView;
  };

  ContentView m_view;

  static constexpr uint8_t k_maxNumberOfChildren = 4;
  ViewController * m_children[k_maxNumberOfChildren];
  uint8_t m_numberOfChildren;
  int8_t m_activeChildIndex;
};

#endif
