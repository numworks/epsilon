#include <escher/button_row_controller.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <cmath>

ButtonRowDelegate::ButtonRowDelegate(ButtonRowController * header, ButtonRowController * footer) :
  m_header(header),
  m_footer(footer)
{
}

ButtonRowController::ContentView::ContentView(ViewController * mainViewController, ButtonRowDelegate * delegate, Position position, Style style, Size size) :
  View(),
  m_mainViewController(mainViewController),
  m_selectedButton(-1),
  m_delegate(delegate),
  m_position(position),
  m_style(style),
  m_size(size)
{
}

int ButtonRowController::ContentView::numberOfButtons() const {
  return m_delegate->numberOfButtons(m_position);
}

Button * ButtonRowController::ContentView::buttonAtIndex(int index) const {
  return m_delegate->buttonAtIndex(index, m_position);
}

void ButtonRowController::ContentView::reload() {
  markRectAsDirty(bounds());
}

int ButtonRowController::ContentView::numberOfSubviews() const {
  return numberOfButtons() + 1;
}

View * ButtonRowController::ContentView::subviewAtIndex(int index) {
  /* Warning: the order of the subviews is important for drity tracking.
   * Indeed, when a child is redrawn, the redrawn area is the smallest
   * rectangle unioniong the dirty rectangle and the previous redrawn area.
   * As the main view is more likely to be bigger, we prefer to set it as the
   * last child. */
  if (index == numberOfSubviews() - 1) {
    return m_mainViewController->view();
  } else {
    return buttonAtIndex(index);
  }
}

void ButtonRowController::ContentView::layoutSubviews(bool force) {
  /* Position the main view */
  if (numberOfButtons() == 0) {
    KDCoordinate margin = m_position == Position::Top ? 1 : 0;
    KDRect mainViewFrame(0, margin, bounds().width(), bounds().height()-margin);
    m_mainViewController->view()->setFrame(mainViewFrame, force);
    return;
  }
  KDCoordinate rowHeight;
  if (m_style == Style::PlainWhite) {
    rowHeight = k_plainStyleHeight;
  } else {
    rowHeight = m_size == Size::Small ? k_embossedStyleHeightSmall : k_embossedStyleHeightLarge;
  }
  KDCoordinate frameOrigin = m_position == Position::Top ? rowHeight+1 : 0;
  KDRect mainViewFrame(0, frameOrigin, bounds().width(), bounds().height() - rowHeight - 1);
    m_mainViewController->view()->setFrame(mainViewFrame, force);

  /* Position buttons */
  int nbOfButtons = numberOfButtons();
  KDCoordinate widthMargin = 0;
  KDCoordinate buttonHeightMargin = 0;
  KDCoordinate buttonHeight = rowHeight;
  if (m_style == Style::EmbossedGray) {
    KDCoordinate totalButtonWidth = 0;
    for (int i = 0; i < nbOfButtons; i++) {
      Button * button = buttonAtIndex(i);
      totalButtonWidth += button->minimalSizeForOptimalDisplay().width();
    }
    widthMargin = std::round(((float)(bounds().width() - totalButtonWidth))/((float)(nbOfButtons+1)));
    buttonHeightMargin = m_size == Size::Small ? k_embossedStyleHeightMarginSmall : k_embossedStyleHeightMarginLarge;
    buttonHeight = rowHeight- 2*buttonHeightMargin;
  }
  KDCoordinate yOrigin = m_position == Position::Top ? buttonHeightMargin : bounds().height()-rowHeight+buttonHeightMargin;
  int currentXOrigin = widthMargin;
  for (int i = 0; i < numberOfButtons(); i++) {
    Button * button = buttonAtIndex(i);
    KDCoordinate buttonWidth = button->minimalSizeForOptimalDisplay().width();
    KDRect buttonFrame(currentXOrigin, yOrigin, buttonWidth, buttonHeight);
    button->setFrame(buttonFrame, force);
    currentXOrigin += buttonWidth + widthMargin;
  }
}

