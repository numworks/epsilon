#ifndef SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include "simple_interactive_curve_view_controller.h"
#include "cursor_view.h"
#include "ok_view.h"
#include "range_parameter_controller.h"
#include "zoom_parameter_controller.h"
#include <poincare/coordinate_2D.h>

namespace Shared {

class InteractiveCurveViewController : public SimpleInteractiveCurveViewController, public InteractiveCurveViewRangeDelegate, public ButtonRowDelegate, public AlternateEmptyViewDefaultDelegate {
public:
  InteractiveCurveViewController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * previousModelsVersions, uint32_t * rangeVersion);

  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("Graph");

  ViewController * rangeParameterController();
  ViewController * zoomParameterController();
  virtual ViewController * initialisationParameterController() = 0;

  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;

  Responder * defaultController() override;

  bool previousModelsWereAllDeleted();

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
  virtual uint32_t modelVersion() = 0;
  virtual uint32_t modelVersionAtIndex(int i) = 0;
  virtual uint32_t rangeVersion() = 0;
  bool isCursorVisible();

  // Closest vertical curve helper
  int closestCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context) const;
  virtual bool closestCurveIndexIsSuitable(int newIndex, int currentIndex) const = 0;
  virtual int selectedCurveIndex() const = 0;
  virtual Poincare::Coordinate2D<double> xyValues(int curveIndex, double t, Poincare::Context * context) const = 0;
  virtual bool suitableYValue(double y) const { return true; }
  virtual int numberOfCurves() const = 0;

  // SimpleInteractiveCurveViewController
  float cursorBottomMarginRatio() override;

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

  virtual bool shouldSetDefaultOnModelChange() const { return false; }
  virtual size_t numberOfMemoizedVersions() const = 0;
  uint32_t * m_modelVersion;
  uint32_t * m_previousModelsVersions;
  uint32_t * m_rangeVersion;
  RangeParameterController m_rangeParameterController;
  ZoomParameterController m_zoomParameterController;
  Button m_rangeButton;
  Button m_zoomButton;
  Button m_defaultInitialisationButton;
};

}

#endif
