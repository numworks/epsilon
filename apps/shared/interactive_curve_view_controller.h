#ifndef SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include <escher.h>
#include "interactive_curve_view_range.h"
#include "curve_view_cursor.h"
#include "curve_view.h"
#include "cursor_view.h"
#include "ok_view.h"
#include "banner_view.h"
#include "range_parameter_controller.h"
#include "zoom_parameter_controller.h"

namespace Shared {

class InteractiveCurveViewController : public ViewController, public ButtonRowDelegate, public AlternateEmptyViewDelegate {
public:
  InteractiveCurveViewController(Responder * parentResponder, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion);
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  ViewController * rangeParameterController();
  ViewController * zoomParameterController();
  virtual ViewController * initialisationParameterController() = 0;

  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;

  Responder * defaultController() override;

  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
protected:
  constexpr static float k_numberOfCursorStepsInGradUnit = 5.0f;
  virtual BannerView * bannerView() = 0;
  virtual bool handleEnter() = 0;
  Responder * tabController() const;
  virtual StackViewController * stackController() const;
  virtual void reloadBannerView() = 0;
  virtual void initRangeParameters() = 0;
  virtual void initCursorParameters() = 0;
  /* the result of moveCursorVertically/Horizontally means:
   * false -> the cursor cannot move in this direction
   * true -> the cursor moved */
  virtual bool moveCursorHorizontally(int direction) = 0;
  virtual bool moveCursorVertically(int direction) = 0;
  virtual uint32_t modelVersion() = 0;
  virtual uint32_t rangeVersion() = 0;
  virtual InteractiveCurveViewRange * interactiveCurveViewRange() = 0;
  virtual CurveView * curveView() = 0;
  virtual bool isCursorVisible() = 0;
  CurveViewCursor * m_cursor;
  CursorView m_cursorView;
  OkView m_okView;
private:
  uint32_t * m_modelVersion;
  uint32_t * m_rangeVersion;
  RangeParameterController m_rangeParameterController;
  ZoomParameterController m_zoomParameterController;
  Button m_rangeButton;
  Button m_zoomButton;
  Button m_defaultInitialisationButton;
};

}

#endif
