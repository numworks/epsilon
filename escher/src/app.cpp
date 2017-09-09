#include <escher/app.h>
#include <escher/window.h>
extern "C" {
#include <assert.h>
}

const char * App::Descriptor::uriName() {
  return nullptr;
}

const I18n::Message *App::Descriptor::name() {
  return &I18n::NullMessage;
}

const I18n::Message *App::Descriptor::upperName() {
  return &I18n::NullMessage;
}

const Image * App::Descriptor::icon() {
  return nullptr;
}

static App::Snapshot::Register *s_registerListHead;

App::Snapshot::Register::Register(App::Snapshot *s) :
  m_listNext(s_registerListHead),
  m_snapshot(s)
{
  s_registerListHead = this;
}

App::Snapshot::Register* App::Snapshot::Register::getNext() {
  return m_listNext;
}

App::Snapshot* App::Snapshot::Register::getSnapshot() {
  return m_snapshot;
}

App::Snapshot::Register* App::Snapshot::Register::getList() {
  return s_registerListHead;
}

void App::Snapshot::pack(App * app) {
  tidy();
  delete app;
}

void App::Snapshot::reset() {
}

void App::Snapshot::tidy() {
}

App::App(Container * container, Snapshot * snapshot, ViewController * rootViewController, const I18n::Message *warningMessage) :
  Responder(nullptr),
  m_magic(Magic),
  m_modalViewController(this, rootViewController),
  m_container(container),
  m_firstResponder(nullptr),
  m_snapshot(snapshot),
  m_warningController(this, warningMessage)
{
}

App::Snapshot * App::snapshot() {
  return m_snapshot;
}

bool App::processEvent(Ion::Events::Event event) {
  Responder * responder = m_firstResponder;
  bool didHandleEvent = false;
  while (responder) {
    didHandleEvent = responder->handleEvent(event);
    if (didHandleEvent) {
      return true;
    }
    responder = responder->parentResponder();
  }
  return false;
}

Responder * App::firstResponder() {
  return m_firstResponder;
}

void App::setFirstResponder(Responder * responder) {
  Responder * previousResponder = m_firstResponder;
  m_firstResponder = responder;
  if (previousResponder) {
    Responder * commonAncestor = previousResponder->commonAncestorWith(m_firstResponder);
    Responder * leafResponder = previousResponder;
    while (leafResponder != commonAncestor) {
      leafResponder->willExitResponderChain(m_firstResponder);
      leafResponder = leafResponder->parentResponder();
    }
    previousResponder->willResignFirstResponder();
  }
  if (m_firstResponder) {
    Responder * commonAncestor = m_firstResponder->commonAncestorWith(previousResponder);
    Responder * leafResponder = m_firstResponder;
    while (leafResponder != commonAncestor) {
      leafResponder->didEnterResponderChain(previousResponder);
      leafResponder = leafResponder->parentResponder();
    }
    m_firstResponder->didBecomeFirstResponder();
  }
}

void App::displayModalViewController(ViewController * vc, float verticalAlignment, float horizontalAlignment,
    KDCoordinate topMargin, KDCoordinate leftMargin, KDCoordinate bottomMargin, KDCoordinate rightMargin) {
  if (m_modalViewController.isDisplayingModal()) {
    m_modalViewController.dismissModalViewController();
  }
  m_modalViewController.displayModalViewController(vc, verticalAlignment, horizontalAlignment, topMargin, leftMargin, bottomMargin, rightMargin);
}

void App::dismissModalViewController() {
  m_modalViewController.dismissModalViewController();
}

void App::displayWarning(const I18n::Message *warningMessage) {
  m_warningController.setLabel(warningMessage);
  m_modalViewController.displayModalViewController(&m_warningController, 0.5f, 0.5f);
}

const Container * App::container() const {
  return m_container;
}

void App::didBecomeActive(Window * window) {
  View * view = m_modalViewController.view();
  assert(m_modalViewController.app() == this);
  m_modalViewController.viewWillAppear();
  window->setContentView(view);
  setFirstResponder(&m_modalViewController);
}

void App::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    dismissModalViewController();
  }
  setFirstResponder(nullptr);
  m_modalViewController.viewDidDisappear();
}

int App::numberOfTimers() {
  return 0;
}

Timer * App::timerAtIndex(int i) {
  assert(false);
  return nullptr;
}
