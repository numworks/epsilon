#include <escher/modal_view_controller.h>
#include <escher/app.h>
#include <assert.h>

ModalViewController::ContentView::ContentView() :
  View(),
  m_regularView(nullptr),
  m_currentModalView(nullptr),
  m_isDisplayingModal(false),
  m_verticalAlignment(0.0f),
  m_horizontalAlignment(0.0f)
{
}

void ModalViewController::ContentView::setMainView(View * regularView) {
  m_regularView = regularView;
}

int ModalViewController::ContentView::numberOfSubviews() const {
  if (m_isDisplayingModal) {
    return 2;
  }
  return 1;
}

View * ModalViewController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return m_regularView;
    case 1:
      if (numberOfSubviews() == 2) {
        return m_currentModalView;
      } else {
        assert(false);
        return nullptr;
      }
    default:
      assert(false);
      return nullptr;
  }
}

KDRect ModalViewController::ContentView::frame() {
  KDCoordinate modalHeight = m_currentModalView->minimalSizeForOptimalDisplay().height();
  modalHeight = modalHeight == 0 ? bounds().height() : modalHeight;
  KDCoordinate modalWidth = m_currentModalView->minimalSizeForOptimalDisplay().width();
  modalWidth = modalWidth == 0 ? bounds().width() : modalWidth;
  KDRect modalViewFrame(m_horizontalAlignment*(bounds().width()-modalWidth), m_verticalAlignment*(bounds().height()-modalHeight), 
     modalWidth, modalHeight);
  return modalViewFrame;
}

void ModalViewController::ContentView::layoutSubviews() {
  m_regularView->setFrame(bounds());
  if (numberOfSubviews() == 2) {
    m_currentModalView->setFrame(frame());
  }
}

void ModalViewController::ContentView::presentModalView(View * modalView, float verticalAlignment, float horizontalAlignment) {
  m_isDisplayingModal = true;
  m_currentModalView = modalView;
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(frame());
  layoutSubviews();
}

void ModalViewController::ContentView::dismissModalView() {
  m_isDisplayingModal = false;
  markRectAsDirty(frame());
  m_currentModalView = nullptr;
  layoutSubviews();
}

bool ModalViewController::ContentView::isDisplayingModal() const {
  return m_isDisplayingModal;
}

ModalViewController::ModalViewController(Responder * parentResponder, ViewController * child) :
  ViewController(parentResponder),
  m_contentView(),
  m_previousResponder(child),
  m_currentModalViewController(nullptr),
  m_regularViewController(child)
{
}

View * ModalViewController::view() {
  if (m_contentView.subviewAtIndex(0) == nullptr) {
    m_contentView.setMainView(m_regularViewController->view());
  }
  return &m_contentView;
}

bool ModalViewController::isDisplayingModal() {
  return m_contentView.isDisplayingModal();
}

const char * ModalViewController::title() const {
  return "Modal View Controller";
}

void ModalViewController::displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment) {
  m_currentModalViewController = vc;
  vc->setParentResponder(this);
  m_previousResponder = app()->firstResponder();
  app()->setFirstResponder(vc);
  m_contentView.presentModalView(vc->view(), verticalAlignment, horizontalAlignment);
}

void ModalViewController::dismissModalViewController() {
  m_currentModalViewController = nullptr;
  app()->setFirstResponder(m_previousResponder);
  m_contentView.dismissModalView();
}

void ModalViewController::didBecomeFirstResponder() {
  if (m_contentView.isDisplayingModal()) {
    app()->setFirstResponder(m_currentModalViewController);
  }
  app()->setFirstResponder(m_previousResponder);
}

bool ModalViewController::handleEvent(Ion::Events::Event event) {
  if (!m_contentView.isDisplayingModal()) {
    return false;
  }
  switch (event) {
    case Ion::Events::Event::ESC:
      dismissModalViewController();
      return true;
    default:
      return false;
  }
}
