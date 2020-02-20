#ifndef HARDWARE_TEST_POP_UP_CONTROLLER_H
#define HARDWARE_TEST_POP_UP_CONTROLLER_H

#include <escher.h>

namespace HardwareTest {

class PopUpController : public ViewController {
public:
  PopUpController();
  View * view() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  class ContentView : public View, public Responder {
  public:
    ContentView(Responder * parentResponder);
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setSelectedButton(int selectedButton);
    int selectedButton();
  private:
    constexpr static KDCoordinate k_buttonMargin = 10;
    constexpr static KDCoordinate k_buttonHeight = 20;
    constexpr static KDCoordinate k_topMargin = 8;
    constexpr static KDCoordinate k_paragraphHeight = 20;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    Button m_cancelButton;
    Button m_okButton;
    MessageTextView m_warningTextView;
    MessageTextView m_messageTextView1;
    MessageTextView m_messageTextView2;
    MessageTextView m_messageTextView3;
    MessageTextView m_messageTextView4;
  };
  ContentView m_contentView;
};

}

#endif

