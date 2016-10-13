#include <escher/header_view_controller.h>
#include <assert.h>

constexpr KDColor HeaderViewController::ContentView::k_separatorHeaderColor;
constexpr KDColor HeaderViewController::ContentView::k_selectedBackgroundColor;

HeaderViewController::ContentView::ContentView(View * subview, Responder * parentResponder) :
  View(),
  m_buttonOne(Button(parentResponder)),
  m_buttonTwo(Button(parentResponder)),
  m_buttonThree(Button(parentResponder)),
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
    KDCoordinate buttonOneWidth = m_buttonOne.minimalSizeForOptimalDisplay().width();
    KDCoordinate buttonTwoWidth = m_buttonTwo.minimalSizeForOptimalDisplay().width();
    KDCoordinate buttonThreeWidth = m_buttonThree.minimalSizeForOptimalDisplay().width();
    KDRect buttonOneFrame(0, 0, buttonOneWidth, k_headerHeight);
    KDRect buttonTwoFrame(buttonOneWidth, 0, buttonTwoWidth, k_headerHeight);
    KDRect buttonThreeFrame(buttonOneWidth + buttonTwoWidth, 0, buttonThreeWidth, k_headerHeight);
    switch(m_numberOfButtons) {
      case 3:
        m_buttonThree.setFrame(buttonThreeFrame);
      case 2:
        m_buttonTwo.setFrame(buttonTwoFrame);
      case 1:
        m_buttonOne.setFrame(buttonOneFrame);
      case 0:
        m_mainView->setFrame(mainViewFrame);
        return;
      default:
        assert(false);
        return;
    }
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

void HeaderViewController::ContentView::setButtonActions(Invocation buttonOneAction, Invocation buttonTwoAction, Invocation buttonThreeAction) {
  m_buttonOne.setInvocation(buttonOneAction);
  m_buttonTwo.setInvocation(buttonTwoAction);
  m_buttonThree.setInvocation(buttonThreeAction);
}

void HeaderViewController::ContentView::setSelectedButton(int selectedButton, App * application) {
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
      application->setFirstResponder(&m_buttonOne);
      break;
    case 1:
      m_buttonTwo.setBackgroundColor(k_selectedBackgroundColor);
      application->setFirstResponder(&m_buttonTwo);
      break;
    case 2:
      m_buttonThree.setBackgroundColor(k_selectedBackgroundColor);
      application->setFirstResponder(&m_buttonThree);
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
  m_contentView(ContentView(mainView, this))
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

void HeaderViewController::setButtonActions(Invocation buttonOneAction, Invocation buttonTwoAction, Invocation buttonThreeAction) {
  m_contentView.setButtonActions(buttonOneAction, buttonTwoAction, buttonThreeAction);
}

void HeaderViewController::setSelectedButton(int selectedButton) {
  App * application = app();
  m_contentView.setSelectedButton(selectedButton, application);
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
