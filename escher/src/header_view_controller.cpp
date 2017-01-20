#include <escher/header_view_controller.h>
#include <assert.h>

constexpr KDColor HeaderViewController::ContentView::k_separatorHeaderColor;
constexpr KDColor HeaderViewController::ContentView::k_selectedBackgroundColor;

HeaderViewController::ContentView::ContentView(ViewController * mainViewController, HeaderViewDelegate * delegate) :
  View(),
  m_mainViewController(mainViewController),
  m_selectedButton(-1),
  m_delegate(delegate)
{
}

int HeaderViewController::ContentView::numberOfButtons() const {
  return m_delegate->numberOfButtons();
}

Button * HeaderViewController::ContentView::buttonAtIndex(int index) {
  return m_delegate->buttonAtIndex(index);
}

int HeaderViewController::ContentView::numberOfSubviews() const {
  return numberOfButtons() + 1;
}

View * HeaderViewController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return m_mainViewController->view();
  } else {
    return buttonAtIndex(index - 1);
  }
}

void HeaderViewController::ContentView::layoutSubviews() {
  if (numberOfButtons() == 0) {
    KDRect mainViewFrame(0, 1, bounds().width(), bounds().height() - 1);
    m_mainViewController->view()->setFrame(mainViewFrame);
    return;
  }
  KDRect mainViewFrame(0, k_headerHeight + 1, bounds().width(), bounds().height() - k_headerHeight - 1);
  m_mainViewController->view()->setFrame(mainViewFrame);
  int currentXOrigin = 0;
  for (int i = 0; i < numberOfButtons(); i++) {
    Button * button = buttonAtIndex(i);
    KDCoordinate buttonWidth = button->minimalSizeForOptimalDisplay().width();
    KDRect buttonFrame(currentXOrigin, 0, buttonWidth, k_headerHeight);
    button->setFrame(buttonFrame);
    currentXOrigin += buttonWidth;
  }
}

void HeaderViewController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  if (numberOfButtons() > 0) {
    ctx->fillRect(KDRect(0, 0, bounds().width(), k_headerHeight), KDColorWhite);
    ctx->fillRect(KDRect(0, k_headerHeight, bounds().width(), 1), k_separatorHeaderColor);
  } else {
    ctx->fillRect(KDRect(0, 0, bounds().width(), 1), k_separatorHeaderColor);
  }
}

bool HeaderViewController::ContentView::setSelectedButton(int selectedButton, App * application) {
  if (selectedButton < -1 || selectedButton >= numberOfButtons() || selectedButton == m_selectedButton) {
    return false;
  }
  if (m_selectedButton >= 0) {
    Button * button = buttonAtIndex(m_selectedButton);
    button->setBackgroundColor(KDColorWhite);
  }
  m_selectedButton = selectedButton;
  if (m_selectedButton >= 0) {
    Button * button = buttonAtIndex(selectedButton);
    button->setBackgroundColor(k_selectedBackgroundColor);
    application->setFirstResponder(button);
    return true;
  }
  return false;
}

int HeaderViewController::ContentView::selectedButton() {
  return m_selectedButton;
}

ViewController * HeaderViewController::ContentView::mainViewController() const {
  return m_mainViewController;
}

HeaderViewDelegate * HeaderViewController::ContentView::headerViewDelegate() const {
  return m_delegate;
}

HeaderViewController::HeaderViewController(Responder * parentResponder, ViewController * mainViewController, HeaderViewDelegate * delegate) :
  ViewController(parentResponder),
  m_contentView(ContentView(mainViewController, delegate))
{
}

View * HeaderViewController::view() {
  return &m_contentView;
}

const char * HeaderViewController::title() const {
  return m_contentView.mainViewController()->title();
}

void HeaderViewController::didBecomeFirstResponder(){
  m_contentView.layoutSubviews();
  app()->setFirstResponder(m_contentView.mainViewController());
}

bool HeaderViewController::setSelectedButton(int selectedButton) {
  App * application = app();
  return m_contentView.setSelectedButton(selectedButton, application);
}

bool HeaderViewController::handleEvent(Ion::Events::Event event) {
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
