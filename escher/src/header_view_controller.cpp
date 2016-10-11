#include <escher/header_view_controller.h>
#include <assert.h>

constexpr KDColor HeaderViewController::ContentView::k_separatorHeaderColor;
constexpr KDColor HeaderViewController::ContentView::k_selectedBackgroundColor;

HeaderViewController::ContentView::ContentView(View * subview) :
  View(),
  m_buttonOne(TextView()),
  m_buttonTwo(TextView()),
  m_buttonThree(TextView()),
  m_numberOfButtons(0),
  m_mainView(subview),
  m_visibleHeader(true),
  m_selectedButton(-1)
{
}

int HeaderViewController::ContentView::numberOfSubviews() const {
  if (m_visibleHeader) {
    return 4;
  }
  return 1;
}

View * HeaderViewController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return m_mainView;
    case 1:
      return &m_buttonOne;
    case 2:
      return &m_buttonTwo;
    case 3:
      return &m_buttonThree;
    default:
      assert(false);
      return nullptr;
  }
}

void HeaderViewController::ContentView::layoutSubviews() {
  if (numberOfSubviews() == 1){
    KDRect mainViewFrame(0, 1, bounds().width(), bounds().height() - 1);
    m_mainView->setFrame(mainViewFrame);
  } else {
    KDRect mainViewFrame(0, k_headerHeight + 1, bounds().width(), bounds().height() - k_headerHeight - 1);
    m_mainView->setFrame(mainViewFrame);
    KDCoordinate buttonOneWidth = m_buttonOne.textSize().width();
    KDRect buttonOneFrame(0, 0, buttonOneWidth + k_buttonTextMargin, k_headerHeight);
    m_buttonOne.setFrame(buttonOneFrame);
    KDCoordinate buttonTwoWidth = m_buttonTwo.textSize().width();
    KDRect buttonTwoFrame(buttonOneWidth + k_buttonTextMargin, 0, buttonTwoWidth + k_buttonTextMargin, k_headerHeight);
    m_buttonTwo.setFrame(buttonTwoFrame);
    KDCoordinate buttonThreeWidth = m_buttonThree.textSize().width();
    KDRect buttonThreeFrame(buttonOneWidth + buttonTwoWidth + 2*k_buttonTextMargin, 0, buttonThreeWidth + k_buttonTextMargin, k_headerHeight);
    m_buttonThree.setFrame(buttonThreeFrame);
  }
}

void HeaderViewController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_visibleHeader) {
    ctx->fillRect(KDRect(0, 0, bounds().width(), k_headerHeight), KDColorWhite);
    ctx->fillRect(KDRect(0, k_headerHeight, bounds().width(), 1), k_separatorHeaderColor);
  } else {
    ctx->fillRect(KDRect(0, 0, bounds().width(), 1), k_separatorHeaderColor);
  }
}

void HeaderViewController::ContentView::setVisibleHeader(bool isVisibleHeader) {
  m_visibleHeader = isVisibleHeader;
  markRectAsDirty(KDRect(0, 0, bounds().width(), bounds().height()));
  layoutSubviews();
}

void HeaderViewController::ContentView::setButtonTitles(const char * buttonOneTitle, const char * buttonTwoTitle, const char * buttonThreeTitle) {
  m_numberOfButtons = (bool)buttonOneTitle + (bool)buttonTwoTitle + (bool)buttonThreeTitle;
  m_buttonOne.setText(buttonOneTitle);
  m_buttonTwo.setText(buttonTwoTitle);
  m_buttonThree.setText(buttonThreeTitle);
}

void HeaderViewController::ContentView::setSelectedButton(int selectedButton) {
  if (selectedButton < -1 || selectedButton >= m_numberOfButtons) {
    return;
  }
  switch (m_selectedButton) {
    case 0:
      m_buttonOne.setBackgroundColor(KDColorWhite);
      break;
    case 1:
      m_buttonTwo.setBackgroundColor(KDColorWhite);
      break;
    case 2:
      m_buttonThree.setBackgroundColor(KDColorWhite);
      break;
    default:
      break;
  }
  m_selectedButton = selectedButton;
  switch (m_selectedButton) {
    case 0:
      m_buttonOne.setBackgroundColor(k_selectedBackgroundColor);
      break;
    case 1:
      m_buttonTwo.setBackgroundColor(k_selectedBackgroundColor);
      break;
    case 2:
      m_buttonThree.setBackgroundColor(k_selectedBackgroundColor);
      break;
    default:
      break;
  }
}

int HeaderViewController::ContentView::selectedButton() {
  return m_selectedButton;
}

HeaderViewController::HeaderViewController(Responder * parentResponder, View * mainView) :
  ViewController(parentResponder),
  m_contentView(ContentView(mainView))
{
}

View * HeaderViewController::view() {
  return &m_contentView;
}

const char * HeaderViewController::title() const {
  return "HeaderViewController";
}

void HeaderViewController::setVisibleHeader(bool isVisibleHeader) {
  m_contentView.setVisibleHeader(isVisibleHeader);
}

void HeaderViewController::setButtonTitles(const char * buttonOneTitle, const char * buttonTwoTitle, const char * buttonThreeTitle) {
  m_contentView.setButtonTitles(buttonOneTitle, buttonTwoTitle, buttonThreeTitle);
}

void HeaderViewController::setSelectedButton(int selectedButton) {
  m_contentView.setSelectedButton(selectedButton);
}

bool HeaderViewController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::LEFT_ARROW:
      if (m_contentView.selectedButton() == 0) {
        return true;
      } else {
        setSelectedButton(m_contentView.selectedButton() - 1);
        return true;
      }
    case Ion::Events::Event::RIGHT_ARROW:
      setSelectedButton(m_contentView.selectedButton() + 1);
      return true;
    case Ion::Events::Event::ENTER:
      return true;
    default:
      return false;
  }
}
