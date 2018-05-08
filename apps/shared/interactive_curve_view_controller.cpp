#include "interactive_curve_view_controller.h"
#include "text_field_delegate_app.h"
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

InteractiveCurveViewController::InteractiveCurveViewController(Responder * parentResponder, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion) :
  SimpleInteractiveCurveViewController(parentResponder, interactiveRange, curveView, cursor),
  ButtonRowDelegate(header, nullptr),
  m_modelVersion(modelVersion),
  m_rangeVersion(rangeVersion),
  m_rangeParameterController(this, interactiveRange),
  m_zoomParameterController(this, interactiveRange, curveView),
  m_rangeButton(this, I18n::Message::Axis, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->rangeParameterController());
  }, this), KDText::FontSize::Small),
  m_zoomButton(this, I18n::Message::Zoom, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->zoomParameterController());
  }, this), KDText::FontSize::Small),
  m_defaultInitialisationButton(this, I18n::Message::Initialization, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->initialisationParameterController());
  }, this), KDText::FontSize::Small)
{
}

const char * InteractiveCurveViewController::title() {
  return I18n::translate(I18n::Message::GraphTab);
}

bool InteractiveCurveViewController::handleEvent(Ion::Events::Event event) {
  if (!curveView()->isMainViewSelected()) {
    if (event == Ion::Events::Down) {
      header()->setSelectedButton(-1);
      curveView()->selectMainView(true);
      app()->setFirstResponder(this);
      reloadBannerView();
      curveView()->reload();
      return true;
    }
    if (event == Ion::Events::Up) {
      header()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    return false;
  }
  if (SimpleInteractiveCurveViewController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::Down || event == Ion::Events::Up) {
    int direction = event == Ion::Events::Down ? -1 : 1;
    if (moveCursorVertically(direction)) {
      reloadBannerView();
      curveView()->reload();
      return true;
    }
    if (event == Ion::Events::Down) {
      return false;
    }
    curveView()->selectMainView(false);
    header()->setSelectedButton(0);
    return true;
  }
  return false;
}

void InteractiveCurveViewController::didBecomeFirstResponder() {
  if (!curveView()->isMainViewSelected()) {
    header()->setSelectedButton(0);
  }
}

ViewController * InteractiveCurveViewController::rangeParameterController() {
  return &m_rangeParameterController;
}

ViewController * InteractiveCurveViewController::zoomParameterController() {
  return &m_zoomParameterController;
}

int InteractiveCurveViewController::numberOfButtons(ButtonRowController::Position) const {
  if (isEmpty()) {
    return 0;
  }
  return 3;
}

Button * InteractiveCurveViewController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  const Button * buttons[3] = {&m_rangeButton, &m_zoomButton, &m_defaultInitialisationButton};
  return (Button *)buttons[index];
}

Responder * InteractiveCurveViewController::defaultController() {
  return tabController();
}

void InteractiveCurveViewController::viewWillAppear() {
  uint32_t newModelVersion = modelVersion();
  if (*m_modelVersion != newModelVersion) {
    *m_modelVersion = newModelVersion;
    initRangeParameters();
    /* Warning: init cursor parameter before reloading banner view. Indeed,
     * reloading banner view needs an updated cursor to load the right data. */
    initCursorParameters();
  }
  uint32_t newRangeVersion = rangeVersion();
  if (*m_rangeVersion != newRangeVersion) {
    *m_rangeVersion = newRangeVersion;
    if (!isCursorVisible()) {
      initCursorParameters();
    }
  }
  curveView()->setOkView(&m_okView);
  if (!curveView()->isMainViewSelected()) {
    curveView()->selectMainView(true);
    header()->setSelectedButton(-1);
  }
  reloadBannerView();
  curveView()->reload();
}

void InteractiveCurveViewController::viewDidDisappear() {
  *m_rangeVersion = rangeVersion();
}

void InteractiveCurveViewController::didEnterResponderChain(Responder * previousFirstResponder) {
  reloadBannerView();
  curveView()->reload();
}

void InteractiveCurveViewController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == nullptr || nextFirstResponder == tabController()) {
    curveView()->selectMainView(false);
    header()->setSelectedButton(-1);
    curveView()->reload();
  }
}

Responder * InteractiveCurveViewController::tabController() const{
  return (stackController()->parentResponder());
}

StackViewController * InteractiveCurveViewController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

}
