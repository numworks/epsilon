#include <escher/modal_view_controller.h>
#include <escher/container.h>
#include <assert.h>

namespace Escher {

ModalViewController::ContentView::ContentView() :
  View(),
  m_regularView(nullptr),
  m_currentModalView(nullptr),
  m_isDisplayingModal(false),
  m_verticalAlignment(0.0f),
  m_horizontalAlignment(0.0f),
  m_topMargin(0),
  m_leftMargin(0),
  m_bottomMargin(0),
  m_rightMargin(0)
{
}

void ModalViewController::ContentView::setMainView(View * regularView) {
  if (m_regularView != regularView) {
    m_regularView = regularView;
  }
}

int ModalViewController::ContentView::numberOfSubviews() const {
  return 1 + m_isDisplayingModal;
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

KDRect ModalViewController::ContentView::modalViewFrame() const {
  KDSize modalSize = m_isDisplayingModal ? m_currentModalView->minimalSizeForOptimalDisplay() : KDSize(0,0);
  KDCoordinate modalHeight = modalSize.height();
  modalHeight = modalHeight == 0 ? bounds().height()-m_topMargin-m_bottomMargin : modalHeight;
  KDCoordinate modalWidth = modalSize.width();
  modalWidth = modalWidth == 0 ? bounds().width()-m_leftMargin-m_rightMargin : modalWidth;
  KDRect modalViewFrame(m_leftMargin + m_horizontalAlignment*(bounds().width()-m_leftMargin-m_rightMargin-modalWidth),
    m_topMargin+m_verticalAlignment*(bounds().height()-m_topMargin-m_bottomMargin-modalHeight), modalWidth, modalHeight);
  return modalViewFrame;
}

void ModalViewController::ContentView::layoutSubviews(bool force) {
  assert(m_regularView != nullptr);
  if (m_isDisplayingModal) {
    assert(m_currentModalView != nullptr);
    KDRect modalFrame = modalViewFrame();
    m_regularView->setFrame(modalFrame == bounds() ? KDRectZero : bounds(), force);
    m_currentModalView->setFrame(modalFrame, force);
  } else {
    m_regularView->setFrame(bounds(), force);
    if (m_currentModalView) {
      m_currentModalView->setFrame(KDRectZero, force);
    }
  }
}

void ModalViewController::ContentView::presentModalView(View * modalView, float verticalAlignment, float horizontalAlignment,
  KDCoordinate topMargin, KDCoordinate leftMargin, KDCoordinate bottomMargin, KDCoordinate rightMargin) {
  m_isDisplayingModal = true;
  m_currentModalView = modalView;
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  m_topMargin = topMargin;
  m_leftMargin = leftMargin;
  m_bottomMargin = bottomMargin;
  m_rightMargin = rightMargin;
  layoutSubviews();
}

void ModalViewController::ContentView::dismissModalView() {
  m_isDisplayingModal = false;
  layoutSubviews();
  m_currentModalView->resetSuperview();
  m_currentModalView = nullptr;
}

bool ModalViewController::ContentView::isDisplayingModal() const {
  return m_isDisplayingModal;
}

void ModalViewController::ContentView::reload() {
  markRectAsDirty(bounds());
  layoutSubviews();
}

ModalViewController::ModalViewController(Responder * parentResponder, ViewController * child) :
  ViewController(parentResponder),
  m_previousResponder(child),
  m_currentModalViewController(nullptr),
  m_regularViewController(child)
{
}

View * ModalViewController::view() {
  return &m_contentView;
}

bool ModalViewController::isDisplayingModal() {
  return m_contentView.isDisplayingModal();
}

void ModalViewController::displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
  KDCoordinate topMargin, KDCoordinate leftMargin, KDCoordinate bottomMargin, KDCoordinate rightMargin) {
  m_currentModalViewController = vc;
  vc->setParentResponder(this);
  m_previousResponder = Container::activeApp()->firstResponder();
  m_currentModalViewController->initView();
  m_contentView.presentModalView(vc->view(), verticalAlignment, horizontalAlignment, topMargin, leftMargin, bottomMargin, rightMargin);
  m_currentModalViewController->viewWillAppear();
  Container::activeApp()->setFirstResponder(vc);
}

void ModalViewController::reloadModalViewController() {
  m_contentView.layoutSubviews();
}

void ModalViewController::dismissModalViewController() {
  if (!isDisplayingModal()) {
    return;
  }
  m_currentModalViewController->viewDidDisappear();
  Container::activeApp()->setFirstResponder(m_previousResponder);
  m_contentView.dismissModalView();
  m_currentModalViewController = nullptr;
}

void ModalViewController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(
    isDisplayingModal() ? m_currentModalViewController : m_regularViewController
  );
}

bool ModalViewController::handleEvent(Ion::Events::Event event) {
  if (!m_contentView.isDisplayingModal()) {
    return false;
  }
  if (event == Ion::Events::Back || event == Ion::Events::Home) {
    dismissModalViewController();
    /* If Home is pressed, dismiss the modal view but still
    * propagate the event so that the app is properly closed. */
    return event != Ion::Events::Home;
  }
  return false;
}

void ModalViewController::initView() {
  m_contentView.setMainView(m_regularViewController->view());
  m_regularViewController->initView();
}

void ModalViewController::viewWillAppear() {
  m_contentView.layoutSubviews();
  if (m_contentView.isDisplayingModal()) {
    m_currentModalViewController->viewWillAppear();
  }
  m_regularViewController->viewWillAppear();
}

void ModalViewController::viewDidDisappear() {
  if (m_contentView.isDisplayingModal()) {
    m_currentModalViewController->viewDidDisappear();
  }
  m_regularViewController->viewDidDisappear();
}

void ModalViewController::reloadView() {
  m_contentView.reload();
}

}
