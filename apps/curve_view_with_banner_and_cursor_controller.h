#ifndef APPS_CURVE_VIEW_WINDOW_WITH_BANNER_AND_CURSOR_CONTROLLER_H
#define APPS_CURVE_VIEW_WINDOW_WITH_BANNER_AND_CURSOR_CONTROLLER_H

#include <escher.h>
#include "curve_view_window_with_cursor.h"
#include "curve_view_with_banner_and_cursor.h"
#include "window_parameter_controller.h"
#include "zoom_parameter_controller.h"

class CurveViewWindowWithBannerAndCursorController : public ViewController, public HeaderViewDelegate, public AlternateEmptyViewDelegate {
public:
  CurveViewWindowWithBannerAndCursorController(Responder * parentResponder, HeaderViewController * header, CurveViewWindowWithCursor * graphWindow, CurveViewWithBannerAndCursor * graphView);
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  ViewController * windowParameterController();
  ViewController * zoomParameterController();
  virtual ViewController * initialisationParameterController() = 0;

  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;

  Responder * defaultController() override;
protected:
  virtual bool handleEnter() = 0;
  Responder * tabController() const;
  StackViewController * stackController() const;
  virtual void reloadBannerView() = 0;
  CurveViewWindowWithCursor * m_graphWindow;
  CurveViewWithBannerAndCursor * m_graphView;
  WindowParameterController m_windowParameterController;
  ZoomParameterController m_zoomParameterController;
  Button m_windowButton;
  Button m_zoomButton;
  Button m_defaultInitialisationButton;
  CursorView m_cursorView;
};

#endif
