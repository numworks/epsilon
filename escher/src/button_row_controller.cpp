#include <escher/button_row_controller.h>
#include <escher/container.h>
#include <escher/palette.h>

#include <cmath>

namespace Escher {

ButtonRowDelegate::ButtonRowDelegate(ButtonRowController* header,
                                     ButtonRowController* footer)
    : m_header(header), m_footer(footer) {}

ButtonRowController::ContentView::ContentView(
    ViewController* mainViewController, ButtonRowDelegate* delegate,
    Position position, Style style, Size size, KDCoordinate verticalMargin)
    : View(),
      m_mainViewController(mainViewController),
      m_selectedButton(-1),
      m_delegate(delegate),
      m_position(position),
      m_style(style),
      m_size(size),
      m_verticalMargin(verticalMargin) {}

int ButtonRowController::ContentView::numberOfButtons() const {
  return m_delegate->numberOfButtons(m_position);
}

ButtonCell* ButtonRowController::ContentView::buttonAtIndex(int index) const {
  return m_delegate->buttonAtIndex(index, m_position);
}

void ButtonRowController::ContentView::reload() { markWholeFrameAsDirty(); }

int ButtonRowController::ContentView::numberOfSubviews() const {
  return numberOfButtons() + 1;
}

View* ButtonRowController::ContentView::subviewAtIndex(int index) {
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

KDCoordinate ButtonRowController::ContentView::buttonRowHeight() const {
  if (m_style == Style::PlainWhite) {
    return Metric::ButtonRowPlainStyleHeight;
  }
  assert(m_style == Style::EmbossedGray);
  return m_size == Size::Small ? Metric::ButtonRowEmbossedStyleHeightSmall
                               : Metric::ButtonRowEmbossedStyleHeightLarge;
}

void ButtonRowController::ContentView::layoutSubviews(bool force) {
  /* Position the main view */
  if (numberOfButtons() == 0) {
    KDCoordinate margin = m_position == Position::Top ? 1 : 0;
    KDRect mainViewFrame(0, margin, bounds().width(),
                         bounds().height() - margin);
    setChildFrame(m_mainViewController->view(), mainViewFrame, force);
    return;
  }
  KDCoordinate rowHeight = buttonRowHeight();
  KDCoordinate fullHeight = m_verticalMargin + rowHeight + 1;
  KDCoordinate frameOrigin = m_position == Position::Top ? fullHeight : 0;
  KDRect mainViewFrame(0, frameOrigin, bounds().width(),
                       bounds().height() - fullHeight);
  setChildFrame(m_mainViewController->view(), mainViewFrame, force);

  /* Position buttons */
  int nbOfButtons = numberOfButtons();
  assert(nbOfButtons > 0);
  KDCoordinate widthMargin = 0;
  KDCoordinate heightMargin = 0;
  if (m_style == Style::EmbossedGray) {
    KDCoordinate buttonHeight =
        buttonAtIndex(0)->minimalSizeForOptimalDisplay().height();
    assert(0 < buttonHeight && buttonHeight <= rowHeight);
    heightMargin = (rowHeight - buttonHeight) / 2;
    KDCoordinate totalButtonWidth = 0;
    for (int i = 0; i < nbOfButtons; i++) {
      KDSize buttonSize = buttonAtIndex(i)->minimalSizeForOptimalDisplay();
      totalButtonWidth += buttonSize.width();
      assert(buttonSize.height() == buttonHeight);
    }
    widthMargin = std::round(((float)(bounds().width() - totalButtonWidth)) /
                             ((float)(nbOfButtons + 1)));
  } else {
    assert(m_style == Style::PlainWhite);
    assert(rowHeight ==
           buttonAtIndex(0)->minimalSizeForOptimalDisplay().height());
  }
  KDCoordinate fullHeightMargin = m_verticalMargin + heightMargin;
  KDCoordinate yOrigin = m_position == Position::Top
                             ? fullHeightMargin
                             : bounds().height() - rowHeight + fullHeightMargin;
  int currentXOrigin = widthMargin;
  for (int i = 0; i < nbOfButtons; i++) {
    ButtonCell* button = buttonAtIndex(i);
    KDSize buttonSize = button->minimalSizeForOptimalDisplay();
    KDRect buttonFrame(currentXOrigin, yOrigin, buttonSize.width(),
                       buttonSize.height());
    setChildFrame(button, buttonFrame, force);
    currentXOrigin += buttonSize.width() + widthMargin;
  }
}

void ButtonRowController::ContentView::drawRowFrame(KDContext* ctx,
                                                    KDCoordinate innerHeight,
                                                    KDColor backgroundColor,
                                                    KDColor borderColor) const {
  constexpr KDCoordinate k_borderHeight = 1;
  KDCoordinate height = innerHeight + m_verticalMargin;
  KDCoordinate y1, y2;
  if (m_position == Position::Top) {
    y1 = 0;
    y2 = height;
  } else {
    y1 = bounds().height() - height;
    y2 = bounds().height() - height - k_borderHeight;
  }
  ctx->fillRect(KDRect(0, y1, bounds().width(), height), backgroundColor);
  ctx->fillRect(KDRect(0, y2, bounds().width(), k_borderHeight), borderColor);
  return;
}

void ButtonRowController::ContentView::drawRect(KDContext* ctx,
                                                KDRect rect) const {
  if (numberOfButtons() == 0) {
    if (m_position == Position::Top) {
      ctx->fillRect(KDRect(0, 0, bounds().width(), 1), Palette::GrayWhite);
    }
    return;
  }
  KDCoordinate rowHeight = buttonRowHeight();
  if (m_style == Style::PlainWhite) {
    drawRowFrame(ctx, rowHeight, KDColorWhite, Palette::GrayWhite);
    return;
  }
  assert(m_style == Style::EmbossedGray);
  drawRowFrame(ctx, rowHeight, Palette::GrayWhite, Palette::GrayMiddle);
}

bool ButtonRowController::ContentView::setSelectedButton(int selectedButton) {
  /* WARNING:
   * selectedButton >= 0 is checked before selectedButton >= numberOfButtons()
   * to avoid calling numberOfButtons in some case.
   * For example, when the Graph app overflows the pool and return to home,
   * the selectedButton is set to -1.
   * Since numberOfButtons() depends on the number of active functions, which
   * depends on the reduced expression of each function, which computation is
   * what made the pool overflow in the first place, numberOfButtons() cannot
   * be called.
   * To avoid this scenario, the check for selectedButton >= numberOfButtons()
   * is skipped when selectedButton == -1.
   */
  if (selectedButton < -1 ||
      (selectedButton >= 0 && selectedButton >= numberOfButtons()) ||
      selectedButton == m_selectedButton) {
    return false;
  }
  if (m_selectedButton >= 0) {
    ButtonCell* button = buttonAtIndex(m_selectedButton);
    button->setHighlighted(false);
  }
  m_selectedButton = selectedButton;
  if (m_selectedButton >= 0) {
    ButtonCell* button = buttonAtIndex(selectedButton);
    button->setHighlighted(true);
    App::app()->setFirstResponder(button);
    return true;
  }
  return false;
}

ButtonRowController::ButtonRowController(Responder* parentResponder,
                                         ViewController* mainViewController,
                                         ButtonRowDelegate* delegate,
                                         Position position, Style style,
                                         Size size, KDCoordinate verticalMargin)
    : ViewController(parentResponder),
      m_contentView(mainViewController, delegate, position, style, size,
                    verticalMargin) {}

const char* ButtonRowController::title() const {
  return m_contentView.mainViewController()->title();
}

ViewController::TitlesDisplay ButtonRowController::titlesDisplay() const {
  return m_contentView.mainViewController()->titlesDisplay();
}

void ButtonRowController::didBecomeFirstResponder() {
  App::app()->setFirstResponder(m_contentView.mainViewController());
}

void ButtonRowController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::WillExit) {
    setSelectedButton(-1);
  }
}

