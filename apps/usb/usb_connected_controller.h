#ifndef USB_USB_CONNECTED_CONTROLLER_H
#define USB_USB_CONNECTED_CONTROLLER_H

#include <escher.h>

namespace USB {

class USBConnectedController : public ViewController {
public:
  USBConnectedController();
  View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override { return false; }
private:
  class ContentView : public View {
  public:
    ContentView(I18n::Message * messages, KDColor * colors);
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    int numberOfSubviews() const override { return 7; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    MessageTextView m_titleTextView;
    MessageTextView m_messageTextView1;
    MessageTextView m_messageTextView2;
    MessageTextView m_messageTextView3;
    MessageTextView m_messageTextView4;
    MessageTextView m_messageTextView5;
    MessageTextView m_messageTextView6;
  };
  ContentView m_contentView;
};

}

#endif

