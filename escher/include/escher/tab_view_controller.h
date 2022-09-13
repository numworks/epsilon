#ifndef ESCHER_TAB_VIEW_CONTROLLER_H
#define ESCHER_TAB_VIEW_CONTROLLER_H

#include <escher/palette.h>
#include <escher/tab_view.h>
#include <escher/tab_view_data_source.h>
#include <escher/view_controller.h>

namespace Escher {

class TabViewController : public ViewController {
public:
  TabViewController(Responder * parentResponder, TabViewDataSource * dataSource, ViewController * one, ViewController * two, ViewController * three, ViewController * four = nullptr);
  View * view() override;
  int activeTab() const;
  void setSelectedTab(int8_t index);
  void setActiveTab(int8_t index, bool enter = true);
  void setDisplayTabs(bool display) { m_view.setDisplayTabs(display); }
  uint8_t numberOfTabs();

  const char * tabName(uint8_t index);
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void didEnterResponderChain(Responder * previousResponder) override;
  void willResignFirstResponder() override;
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  virtual KDColor tabBackgroundColor() const { return Palette::PurpleBright; }

private:
  ViewController * activeViewController();
  class ContentView : public View {
  public:
    ContentView();

    void setActiveView(View * view);
    void setDisplayTabs(bool display);
    TabView m_tabView;
  protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    View * m_activeView;
    bool m_displayTabs;
  };

  ContentView m_view;

  constexpr static uint8_t k_maxNumberOfChildren = 4;
  ViewController * m_children[k_maxNumberOfChildren];
  uint8_t m_numberOfChildren;
  TabViewDataSource * m_dataSource;
};

}
#endif
