#ifndef ESCHER_ALTERNATE_VIEW_CONTROLLER_H
#define ESCHER_ALTERNATE_VIEW_CONTROLLER_H

#include <escher/i18n.h>
#include <escher/view_controller.h>
#include <escher/responder.h>

namespace Escher {

class AlternateViewDelegate {
public:
  virtual ViewController * activeViewController() = 0;
};

class AlternateViewController : public ViewController {
public:
  AlternateViewController(Responder * parentResponder, AlternateViewDelegate * delegate, const char * title);
  View * view() override { return &m_contentView; }
  const char * title() override { return m_title; }
  ViewController::TitlesDisplay titlesDisplay() override { return TitlesDisplay::DisplayNoTitle; }
  void didBecomeFirstResponder() override;
  void initView() override { m_contentView.mainViewController()->initView(); }
  void viewWillAppear() override;
  void viewDidDisappear() override { m_contentView.mainViewController()->viewDidDisappear(); }
private:
  class ContentView : public View {
  public:
    ContentView(AlternateViewDelegate * delegate) : m_delegate(delegate) {}
    ViewController * mainViewController() const { return m_delegate->activeViewController(); }
    void layoutSubviews(bool force = false) override { mainViewController()->view()->setFrame(bounds(), force); }
  private:
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override { return mainViewController()->view(); }
    AlternateViewDelegate * m_delegate;
  };
  ContentView m_contentView;
  const char * m_title;
};


}
#endif
