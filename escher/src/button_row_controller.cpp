#include <escher/button_row_controller.h>
#include <escher/palette.h>
#include <assert.h>
#include <math.h>

ButtonRowDelegate::ButtonRowDelegate(ButtonRowController * header, ButtonRowController * footer) :
  m_header(header),
  m_footer(footer)
{
}

int ButtonRowDelegate::numberOfButtons(ButtonRowController::Position position) const {
  return 0;
}

Button * ButtonRowDelegate::buttonAtIndex(int index, ButtonRowController::Position position) const {
  assert(false);
  return nullptr;
}

ButtonRowController * ButtonRowDelegate::footer() {
  return m_footer;
}

ButtonRowController * ButtonRowDelegate::header() {
  return m_header;
}

ButtonRowController::ContentView::ContentView(ViewController * mainViewController, ButtonRowDelegate * delegate, Position position, Style style) :
  View(),
  m_mainViewController(mainViewController),
  m_selectedButton(-1),
  m_delegate(delegate),
  m_position(position),
  m_style(style)
{
}

int ButtonRowController::ContentView::numberOfButtons() const {
  return m_delegate->numberOfButtons(m_position);
}

Button * ButtonRowController::ContentView::buttonAtIndex(int index) const {
  return m_delegate->buttonAtIndex(index, m_position);
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

void ButtonRowController::ContentView::layoutSubviews() {
  /* Position the main view */
  if (numberOfButtons() == 0) {
    KDCoordinate margin = m_position == Position::Top ? 1 : 0;
    KDRect mainViewFrame(0, margin, bounds().width(), bounds().height()-margin);
    m_mainViewController->view()->setFrame(mainViewFrame);
    return;
  }
  KDCoordinate rowHeight = m_style == Style::PlainWhite ? k_plainStyleHeight : k_embossedStyleHeight;
  KDCoordinate frameOrigin = m_position == Position::Top ? rowHeight+1 : 0;
  KDRect mainViewFrame(0, frameOrigin, bounds().width(), bounds().height() - rowHeight - 1);
    m_mainViewController->view()->setFrame(mainViewFrame);

  /* Position buttons */
  int nbOfButtons = numberOfButtons();
  KDCoordinate widthMargin = 0;
  KDCoordinate buttonHeightMargin = 0;
  KDCoordinate buttonHeight = rowHeight;
  if (m_style == Style::EmbossedGrey) {
    KDCoordinate totalButtonWidth = 0;
    for (int i = 0; i < nbOfButtons; i++) {
      Button * button = buttonAtIndex(i);
      totalButtonWidth += button->minimalSizeForOptimalDisplay().width();
    }
    widthMargin = roundf((bounds().width() - totalButtonWidth)/(nbOfButtons+1));
    buttonHeightMargin = k_embossedStyleHeightMargin;
    buttonHeight = rowHeight- 2*k_embossedStyleHeightMargin;
  }
  KDCoordinate yOrigin = m_position == Position::Top ? buttonHeightMargin : bounds().height()-rowHeight+buttonHeightMargin;
  int currentXOrigin = widthMargin;
  for (int i = 0; i < numberOfButtons(); i++) {
    Button * button = buttonAtIndex(i);
    KDCoordinate buttonWidth = button->minimalSizeForOptimalDisplay().width();
    KDRect buttonFrame(currentXOrigin, yOrigin, buttonWidth, buttonHeight);
    button->setFrame(buttonFrame);
    currentXOrigin += buttonWidth + widthMargin;
  }
}

void ButtonRowController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  if (numberOfButtons() == 0) {
      if (m_position == Position::Top) {
        ctx->fillRect(KDRect(0, 0, bounds().width(), 1), Palette::GreyWhite);
      }
    return;
  }
  if (m_style == Style::PlainWhite) {
    if (m_position == Position::Top) {
      ctx->fillRect(KDRect(0, 0, bounds().width(), k_plainStyleHeight), KDColorWhite);
      ctx->fillRect(KDRect(0, k_plainStyleHeight, bounds().width(), 1), Palette::GreyWhite);
    } else {
      ctx->fillRect(KDRect(0, bounds().height() - k_plainStyleHeight, bounds().width(), k_plainStyleHeight), KDColorWhite);
      ctx->fillRect(KDRect(0,  bounds().height() - k_plainStyleHeight-1, bounds().width(), 1), Palette::GreyWhite);
    }
    return;
  }
  if (m_position == Position::Top) {
    ctx->fillRect(KDRect(0, 0, bounds().width(), k_embossedStyleHeight), Palette::GreyWhite);
    ctx->fillRect(KDRect(0, k_embossedStyleHeight, bounds().width(), 1), Palette::GreyMiddle);
  } else {
    ctx->fillRect(KDRect(0, bounds().height() - k_embossedStyleHeight, bounds().width(), k_embossedStyleHeight), Palette::GreyWhite);
    ctx->fillRect(KDRect(0,  bounds().height() - k_embossedStyleHeight-1, bounds().width(), 1), Palette::GreyMiddle);
  }
  KDCoordinate y0 = m_position == Position::Top ? k_embossedStyleHeightMargin-1 : bounds().height()-k_embossedStyleHeight+k_embossedStyleHeightMargin-1;
  KDCoordinate y1 = m_position == Position::Top ? k_embossedStyleHeight-k_embossedStyleHeightMargin-2 : bounds().height()-k_embossedStyleHeightMargin;
  KDCoordinate totalButtonWidth = 0;
  for (int i = 0; i < numberOfButtons(); i++) {
    Button * button = buttonAtIndex(i);
    totalButtonWidth += button->minimalSizeForOptimalDisplay().width();
  }
  KDCoordinate widthMargin = roundf((bounds().width() - totalButtonWidth)/(numberOfButtons()+1));

  int currentXOrigin = widthMargin-1;
  for (int i = 0; i < numberOfButtons(); i++) {
    Button * button = buttonAtIndex(i);
    KDCoordinate buttonWidth = button->minimalSizeForOptimalDisplay().width();
    ctx->fillRect(KDRect(currentXOrigin, y0, 1, y1-y0+1), Palette::GreyMiddle);
    ctx->fillRect(KDRect(currentXOrigin-1, y0, 1, y1-y0+2), Palette::GreyDark);
    ctx->fillRect(KDRect(currentXOrigin, y0, buttonWidth+2, 1), Palette::GreyMiddle);
    ctx->fillRect(KDRect(currentXOrigin, y1, buttonWidth+2, 1), Palette::GreyMiddle);
    ctx->fillRect(KDRect(currentXOrigin, y1+1, buttonWidth+2, 1), Palette::GreyDark);
    ctx->fillRect(KDRect(currentXOrigin+1+buttonWidth, y0, 1, y1-y0+1), Palette::GreyMiddle);
    currentXOrigin += buttonWidth + widthMargin;
  }
}

bool ButtonRowController::ContentView::setSelectedButton(int selectedButton, App * application) {
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
    application->setFirstResponder(button);
    return true;
  }
  return false;
}

int ButtonRowController::ContentView::selectedButton() {
  return m_selectedButton;
}

ViewController * ButtonRowController::ContentView::mainViewController() const {
  return m_mainViewController;
}

ButtonRowDelegate * ButtonRowController::ContentView::buttonRowDelegate() const {
  return m_delegate;
}

ButtonRowController::ButtonRowController(Responder * parentResponder, ViewController * mainViewController, ButtonRowDelegate * delegate, Position position, Style style) :
  ViewController(parentResponder),
  m_contentView(ContentView(mainViewController, delegate, position, style))
{
}

View * ButtonRowController::view() {
  return &m_contentView;
}

const char * ButtonRowController::title() {
  return m_contentView.mainViewController()->title();
}

void ButtonRowController::didBecomeFirstResponder(){
  app()->setFirstResponder(m_contentView.mainViewController());
}

bool ButtonRowController::setSelectedButton(int selectedButton) {
  App * application = app();
  return m_contentView.setSelectedButton(selectedButton, application);
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
  if (event == Ion::Events::OK) {
    return true;
  }
  return false;
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
