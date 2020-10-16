#ifndef SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include "simple_interactive_curve_view_controller.h"
#include "cursor_view.h"
#include "ok_view.h"
#include "range_parameter_controller.h"
#include "function_zoom_and_pan_curve_view_controller.h"
#include <poincare/coordinate_2D.h>

namespace Shared {

class InteractiveCurveViewController : public SimpleInteractiveCurveViewController, public InteractiveCurveViewRangeDelegate, public ButtonRowDelegate, public AlternateEmptyViewDefaultDelegate {
public:
  InteractiveCurveViewController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, uint32_t * rangeVersion);

  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("Graph");

  RangeParameterController * rangeParameterController();
  ViewController * zoomParameterController();

  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;

  Responder * defaultController() override;

  void viewWillAppear() override;
  void viewDidDisappear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;

protected:
  Responder * tabController() const;
  virtual StackViewController * stackController() const;
  virtual void initCursorParameters() = 0;
  virtual bool moveCursorVertically(int direction) = 0;
  virtual uint32_t rangeVersion() = 0;
  bool isCursorVisible();
  virtual bool isCursorHanging() = 0;

  // Closest vertical curve helper
  int closestCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context) const;
  virtual bool closestCurveIndexIsSuitable(int newIndex, int currentIndex) const = 0;
  virtual int selectedCurveIndex() const = 0;
  virtual Poincare::Coordinate2D<double> xyValues(int curveIndex, double t, Poincare::Context * context) const = 0;
  virtual bool suitableYValue(double y) const { return true; }
  virtual int numberOfCurves() const = 0;

  // SimpleInteractiveCurveViewController
  float cursorBottomMarginRatio() override;

  InteractiveCurveViewRange * interactiveRange() { return m_interactiveRange; }

  OkView m_okView;
private:
  /* The value 21 is the actual height of the ButtonRow, that is
   * ButtonRowController::ContentView::k_plainStyleHeight + 1.
   * That value is not public though. */
  constexpr static float k_viewHeight = Ion::Display::Height - Metric::TitleBarHeight - Metric::TabHeight - 21;
  float estimatedBannerHeight() const;
  virtual int estimatedBannerNumberOfLines() const { return 1; }

  // InteractiveCurveViewRangeDelegate
  float addMargin(float x, float range, bool isVertical, bool isMin) override;

  void setCurveViewAsMainView();

  void navigationButtonAction();
  /* Those  two methods return the new status for the button, ie either
   * m_interactiveRange->m_zoomAuto or m_zoomNormalize respectively. */
  bool autoButtonAction();
  bool normalizeButtonAction();

  uint32_t * m_rangeVersion;
  RangeParameterController m_rangeParameterController;
  FunctionZoomAndPanCurveViewController m_zoomParameterController;
  InteractiveCurveViewRange * m_interactiveRange;
  Button m_autoButton;
  Button m_normalizeButton;
  Button m_navigationButton;
  Button m_rangeButton;
};

}

#endif
