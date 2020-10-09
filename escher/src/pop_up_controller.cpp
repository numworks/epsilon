#include <escher/pop_up_controller.h>
#include <assert.h>

PopUpController::PopUpController(int numberOfLines, Invocation OkInvocation) :
  ViewController(nullptr),
  m_contentView(this, numberOfLines, OkInvocation)
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

PopUpController::ContentView::ContentView(Responder * parentResponder, int numberOfLines, Invocation okInvocation) :
  Responder(parentResponder),
  m_cancelButton(
    this, I18n::Message::Cancel,
    Invocation(
      [](void * context, void * sender) {
        Container::activeApp()->dismissModalViewController();
        return true;
      }, this),
    KDFont::SmallFont),
  m_okButton(this, I18n::Message::Ok, okInvocation, KDFont::SmallFont),
  m_warningTextView(KDFont::SmallFont, I18n::Message::Warning, 0.5, 0.5, KDColorWhite, KDColorBlack),
  m_numberOfLines(numberOfLines),
  m_messageTextViews{}
{
  assert(m_numberOfLines <= k_maxNumberOfLines && m_numberOfLines >= 0);
  for (int i = 0; i < m_numberOfLines; i++) {
    m_messageTextViews[i].setFont(KDFont::SmallFont);
    m_messageTextViews[i].setAlignment(0.5f, 0.5f);
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

void PopUpController::ContentView::setMessage(int index, I18n::Message message) {
  assert(index >=0 && index < m_numberOfLines);
  m_messageTextViews[index].setMessage(message);
}

int PopUpController::ContentView::numberOfSubviews() const {
  // MessageTextViews + WarningTextView + CancelButton + OkButton
  return m_numberOfLines + 3;
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
  return &m_messageTextViews[index-1];
}

void PopUpController::ContentView::layoutSubviews(bool force) {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_warningTextView.setFrame(KDRect(0, k_topMargin, width, textHeight), force);

  // Offset to center text vertically
  const int offset = (k_maxNumberOfLines - m_numberOfLines) / 2;

  for (int i = 0; i < m_numberOfLines; i++) {
    m_messageTextViews[i].setFrame(KDRect(0, k_topMargin + k_paragraphHeight + (i + 1 + offset) * textHeight, width, textHeight), force);
  }

  m_cancelButton.setFrame(KDRect(k_buttonMargin, height - k_buttonMargin - k_buttonHeight, (width - 3 * k_buttonMargin) / 2, k_buttonHeight), force);
  m_okButton.setFrame(KDRect(2 * k_buttonMargin + (width - 3 * k_buttonMargin) / 2, height - k_buttonMargin - k_buttonHeight, (width - 3 * k_buttonMargin) / 2, k_buttonHeight), force);
}
