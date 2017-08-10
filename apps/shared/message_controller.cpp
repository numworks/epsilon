#include "message_controller.h"
#include "../apps_container.h"
#include <assert.h>

MessageController::MessageController(I18n::Message * messages, KDColor * colors) :
  ViewController(nullptr),
  m_contentView(messages, colors)
{
}

View * MessageController::view() {
  return &m_contentView;
}

bool MessageController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Back && event != Ion::Events::OnOff && event != Ion::Events::Home) {
    app()->dismissModalViewController();
    return true;
  }
  return false;
}

MessageController::ContentView::ContentView(I18n::Message * messages, KDColor * colors) :
  m_titleTextView(KDText::FontSize::Large, messages[0], 0.5f, 0.5f, colors[0]),
  m_messageTextView1(KDText::FontSize::Small, messages[1], 0.5f, 0.5f, colors[1]),
  m_messageTextView2(KDText::FontSize::Small, messages[2], 0.5f, 0.5f, colors[2]),
  m_messageTextView3(KDText::FontSize::Small, messages[3], 0.5f, 0.5f, colors[3]),
  m_messageTextView4(KDText::FontSize::Small, messages[4], 0.5f, 0.5f, colors[4]),
  m_skipView(KDText::FontSize::Small, I18n::Message::Skip, 1.0f, 0.5f),
  m_okView()
{
}

void MessageController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

int MessageController::ContentView::numberOfSubviews() const {
  return 7;
}

View * MessageController::ContentView::subviewAtIndex(int index) {
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
      return &m_skipView;
    case 6:
      return &m_okView;
    default:
      assert(false);
      return nullptr;
  }
}

void MessageController::ContentView::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate titleHeight = m_titleTextView.minimalSizeForOptimalDisplay().height();
  KDCoordinate textHeight = KDText::charSize(KDText::FontSize::Small).height();
  m_titleTextView.setFrame(KDRect(0, k_titleMargin, width, titleHeight));
  m_messageTextView1.setFrame(KDRect(0, k_paragraphHeight, width, textHeight));
  m_messageTextView2.setFrame(KDRect(0, k_paragraphHeight+textHeight, width, textHeight));
  m_messageTextView3.setFrame(KDRect(0, k_paragraphHeight+2*textHeight+k_paragraphMargin, width, textHeight));
  m_messageTextView4.setFrame(KDRect(0, k_paragraphHeight+3*textHeight+k_paragraphMargin, width, textHeight));
  KDSize okSize = m_okView.minimalSizeForOptimalDisplay();
  m_skipView.setFrame(KDRect(0, height-k_bottomMargin-textHeight, width-okSize.width()-k_okMargin-k_skipMargin, textHeight));
  m_okView.setFrame(KDRect(width - okSize.width()-k_okMargin, height-okSize.height()-k_okMargin, okSize));
}
