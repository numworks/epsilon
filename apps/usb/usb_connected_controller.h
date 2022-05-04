#ifndef USB_USB_CONNECTED_CONTROLLER_H
#define USB_USB_CONNECTED_CONTROLLER_H

#include <escher.h>

namespace USB {

class USBConnectedController : public ViewController {
public:
  USBConnectedController();
  View * view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override { return false; };
private:
  class ContentView : public View {
  public:
    ContentView();
    void drawRect(KDContext * ctx, KDRect rect) const override;
  protected:
    int numberOfSubviews() const override { return k_numberOfUSBMessages; }
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
  private:
    constexpr static KDCoordinate k_titleMargin = 30;
    constexpr static KDCoordinate k_paragraphHeight = 80;
    constexpr static uint8_t k_numberOfUSBMessages = 8;
    MessageTextView m_messageTextViews[k_numberOfUSBMessages];
  };
  ContentView m_contentView;
};

}
#endif

