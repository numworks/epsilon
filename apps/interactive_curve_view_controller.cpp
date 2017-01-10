#include "interactive_curve_view_controller.h"
#include <assert.h>
#include <math.h>

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
  }, this)),
  m_zoomButton(this, "Zoom", Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->zoomParameterController());
  }, this)),
  m_defaultInitialisationButton(this, "Initialisation", Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->initialisationParameterController());
  }, this))
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
      curveView()->reloadSelection();
      reloadBannerView();
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
    curveView()->reload();
    reloadBannerView();
    return true;
  }
  if (event == Ion::Events::Minus) {
    interactiveCurveViewRange()->zoom(3.0f/4.0f);
    curveView()->reload();
    reloadBannerView();
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    curveView()->reloadSelection();
    int didMoveCursor = moveCursorHorizontally(direction);
    if (didMoveCursor == 0) {
      curveView()->reloadSelection();
    }
    if (didMoveCursor == 1) {
      curveView()->reload();
    }
    reloadBannerView();
    return (didMoveCursor >= 0);
  }
  if (event == Ion::Events::Down || event == Ion::Events::Up) {
    int direction = event == Ion::Events::Down ? -1 : 1;
    curveView()->reloadSelection();
    int didMoveCursor = moveCursorVertically(direction);
    if (didMoveCursor < 0) {
      if (event == Ion::Events::Down) {
        return false;
      }
      curveView()->selectMainView(false);
      headerViewController()->setSelectedButton(0);
      return true;
    }
    if (didMoveCursor == 0) {
      curveView()->reloadSelection();
    }
    if (didMoveCursor == 1) {
      curveView()->reload();
    }
    reloadBannerView();
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
  }
  uint32_t newRangeVersion = rangeVersion();
  if (m_rangeVersion != newRangeVersion) {
    m_rangeVersion = newRangeVersion;
    initCursorParameters();
  }
  headerViewController()->setSelectedButton(-1);
  curveView()->selectMainView(true);
  // Layout view whe the graph view that might have been modified by the zoom page
  headerViewController()->layoutView();
  // Reload graph view
  curveView()->reload();
  reloadBannerView();
}

ViewController * InteractiveCurveViewController::rangeParameterController() {
  return &m_rangeParameterController;
}

ViewController * InteractiveCurveViewController::zoomParameterController() {
  return &m_zoomParameterController;
}

int InteractiveCurveViewController::numberOfButtons() const {
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
