#ifndef ESCHER_BANK_VIEW_CONTROLLER_H
#define ESCHER_BANK_VIEW_CONTROLLER_H

#include <escher/view_controller.h>

namespace Escher {

class BankViewController : public ViewController {
public:
  BankViewController(Responder * parentViewController);

  virtual ViewController * childAtIndex(int i) = 0;
  virtual int numberOfChildren() = 0;

  void setActiveIndex(int i);
  int activeIndex() const { return m_activeIndex; }

  View * view() override { return &m_view; }
  void didBecomeFirstResponder() override;
  void initView() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  class ContentView : public View {
  public:
    ContentView(): View(), m_subview(nullptr) {}
    void setSubview(View * view);
  private:
    int numberOfSubviews() const override {
      return m_subview == nullptr ? 0 : 1;
    }
    View * subviewAtIndex(int index) override {
      assert(index == 0);
      return m_subview;
    }
    void layoutSubviews(bool force = false) override {
      m_subview->setFrame(bounds(), force);
    }
    View * m_subview;
  };
  ViewController * activeViewController() {
    assert(m_activeIndex >= 0 && m_activeIndex < numberOfChildren());
    return childAtIndex(m_activeIndex);
  }
  ContentView m_view;
  int m_activeIndex;
};

}

#endif
