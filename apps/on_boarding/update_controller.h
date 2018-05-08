#ifndef ON_BOARDING_UPDATE_CONTROLLER_H
#define ON_BOARDING_UPDATE_CONTROLLER_H

#include <escher.h>
#include "../i18n.h"
#include "../shared/message_view.h"
#include "../shared/ok_view.h"

namespace OnBoarding {

class UpdateController : public ViewController {
public:
  UpdateController();
  View * view() override { return &m_messageViewWithSkip; }
  bool handleEvent(Ion::Events::Event event) override;
private:
  class MessageViewWithSkip : public MessageView {
  public:
    MessageViewWithSkip(I18n::Message * messages, KDColor * colors, uint8_t numberOfMessages);
  protected:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
  private:
    constexpr static KDCoordinate k_bottomMargin = 13;
    constexpr static KDCoordinate k_okMargin = 10;
    constexpr static KDCoordinate k_skipMargin = 4;
    MessageTextView m_skipView;
    Shared::OkView m_okView;
  };
  MessageViewWithSkip m_messageViewWithSkip;
};

}

#endif

