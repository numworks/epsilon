#ifndef ESCHER_TAB_VIEW_CONTROLLER_H
#define ESCHER_TAB_VIEW_CONTROLLER_H

#include <escher/app.h>
#include <escher/palette.h>
#include <escher/tab_view.h>
#include <escher/tab_view_data_source.h>
#include <escher/view_controller.h>

namespace Escher {

class TabViewController : public ViewController {
 public:
  TabViewController(Responder* parentResponder, TabViewDataSource* dataSource,
                    ViewController* one, ViewController* two,
                    ViewController* three);
  View* view() override;
  int activeTab() const;
  void selectTab();
  void setSelectedTab(int8_t index);
  virtual void setActiveTab(int8_t index, bool enter = true);
  void setDisplayTabs(bool display) { m_view.setDisplayTabs(display); }
  void enterActiveTab() { setActiveTab(activeTab()); }
  uint8_t numberOfTabs();

  virtual const char* tabName(uint8_t index);
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willResignFirstResponder() override;
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  ViewController* activeViewController();
  virtual KDColor tabBackgroundColor() const { return Palette::PurpleBright; }

 protected:
  class ContentView : public View {
   public:
    ContentView();

    void setActiveView(View* view);
    void setDisplayTabs(bool display);
    TabView m_tabView;

   protected:
#if ESCHER_VIEW_LOGGING
    const char* className() const override;
#endif
   private:
    int numberOfSubviews() const override;
    View* subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;

    View* m_activeView;
    bool m_displayTabs;
  };

  ContentView m_view;

  virtual ViewController* children(uint8_t index) { return m_children[index]; }
  constexpr static uint8_t k_maxNumberOfChildren = 3;
  ViewController* m_children[k_maxNumberOfChildren];
  uint8_t m_numberOfChildren;
  TabViewDataSource* m_dataSource;
  bool m_isSelected;
};

}  // namespace Escher
#endif