int ButtonRowController::selectedButton() {
  return m_contentView.selectedButton();
}

bool ButtonRowController::setSelectedButton(int selectedButton) {
  return m_contentView.setSelectedButton(selectedButton);
}

void ButtonRowController::setMessageOfButtonAtIndex(I18n::Message message,
                                                    int index) {
  if (m_contentView.numberOfButtons() > index) {
    m_contentView.buttonAtIndex(0)->setMessage(message);
    m_contentView.layoutSubviews();
    m_contentView.reload();
  }
}

bool ButtonRowController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    if (m_contentView.selectedButton() != 0) {
      setSelectedButton(m_contentView.selectedButton() - 1);
    }
    return true;
  }
  if (event == Ion::Events::Right) {
    if (m_contentView.selectedButton() != -1) {
      setSelectedButton(m_contentView.selectedButton() + 1);
    }
    return true;
  }
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

void ButtonRowController::initView() {
  m_contentView.mainViewController()->initView();
}

void ButtonRowController::viewWillAppear() {
  /* We need to layout subviews at first appearance because the number of
   * buttons might have changed between 2 appearances. */
  m_contentView.layoutSubviews();
  m_contentView.mainViewController()->viewWillAppear();
}

void ButtonRowController::viewDidDisappear() {
  m_contentView.mainViewController()->viewDidDisappear();
}

void ButtonRowController::privateModalViewAltersFirstResponder(
    FirstResponderAlteration alteration) {
  if (alteration != FirstResponderAlteration::DidRestore) {
    return;
  }
  int n = m_contentView.numberOfButtons();
  Responder* firstResponder = App::app()->firstResponder();
  for (int i = 0; i < n; i++) {
    if (static_cast<Responder*>(m_contentView.buttonAtIndex(i)) ==
        firstResponder) {
      setSelectedButton(i);
      return;
    }
  }
}

}  // namespace Escher
