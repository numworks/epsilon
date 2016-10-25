#include <escher/header_view_controller.h>
#include <assert.h>

constexpr KDColor HeaderViewController::ContentView::k_separatorHeaderColor;
constexpr KDColor HeaderViewController::ContentView::k_selectedBackgroundColor;

HeaderViewController::ContentView::ContentView(View * subview, HeaderViewController * headerViewController) :
  View(),
  m_mainView(subview),
  m_visibleHeader(true),
  m_selectedButton(-1),
  m_headerViewController(headerViewController)
{
}

int HeaderViewController::ContentView::numberOfButtons() const {
  return m_headerViewController->numberOfButtons();
}
Button * HeaderViewController::ContentView::buttonAtIndex(int index) {
  return m_headerViewController->buttonAtIndex(index);
}

int HeaderViewController::ContentView::numberOfSubviews() const {
  if (m_visibleHeader) {
    return numberOfButtons() + 1;
  }
  return 1;
}

View * HeaderViewController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return m_mainView;
  } else {
    return buttonAtIndex(index - 1);
  }
}

void HeaderViewController::ContentView::layoutSubviews() {
  if (numberOfButtons() == 0) {
    KDRect mainViewFrame(0, 1, bounds().width(), bounds().height() - 1);
    m_mainView->setFrame(mainViewFrame);
    return;
  }
  KDRect mainViewFrame(0, k_headerHeight + 1, bounds().width(), bounds().height() - k_headerHeight - 1);
  m_mainView->setFrame(mainViewFrame);
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

HeaderViewController::HeaderViewController(Responder * parentResponder, View * mainView) :
  ViewController(parentResponder),
  m_contentView(ContentView(mainView, this))
{
}

int HeaderViewController::numberOfButtons() const {
  return 0;
}

Button * HeaderViewController::buttonAtIndex(int index) {
  assert(false);
  return nullptr;
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

bool HeaderViewController::setSelectedButton(int selectedButton) {
  App * application = app();
  return m_contentView.setSelectedButton(selectedButton, application);
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
