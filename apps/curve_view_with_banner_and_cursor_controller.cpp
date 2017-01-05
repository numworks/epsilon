#include "curve_view_with_banner_and_cursor_controller.h"
#include <assert.h>
#include <math.h>

CurveViewWindowWithBannerAndCursorController::CurveViewWindowWithBannerAndCursorController(Responder * parentResponder,
    HeaderViewController * header, CurveViewWindowWithCursor * graphWindow, CurveViewWithBannerAndCursor * graphView) :
  ViewController(parentResponder),
  HeaderViewDelegate(header),
  m_graphWindow(graphWindow),
  m_graphView(graphView),
  m_windowParameterController(WindowParameterController(this, m_graphWindow)),
  m_zoomParameterController(ZoomParameterController(this, m_graphWindow, m_graphView)),
  m_windowButton(this, "Axes", Invocation([](void * context, void * sender) {
    CurveViewWindowWithBannerAndCursorController * graphController = (CurveViewWindowWithBannerAndCursorController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->windowParameterController());
  }, this)),
  m_zoomButton(this, "Zoom", Invocation([](void * context, void * sender) {
    CurveViewWindowWithBannerAndCursorController * graphController = (CurveViewWindowWithBannerAndCursorController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->zoomParameterController());
  }, this)),
  m_defaultInitialisationButton(this, "Initialisation", Invocation([](void * context, void * sender) {
    CurveViewWindowWithBannerAndCursorController * graphController = (CurveViewWindowWithBannerAndCursorController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->initialisationParameterController());
  }, this)),
  m_cursorView(CursorView())
{
}

const char * CurveViewWindowWithBannerAndCursorController::title() const {
  return "Graphique";
}

View * CurveViewWindowWithBannerAndCursorController::view() {
  return m_graphView;
}

bool CurveViewWindowWithBannerAndCursorController::handleEvent(Ion::Events::Event event) {
  if (!m_graphView->isMainViewSelected()) {
    if (event == Ion::Events::Down) {
      headerViewController()->setSelectedButton(-1);
      m_graphView->selectMainView(true);
      m_graphView->reloadSelection();
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
    m_graphWindow->zoom(1.0f/3.0f);
    m_graphView->reload();
    reloadBannerView();
    return true;
  }
  if (event == Ion::Events::Minus) {
    m_graphWindow->zoom(3.0f/4.0f);
    m_graphView->reload();
    reloadBannerView();
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    int direction = event == Ion::Events::Left ? -1 : 1;
    m_graphView->reloadSelection();
    int didMoveCursor = m_graphWindow->moveCursorHorizontally(direction);
    if (didMoveCursor == 0) {
      m_graphView->reloadSelection();
    } else {
      m_graphView->reload();
    }
    reloadBannerView();
    return (didMoveCursor >= 0);
  }
  if (event == Ion::Events::Down || event == Ion::Events::Up) {
    int direction = event == Ion::Events::Down ? -1 : 1;
    m_graphView->reloadSelection();
    int didMoveCursor = m_graphWindow->moveCursorVertically(direction);
    if (didMoveCursor < 0) {
      if (event == Ion::Events::Down) {
        return false;
      }
      m_graphView->selectMainView(false);
      headerViewController()->setSelectedButton(0);
      return true;
    }
    if (didMoveCursor == 0) {
      m_graphView->reloadSelection();
    }
    if (didMoveCursor == 1) {
      m_graphView->reload();
    }
    reloadBannerView();
    return true;
  }
  if (event == Ion::Events::OK) {
    return handleEnter();
  }
  return false;
}

void CurveViewWindowWithBannerAndCursorController::didBecomeFirstResponder() {
  headerViewController()->setSelectedButton(-1);
  m_graphView->selectMainView(true);
  // Layout view whe the graph view that might have been modified by the zoom page
  headerViewController()->layoutView();
  // Reload graph view
  m_graphView->reload();
  reloadBannerView();
}

ViewController * CurveViewWindowWithBannerAndCursorController::windowParameterController() {
  return &m_windowParameterController;
}

ViewController * CurveViewWindowWithBannerAndCursorController::zoomParameterController() {
  return &m_zoomParameterController;
}

int CurveViewWindowWithBannerAndCursorController::numberOfButtons() const {
  return 3;
}

Button * CurveViewWindowWithBannerAndCursorController::buttonAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_windowButton;
    case 1:
      return &m_zoomButton;
    case 2:
      return &m_defaultInitialisationButton;
    default:
      assert(false);
  }
  return nullptr;
}

Responder * CurveViewWindowWithBannerAndCursorController::defaultController() {
  return tabController();
}

Responder * CurveViewWindowWithBannerAndCursorController::tabController() const{
  return (parentResponder()->parentResponder()->parentResponder()->parentResponder());
}

StackViewController * CurveViewWindowWithBannerAndCursorController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}
