#ifndef SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include "simple_interactive_curve_view_controller.h"
#include "cursor_view.h"
#include "ok_view.h"
#include "range_parameter_controller.h"
#include "zoom_parameter_controller.h"

namespace Shared {

class InteractiveCurveViewController : public SimpleInteractiveCurveViewController, public InteractiveCurveViewRangeDelegate, public ButtonRowDelegate, public AlternateEmptyViewDefaultDelegate {
public:
  InteractiveCurveViewController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion);

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
  virtual void initCursorParameters() = 0;
  virtual bool moveCursorVertically(int direction) = 0;
  virtual uint32_t modelVersion() = 0;
  virtual uint32_t rangeVersion() = 0;
  virtual bool isCursorVisible() = 0;

  // Closest vertical curve helper
  int closestCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context) const;
  virtual bool closestCurveIndexIsSuitable(int newIndex, int currentIndex) const { assert(false); return false; }
  virtual double yValue(int curveIndex, double x, Poincare::Context * context) const { assert(false); return 0; }
  virtual bool suitableYValue(double y) const { return true; }
  virtual int numberOfCurves() const = 0;

  OkView m_okView;
private:
  // InteractiveCurveViewRangeDelegate
  float addMargin(float x, float range, bool isMin) override;
  virtual float displayTopMarginRatio() = 0;
  virtual float displayBottomMarginRatio() = 0;

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
