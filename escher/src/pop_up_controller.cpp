#include <escher/pop_up_controller.h>
#include <escher/container.h>
#include <assert.h>

namespace Escher {

PopUpController::PopUpController(Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage, PopUpViewDelegate * delegate) :
  ViewController(nullptr),
  m_contentView(this, OkInvocation, warningMessage, okMessage, cancelMessage, delegate)
{
}

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

PopUpController::ContentView::ContentView(Responder * parentResponder, int numberOfLines, Invocation okInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage) :
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
  m_numberOfLines(numberOfLines),
  m_messageTextViews{}
{
  for (int i = 0; i < m_numberOfLines; i++) {
    m_messageTextViews[i].setFont(KDFont::SmallFont);
    m_messageTextViews[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_messageTextViews[i].setBackgroundColor(KDColorBlack);
    m_messageTextViews[i].setTextColor(KDColorWhite);
  }
}

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
  return m_delegate->textViewAtIndex[index-1];
}

void PopUpController::ContentView::layoutSubviews(bool force) {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_warningTextView.setFrame(KDRect(0, k_topMargin, width, textHeight), force);

  m_delegate->layoutSubView(force, width);
  /*for (int i = 0; i < m_numberOfLines; i++) {
    m_messageTextViews[i].setFrame(KDRect(0, k_topMargin + k_paragraphHeight + (i + 1 + offset) * textHeight, width, textHeight), force);
  }*/

  m_cancelButton.setFrame(KDRect(k_buttonMargin, height - k_buttonMargin - k_buttonHeight, (width - 3 * k_buttonMargin) / 2, k_buttonHeight), force);
  m_okButton.setFrame(KDRect(2 * k_buttonMargin + (width - 3 * k_buttonMargin) / 2, height - k_buttonMargin - k_buttonHeight, (width - 3 * k_buttonMargin) / 2, k_buttonHeight), force);
}

MessagePopUpController::MessagePopUpController(int numberOfLines, Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage);
    PopUpController(Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage, this),
    PopUpViewDelegate(numberOfLines),
    m_messageTextViews{}
{
  for (int i = 0; i < m_numberOfLines; i++) {
    m_messageTextViews[i].setFont(KDFont::SmallFont); // TODO : PUT FONT ALIGNMENT COLOR IN DELEGATE AS K_
    m_messageTextViews[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_messageTextViews[i].setBackgroundColor(KDColorBlack);
    m_messageTextViews[i].setTextColor(KDColorWhite);
  }
}

void MessagePopUpController::setContentMessage(int index, I18n::Message message) {
  assert(index >=0 && index < m_numberOfLines);
  m_messageTextViews[index].setMessage(message);
}

void BufferPopUpController::layoutSubviews(bool force, KDCoordinate width) {
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height(); // TAKE FONT FROM K_ see above
  // Offset to center text vertically
  const int offset = (k_maxNumberOfLines - m_numberOfLines) / 2;
  for (int i = 0; i < m_numberOfLines; i++) {
    m_bufferTextViews[i].setFrame(KDRect(0, ContentView::k_topMargin + ContentView::k_paragraphHeight + (i + 1 + offset) * textHeight, width, textHeight), force);
  }
}

BufferPopUpController::BufferPopUpController(int numberOfLines, Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage);
    PopUpController(Invocation OkInvocation, I18n::Message warningMessage, I18n::Message okMessage, I18n::Message cancelMessage, this),
    PopUpViewDelegate(numberOfLines),
    m_bufferTextViews{}
{
  for (int i = 0; i < m_numberOfLines; i++) {
    m_bufferTextViews[i].setFont(KDFont::SmallFont); // TODO : PUT FONT ALIGNMENT COLOR IN DELEGATE AS K_
    m_bufferTextViews[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
    m_bufferTextViews[i].setBackgroundColor(KDColorBlack);
    m_bufferTextViews[i].setTextColor(KDColorWhite);
  }
}

void BufferPopUpController::setContentText(int index, I18n::Message message) {
  assert(index >=0 && index < m_numberOfLines);
  m_messageTextViews[index].setText(message);
}

void BufferPopUpController::setSimpleCustomContentText(int index, I18n::Message message, const char * string) {
  assert(index >=0 && index < m_numberOfLines);
  m_messageTextViews[index].setSimpleCustomText(message, string);
}

void MessagePopUpController::layoutSubviews(bool force, KDCoordinate width) {
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height(); // TAKE FONT FROM K_ see above
  // Offset to center text vertically
  const int offset = (k_maxNumberOfLines - m_numberOfLines) / 2;
  for (int i = 0; i < m_numberOfLines; i++) {
    m_bufferTextViews[i].setFrame(KDRect(0, ContentView::k_topMargin + ContentView::k_paragraphHeight + (i + 1 + offset) * textHeight, width, textHeight), force);
  }
}


}