void ButtonRowController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  if (numberOfButtons() == 0) {
      if (m_position == Position::Top) {
        ctx->fillRect(KDRect(0, 0, bounds().width(), 1), Palette::ButtonBorderOut);
      }
    return;
  }
  if (m_style == Style::PlainWhite) {
    if (m_position == Position::Top) {
      ctx->fillRect(KDRect(0, 0, bounds().width(), k_plainStyleHeight), Palette::SubTabBackground);
      ctx->fillRect(KDRect(0, k_plainStyleHeight, bounds().width(), 1), Palette::ButtonBorderOut);
    } else {
      ctx->fillRect(KDRect(0, bounds().height() - k_plainStyleHeight, bounds().width(), k_plainStyleHeight), Palette::SubTabBackground);
      ctx->fillRect(KDRect(0, bounds().height() - k_plainStyleHeight-1, bounds().width(), 1), Palette::ButtonBorderOut);
    }
    return;
  }
  int buttonHeight = m_size == Size::Small ? k_embossedStyleHeightSmall : k_embossedStyleHeightLarge;
  int buttonMargin = m_size == Size::Small ? k_embossedStyleHeightMarginSmall : k_embossedStyleHeightMarginLarge;
  if (m_position == Position::Top) {
    ctx->fillRect(KDRect(0, 0, bounds().width(), buttonHeight), Palette::ButtonBorderOut);
    ctx->fillRect(KDRect(0, buttonHeight, bounds().width(), 1), Palette::ButtonRowBorder);
  } else {
    ctx->fillRect(KDRect(0, bounds().height() - buttonHeight, bounds().width(), buttonHeight), Palette::ButtonBorderOut);
    ctx->fillRect(KDRect(0,  bounds().height() - buttonHeight-1, bounds().width(), 1), Palette::ButtonRowBorder);
  }
  KDCoordinate y0 = m_position == Position::Top ? buttonMargin-1 : bounds().height()-buttonHeight+buttonMargin-1;
  KDCoordinate y1 = m_position == Position::Top ? buttonHeight-buttonMargin-2 : bounds().height()-buttonMargin;
  KDCoordinate totalButtonWidth = 0;
  for (int i = 0; i < numberOfButtons(); i++) {
    Button * button = buttonAtIndex(i);
    totalButtonWidth += button->minimalSizeForOptimalDisplay().width();
  }
  KDCoordinate widthMargin = std::round(((float)(bounds().width() - totalButtonWidth))/((float)(numberOfButtons()+1)));

  int currentXOrigin = widthMargin-1;
  for (int i = 0; i < numberOfButtons(); i++) {
    Button * button = buttonAtIndex(i);
    KDCoordinate buttonWidth = button->minimalSizeForOptimalDisplay().width();
    ctx->fillRect(KDRect(currentXOrigin, y0, 1, y1-y0+1), Palette::ButtonRowBorder);
    ctx->fillRect(KDRect(currentXOrigin-1, y0, 1, y1-y0+2), Palette::SecondaryText);
    ctx->fillRect(KDRect(currentXOrigin, y0, buttonWidth+2, 1), Palette::ButtonRowBorder);
    ctx->fillRect(KDRect(currentXOrigin, y1, buttonWidth+2, 1), Palette::ButtonRowBorder);
    ctx->fillRect(KDRect(currentXOrigin, y1+1, buttonWidth+2, 1), Palette::SecondaryText);
    ctx->fillRect(KDRect(currentXOrigin+1+buttonWidth, y0, 1, y1-y0+1), Palette::ButtonRowBorder);
    currentXOrigin += buttonWidth + widthMargin;
  }
}

bool ButtonRowController::ContentView::setSelectedButton(int selectedButton) {
  if (selectedButton < -1 || selectedButton >= numberOfButtons() || selectedButton == m_selectedButton) {
    return false;
  }
  if (m_selectedButton >= 0) {
    Button * button = buttonAtIndex(m_selectedButton);
    button->setHighlighted(false);
  }
  m_selectedButton = selectedButton;
  if (m_selectedButton >= 0) {
    Button * button = buttonAtIndex(selectedButton);
    button->setHighlighted(true);
    Container::activeApp()->setFirstResponder(button);
    return true;
  }
  return false;
}

ButtonRowController::ButtonRowController(Responder * parentResponder, ViewController * mainViewController, ButtonRowDelegate * delegate, Position position, Style style, Size size) :
  ViewController(parentResponder),
  m_contentView(mainViewController, delegate, position, style, size)
{
}

const char * ButtonRowController::title() {
  return m_contentView.mainViewController()->title();
}

void ButtonRowController::didBecomeFirstResponder(){
  Container::activeApp()->setFirstResponder(m_contentView.mainViewController());
}

int ButtonRowController::selectedButton() {
  return m_contentView.selectedButton();
}

bool ButtonRowController::setSelectedButton(int selectedButton) {
  return m_contentView.setSelectedButton(selectedButton);
}

void ButtonRowController::setMessageOfButtonAtIndex(I18n::Message message, int index) {
  if (m_contentView.numberOfButtons() > index) {
    m_contentView.buttonAtIndex(0)->setMessage(message);
    m_contentView.layoutSubviews();
    m_contentView.reload();
  }
}

bool ButtonRowController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    if (m_contentView.selectedButton() == 0) {
    } else {
      setSelectedButton(m_contentView.selectedButton() - 1);
    }
    return true;
  }
  if (event == Ion::Events::Right) {
    if (m_contentView.selectedButton() == - 1) {
    } else {
      setSelectedButton(m_contentView.selectedButton() + 1);
    }
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return true;
  }
  return false;
}

void ButtonRowController::initView() {
  m_contentView.mainViewController()->initView();
}

void ButtonRowController::viewWillAppear() {
  /* We need to layout subviews at first appearance because the number of
   * buttons might have changed between 2 appearences. */
  m_contentView.layoutSubviews();
  m_contentView.mainViewController()->viewWillAppear();
}

void ButtonRowController::viewDidDisappear() {
  m_contentView.mainViewController()->viewDidDisappear();
}
