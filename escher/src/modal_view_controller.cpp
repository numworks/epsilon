#include <escher/modal_view_controller.h>
#include <escher/app.h>
#include <assert.h>

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
  KDRect regularFrame = bounds();
  KDRect modalFrame = frame();
  bool shouldDrawTheRegularViewBehind = !modalFrame.contains(regularFrame.topLeft()) || !modalFrame.contains(regularFrame.bottomRight());
  return 1 + (m_isDisplayingModal && shouldDrawTheRegularViewBehind);
}

View * ModalViewController::ContentView::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      if (m_isDisplayingModal && numberOfSubviews() == 1) {
        return m_currentModalView;
      }
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

KDRect ModalViewController::ContentView::frame() const {
  KDSize modalSize = m_isDisplayingModal ? m_currentModalView->minimalSizeForOptimalDisplay() : KDSize(0,0);
  KDCoordinate modalHeight = modalSize.height();
  modalHeight = modalHeight == 0 ? bounds().height()-m_topMargin-m_bottomMargin : modalHeight;
  KDCoordinate modalWidth = modalSize.width();
  modalWidth = modalWidth == 0 ? bounds().width()-m_leftMargin-m_rightMargin : modalWidth;
  KDRect modalViewFrame(m_leftMargin + m_horizontalAlignment*(bounds().width()-m_leftMargin-m_rightMargin-modalWidth),
    m_topMargin+m_verticalAlignment*(bounds().height()-m_topMargin-m_bottomMargin-modalHeight), modalWidth, modalHeight);
  return modalViewFrame;
}

void ModalViewController::ContentView::layoutSubviews() {
  assert(m_regularView != nullptr);
  m_regularView->setFrame(bounds());
  if (m_isDisplayingModal) {
    assert(m_currentModalView != nullptr);
    m_currentModalView->setFrame(frame());
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
  markRectAsDirty(frame());
  layoutSubviews();
}

void ModalViewController::ContentView::dismissModalView() {
  m_isDisplayingModal = false;
  markRectAsDirty(frame());
  m_currentModalView->resetSuperview();
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
  return &m_contentView;
}

bool ModalViewController::isDisplayingModal() {
  return m_contentView.isDisplayingModal();
}

void ModalViewController::displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
  KDCoordinate topMargin, KDCoordinate leftMargin, KDCoordinate bottomMargin, KDCoordinate rightMargin) {
  m_currentModalViewController = vc;
  vc->setParentResponder(this);
  m_previousResponder = app()->firstResponder();
  m_contentView.presentModalView(vc->view(), verticalAlignment, horizontalAlignment, topMargin, leftMargin, bottomMargin, rightMargin);
  m_currentModalViewController->viewWillAppear();
  app()->setFirstResponder(vc);
}

void ModalViewController::dismissModalViewController() {
  m_currentModalViewController->viewDidDisappear();
  app()->setFirstResponder(m_previousResponder);
  m_contentView.dismissModalView();
  m_currentModalViewController = nullptr;
}

void ModalViewController::didBecomeFirstResponder() {
  if (m_contentView.isDisplayingModal()) {
    app()->setFirstResponder(m_currentModalViewController);
  }
  app()->setFirstResponder(m_regularViewController);
}

bool ModalViewController::handleEvent(Ion::Events::Event event) {
  if (!m_contentView.isDisplayingModal()) {
    return false;
  }
  if (event == Ion::Events::Back) {
    dismissModalViewController();
    return true;
  }
  return false;
}

void ModalViewController::viewWillAppear() {
  m_contentView.setMainView(m_regularViewController->view());
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
