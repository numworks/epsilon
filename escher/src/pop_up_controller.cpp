#include <escher/pop_up_controller.h>
#include <escher/metric.h>
#include <escher/container.h>
#include <assert.h>

namespace Escher {

// PopUpController
PopUpController::PopUpController(Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage, PopUpViewDelegate * delegate) :
  ViewController(nullptr),
  m_contentView(this, OkInvocation, warningMessage, okMessage, cancelMessage, delegate)
{ }

View * PopUpController::view() {
  return &m_contentView;
}

void PopUpController::didBecomeFirstResponder() {
  m_contentView.setSelectedButton(0);
}

bool PopUpController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left && m_contentView.selectedButton() == 1) {
    m_contentView.setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::Right && m_contentView.selectedButton() == 0) {
    m_contentView.setSelectedButton(1);
    return true;
  }
  return false;
}

void PopUpController::presentModally() {
  Escher::Container::activeApp()->displayModalViewController(this, 0.f, 0.f, Escher::Metric::PopUpTopMargin, Escher::Metric::PopUpRightMargin, Escher::Metric::PopUpBottomMargin, Escher::Metric::PopUpLeftMargin);
}


// PopUp Content View
PopUpController::ContentView::ContentView(Responder * parentResponder, Invocation okInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage, PopUpViewDelegate * delegate) :
  Responder(parentResponder),
  m_cancelButton(
    this, cancelMessage,
    Invocation(
      [](void * context, void * sender) {
        Container::activeApp()->dismissModalViewController();
        return true;
      }, this),
    KDFont::SmallFont),
  m_okButton(this, okMessage, okInvocation, KDFont::SmallFont),
  m_warningTextView(KDFont::SmallFont, warningMessage, KDContext::k_alignCenter, KDContext::k_alignCenter, KDColorWhite, KDColorBlack),
  m_delegate(delegate)
{ }

void PopUpController::ContentView::setSelectedButton(int selectedButton) {
  m_cancelButton.setHighlighted(selectedButton == 0);
  m_okButton.setHighlighted(selectedButton == 1);
  Container::activeApp()->setFirstResponder(selectedButton == 0 ? &m_cancelButton : &m_okButton);
}

int PopUpController::ContentView::selectedButton() {
  return m_cancelButton.isHighlighted() ? 0 : 1;
}

int PopUpController::ContentView::numberOfSubviews() const {
  // Text + WarningTextView + CancelButton + OkButton
  return m_delegate->numberOfLines() + 3;
}

View * PopUpController::ContentView::subviewAtIndex(int index) {
  int totalSubviews = numberOfSubviews();
  if (index < 0 || index >= totalSubviews) {
    assert(false);
    return nullptr;
  }
  if (index == 0) {
    return &m_warningTextView;
  }
  if (index == totalSubviews - 2) {
    return &m_cancelButton;
  }
  if (index == totalSubviews - 1) {
    return &m_okButton;
  }
  return m_delegate->textViewAtIndex(index-1);
}

void PopUpController::ContentView::layoutSubviews(bool force) {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_warningTextView.setFrame(KDRect(0, k_topMargin, width, textHeight), force);
  // Offset to center text vertically
  const int offset = (PopUpViewDelegate::k_maxNumberOfLines - m_delegate->numberOfLines()) / 2;
  for (int i = 0; i < m_delegate->numberOfLines(); i++) {
    TextView * textView =  m_delegate->textViewAtIndex(i);
    textView->setFont(KDFont::SmallFont);
    textView->setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    textView->setBackgroundColor(KDColorBlack);
    textView->setTextColor(KDColorWhite);
    textView->setFrame(KDRect(0, k_topMargin + k_paragraphHeight + (i + 1 + offset) * textHeight, width, textHeight), force);
  }
  m_cancelButton.setFrame(KDRect(k_buttonMargin, height - k_buttonMargin - k_buttonHeight, (width - 3 * k_buttonMargin) / 2, k_buttonHeight), force);
  m_okButton.setFrame(KDRect(2 * k_buttonMargin + (width - 3 * k_buttonMargin) / 2, height - k_buttonMargin - k_buttonHeight, (width - 3 * k_buttonMargin) / 2, k_buttonHeight), force);
}

// MessagePopUpController
MessagePopUpController::MessagePopUpController(int numberOfLines, Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage) :
    PopUpController(OkInvocation, warningMessage, okMessage, cancelMessage, this),
    PopUpViewDelegate(numberOfLines),
    m_messageTextViews{}
{ }

MessagePopUpController::MessagePopUpController(Escher::Invocation OkInvocation, std::initializer_list<I18n::Message> messages) :
  MessagePopUpController(messages.size(), OkInvocation, I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel) {
  int index = 0;
  for (I18n::Message message : messages) {
    setContentMessage(index++, message);
  }
}

void MessagePopUpController::setContentMessage(int index, I18n::Message message) {
  assert(index >=0 && index < m_numberOfLines);
  m_messageTextViews[index].setMessage(message);
}

// BufferPopUpController
BufferPopUpController::BufferPopUpController(int numberOfLines, Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage) :
    PopUpController(OkInvocation, warningMessage, okMessage, cancelMessage, this),
    PopUpViewDelegate(numberOfLines),
    m_bufferTextViews{}
{ }

BufferPopUpController::BufferPopUpController(Escher::Invocation OkInvocation, int numberOfLines) :
  BufferPopUpController(numberOfLines, OkInvocation, I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel)
{ }

void BufferPopUpController::setContentText(int index, const char * text) {
  assert(index >=0 && index < m_numberOfLines);
  m_bufferTextViews[index].setText(text);
}

void BufferPopUpController::setSimpleCustomContentText(int index, I18n::Message message, const char * string) {
  assert(index >=0 && index < m_numberOfLines);
  m_bufferTextViews[index].setSimpleCustomText(message, string);
}

}
