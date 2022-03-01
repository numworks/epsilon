#ifndef ESCHER_POP_UP_CONTROLLER_H
#define ESCHER_POP_UP_CONTROLLER_H

#include <escher/button.h>
#include <escher/invocation.h>
#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>
#include <escher/view_controller.h>
#include <escher/i18n.h>

namespace Escher {

class HighContrastButton : public Button {
public:
  using Button::Button;
  KDColor highlightedBackgroundColor() const override { return Palette::YellowDark; }
};

class PopUpViewDelegate {
public:
  constexpr static int k_maxNumberOfLines = 4;
  PopUpViewDelegate(int numberOfLines) : m_numberOfLines(numberOfLines) {
    assert(m_numberOfLines <= k_maxNumberOfLines && m_numberOfLines >= 0);
  }
  virtual TextView * textViewAtIndex(int i) = 0;
  int numberOfLines() { return m_numberOfLines; }
protected:
  const int m_numberOfLines;
};


class PopUpController : public ViewController {
public:
  PopUpController(Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage, PopUpViewDelegate * delegate);
  View * view() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
protected:
  class ContentView : public View, public Responder {
  public:
    ContentView(Responder * parentResponder, Invocation okInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage, PopUpViewDelegate * delegate);
    void drawRect(KDContext * ctx, KDRect rect) const override { ctx->fillRect(bounds(), KDColorBlack); }
    void setSelectedButton(int selectedButton);
    int selectedButton();
    constexpr static KDCoordinate k_buttonMargin = 10;
    constexpr static KDCoordinate k_buttonHeight = 20;
    constexpr static KDCoordinate k_topMargin = 8;
    constexpr static KDCoordinate k_paragraphHeight = 20;
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    HighContrastButton m_cancelButton;
    HighContrastButton m_okButton;
    MessageTextView m_warningTextView;
    PopUpViewDelegate * m_delegate;
  };
  ContentView m_contentView;
};

class MessagePopUpController : public PopUpController, public PopUpViewDelegate {
public:
  MessagePopUpController(int numberOfLines, Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage);
  TextView * textViewAtIndex(int i) override { return &m_messageTextViews[i]; }
  void setContentMessage(int index, I18n::Message message);
private:
  MessageTextView m_messageTextViews[k_maxNumberOfLines];
};

class BufferPopUpController : public PopUpController, public PopUpViewDelegate {
public:
  BufferPopUpController(int numberOfLines, Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage);
  TextView * textViewAtIndex(int i) override { return &m_bufferTextViews[i]; }
  void setContentText(int index, const char * text);
  void setSimpleCustomContentText(int index, I18n::Message message, const char * string = "");
private:
  BufferTextView m_bufferTextViews[k_maxNumberOfLines];
};

}

#endif
