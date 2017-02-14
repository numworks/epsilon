#include "interactive_curve_view_controller.h"
#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <assert.h>
#include <math.h>

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
  }, this), KDText::FontSize::Small),
  m_displayModeVersion(Expression::FloatDisplayMode::Auto)
{
}

const char * InteractiveCurveViewController::title() const {
  return "Graphique";
}

View * InteractiveCurveViewController::view() {
  AppsContainer * myContainer = ((TextFieldDelegateApp *)app())->container();
  Expression::FloatDisplayMode FloatDisplayMode = myContainer->preferences()->displayMode();
  if (FloatDisplayMode != m_displayModeVersion) {
    reloadBannerView();
    curveView()->reload();
    m_displayModeVersion = FloatDisplayMode;
  }
  return curveView();
}

bool InteractiveCurveViewController::handleEvent(Ion::Events::Event event) {
  if (!curveView()->isMainViewSelected()) {
    if (event == Ion::Events::Down) {
      headerViewController()->setSelectedButton(-1);
      curveView()->selectMainView(true);
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
  curveView()->selectMainView(true);
  uint32_t newModelVersion = modelVersion();
  if (m_modelVersion != newModelVersion) {
    m_modelVersion = newModelVersion;
    initRangeParameters();
    initCursorParameters();
  }
  uint32_t newRangeVersion = rangeVersion();
  if (m_rangeVersion != newRangeVersion) {
    m_rangeVersion = newRangeVersion;
    initCursorParameters();
  }
  headerViewController()->setSelectedButton(-1);
  // Reload graph view
  reloadBannerView();
  curveView()->reload();
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

Responder * InteractiveCurveViewController::tabController() const{
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * InteractiveCurveViewController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

}
