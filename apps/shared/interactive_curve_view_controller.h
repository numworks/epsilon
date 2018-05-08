#ifndef SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include "simple_interactive_curve_view_controller.h"
#include "cursor_view.h"
#include "ok_view.h"
#include "range_parameter_controller.h"
#include "zoom_parameter_controller.h"

namespace Shared {

class InteractiveCurveViewController : public SimpleInteractiveCurveViewController, public ButtonRowDelegate, public AlternateEmptyViewDelegate {
public:
  InteractiveCurveViewController(Responder * parentResponder, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion);
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
  Responder * tabController() const;
  virtual StackViewController * stackController() const;
  virtual void initRangeParameters() = 0;
  virtual void initCursorParameters() = 0;
  virtual bool moveCursorVertically(int direction) = 0;
  virtual uint32_t modelVersion() = 0;
  virtual uint32_t rangeVersion() = 0;
  virtual bool isCursorVisible() = 0;
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
