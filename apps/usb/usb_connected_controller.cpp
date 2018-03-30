#include "usb_connected_controller.h"
#include "../apps_container.h"
#include <assert.h>

namespace USB {

static I18n::Message sUSBConnectedMessages[] = {
  I18n::Message::USBConnected,
  I18n::Message::ConnectedMessage1,
  I18n::Message::ConnectedMessage2,
  I18n::Message::ConnectedMessage3,
  I18n::Message::ConnectedMessage4,
  I18n::Message::ConnectedMessage5,
  I18n::Message::ConnectedMessage6};

static KDColor sUSBConnectedColors[] = {
  KDColorBlack,
  KDColorBlack,
  KDColorBlack,
  Palette::YellowDark,
  KDColorBlack,
  KDColorBlack,
  KDColorBlack};

USBConnectedController::ContentView::ContentView(I18n::Message * messages, KDColor * colors) :
  m_titleTextView(KDText::FontSize::Large, messages[0], 0.5f, 0.5f, colors[0]),
  m_messageTextView1(KDText::FontSize::Small, messages[1], 0.5f, 0.5f, colors[1]),
  m_messageTextView2(KDText::FontSize::Small, messages[2], 0.5f, 0.5f, colors[2]),
  m_messageTextView3(KDText::FontSize::Small, messages[3], 0.5f, 0.5f, colors[3]),
  m_messageTextView4(KDText::FontSize::Small, messages[4], 0.5f, 0.5f, colors[4]),
  m_messageTextView5(KDText::FontSize::Small, messages[5], 0.5f, 0.5f, colors[5]),
  m_messageTextView6(KDText::FontSize::Small, messages[6], 0.5f, 0.5f, colors[6])
{
}

void USBConnectedController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

View * USBConnectedController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_titleTextView;
    case 1:
      return &m_messageTextView1;
    case 2:
      return &m_messageTextView2;
    case 3:
      return &m_messageTextView3;
    case 4:
      return &m_messageTextView4;
    case 5:
      return &m_messageTextView5;
    case 6:
      return &m_messageTextView6;
    default:
      assert(false);
      return nullptr;
  }
}

void USBConnectedController::ContentView::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate titleHeight = m_titleTextView.minimalSizeForOptimalDisplay().height();
  KDCoordinate textHeight = KDText::charSize(KDText::FontSize::Small).height();
  m_titleTextView.setFrame(KDRect(0, Metric::MessageScreenTitleMargin, width, titleHeight));
  m_messageTextView1.setFrame(KDRect(0, Metric::MessageScreenParagraphHeight, width, textHeight));
  m_messageTextView2.setFrame(KDRect(0, Metric::MessageScreenParagraphHeight+textHeight, width, textHeight));
  m_messageTextView3.setFrame(KDRect(0, Metric::MessageScreenParagraphHeight+2*textHeight, width, textHeight));
  m_messageTextView4.setFrame(KDRect(0, Metric::MessageScreenParagraphHeight+3*textHeight+Metric::MessageScreenParagraphMargin, width, textHeight));
  m_messageTextView5.setFrame(KDRect(0, Metric::MessageScreenParagraphHeight+4*textHeight+Metric::MessageScreenParagraphMargin, width, textHeight));
  m_messageTextView6.setFrame(KDRect(0, Metric::MessageScreenParagraphHeight+5*textHeight+Metric::MessageScreenParagraphMargin, width, textHeight));
}

USBConnectedController::USBConnectedController() :
  ViewController(nullptr),
  m_contentView(sUSBConnectedMessages, sUSBConnectedColors)
{
}

}
