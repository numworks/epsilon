#ifndef ESCHER_POP_UP_CONTROLLER_H
#define ESCHER_POP_UP_CONTROLLER_H

#include <escher/button.h>
#include <escher/invocation.h>
#include <escher/view_controller.h>
#include <escher/i18n.h>

namespace Escher {

class HighContrastButton : public Button {
public:
  using Button::Button;
  KDColor highlightedBackgroundColor() const override { return Palette::YellowDark; }
};

class PopUpController : public ViewController {
public:
  PopUpController(int numberOfLines, Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage);
  View * view() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void setContentMessage(int index, I18n::Message message);
protected:
  class ContentView : public View, public Responder {
  public:
    ContentView(Responder * parentResponder, int numberOfLines, Invocation okInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage);
    void drawRect(KDContext * ctx, KDRect rect) const override { ctx->fillRect(bounds(), KDColorBlack); }
    void setSelectedButton(int selectedButton);
    int selectedButton();
    void setMessage(int index, I18n::Message message);
  private:
    constexpr static KDCoordinate k_buttonMargin = 10;
    constexpr static KDCoordinate k_buttonHeight = 20;
    constexpr static KDCoordinate k_topMargin = 8;
    constexpr static KDCoordinate k_paragraphHeight = 20;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    HighContrastButton m_cancelButton;
    HighContrastButton m_okButton;
    MessageTextView m_warningTextView;
    const int m_numberOfLines;
    constexpr static int k_maxNumberOfLines = 4;
    MessageTextView m_messageTextViews[k_maxNumberOfLines];
  };
  ContentView m_contentView;
};

}
#endif
