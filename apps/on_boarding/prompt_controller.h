#ifndef ON_PROMPT_CONTROLLER_H
#define ON_PROMPT_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/view_controller.h>
#include "../shared/message_view.h"
#include "../shared/ok_view.h"

namespace OnBoarding {

class PromptController : public Escher::ViewController {
public:
  PromptController(const I18n::Message * messages, const KDColor * colors, uint8_t numberOfMessages);
  Escher::View * view() override { return &m_messageViewWithSkip; }
  bool handleEvent(Ion::Events::Event & event) override;
private:
  class MessageViewWithSkip : public Shared::MessageView {
  public:
    MessageViewWithSkip(const I18n::Message * messages, const KDColor * colors, uint8_t numberOfMessages);
  protected:
    int numberOfSubviews() const override;
    Escher::View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
  private:
    constexpr static KDCoordinate k_bottomMargin = 13;
    constexpr static KDCoordinate k_okMargin = 10;
    constexpr static KDCoordinate k_skipMargin = 4;
    Escher::MessageTextView m_skipView;
    Shared::OkView m_okView;
  };
  MessageViewWithSkip m_messageViewWithSkip;
};

}

#endif
