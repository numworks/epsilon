#include "interactive_curve_view_controller.h"
#include "text_field_delegate_app.h"
#include <math.h>
#include <assert.h>

using namespace Poincare;

namespace Shared {

InteractiveCurveViewController::InteractiveCurveViewController(Responder * parentResponder, HeaderViewController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_cursor(),
  m_cursorView(CursorView()),
  m_rangeParameterController(RangeParameterController(this, interactiveRange)),
  m_zoomParameterController(ZoomParameterController(this, interactiveRange, curveView)),
  m_rangeButton(this, "Axes", Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->rangeParameterController());
  }, this), KDText::FontSize::Small),
  m_zoomButton(this, "Zoom", Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->zoomParameterController());
  }, this), KDText::FontSize::Small),
  m_defaultInitialisationButton(this, "Initialisation", Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->initialisationParameterController());
  }, this), KDText::FontSize::Small)
{
}

const char * InteractiveCurveViewController::title() const {
  return "Graphique";
}

View * InteractiveCurveViewController::view() {
  return curveView();
}

bool InteractiveCurveViewController::handleEvent(Ion::Events::Event event) {
  if (!curveView()->isMainViewSelected()) {
    if (event == Ion::Events::Down) {
      headerViewController()->setSelectedButton(-1);
      curveView()->selectMainView(true);
      app()->setFirstResponder(this);
      reloadBannerView();
      curveView()->reload();
      return true;
    }
    if (event == Ion::Events::Up) {
      headerViewController()->setSelectedButton(-1);
      app()->setFirstResponder(tabController());
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Plus) {
    interactiveCurveViewRange()->zoom(1.0f/3.0f);
    reloadBannerView();
    curveView()->reload();
    return true;
  }
  if (event == Ion::Events::Minus) {
    interactiveCurveViewRange()->zoom(3.0f/4.0f);
    reloadBannerView();
    curveView()->reload();
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    if (moveCursorHorizontally(direction)) {
      reloadBannerView();
      curveView()->reload();
      return true;
    }
    return false;
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
    headerViewController()->setSelectedButton(0);
    return true;
  }
  if (event == Ion::Events::OK) {
    return handleEnter();
  }
  return false;
}

void InteractiveCurveViewController::didBecomeFirstResponder() {
  uint32_t newModelVersion = modelVersion();
  if (m_modelVersion != newModelVersion) {
    m_modelVersion = newModelVersion;
    initRangeParameters();
    /* Warning: init cursor parameter before reloading banner view. Indeed,
     * reloading banner view needs an updated cursor to load the right data. */
    initCursorParameters();
    centerCursorVertically();
    reloadBannerView();
    curveView()->reload();
  }
  uint32_t newRangeVersion = rangeVersion();
  if (m_rangeVersion != newRangeVersion) {
    m_rangeVersion = newRangeVersion;
    initCursorParameters();
    reloadBannerView();
    curveView()->reload();
  }
  if (!curveView()->isMainViewSelected()) {
    headerViewController()->setSelectedButton(0);
  }
}

ViewController * InteractiveCurveViewController::rangeParameterController() {
  return &m_rangeParameterController;
}

ViewController * InteractiveCurveViewController::zoomParameterController() {
  return &m_zoomParameterController;
}

int InteractiveCurveViewController::numberOfButtons() const {
  if (isEmpty()) {
    return 0;
  }
  return 3;
}

Button * InteractiveCurveViewController::buttonAtIndex(int index) {
  Button * buttons[3] = {&m_rangeButton, &m_zoomButton, &m_defaultInitialisationButton};
  return buttons[index];
}

Responder * InteractiveCurveViewController::defaultController() {
  return tabController();
}

void InteractiveCurveViewController::viewWillAppear() {
  curveView()->selectMainView(true);
  headerViewController()->setSelectedButton(-1);
  reloadBannerView();
  curveView()->reload();
}

Responder * InteractiveCurveViewController::tabController() const{
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * InteractiveCurveViewController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

void InteractiveCurveViewController::centerCursorVertically() {
  if (!interactiveCurveViewRange()->yAuto()) {
    interactiveCurveViewRange()->centerAxisAround(CurveViewRange::Axis::Y, m_cursor.y());
  }
}
}
