#ifndef SHARED_FUNCTION_GRAPH_CONTROLLER_H
#define SHARED_FUNCTION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "initialisation_parameter_controller.h"
#include "function_banner_delegate.h"
#include "interactive_curve_view_controller.h"
#include "function_store.h"
#include "function_graph_view.h"
#include "function_curve_parameter_controller.h"

namespace Shared {

class FunctionGraphController : public InteractiveCurveViewController, public FunctionBannerDelegate {
public:
  FunctionGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header,  InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Poincare::Preferences::AngleUnit * angleUnitVersion);
  bool isEmpty() const override;
  ViewController * initialisationParameterController() override;
  void viewWillAppear() override;

protected:
  float cursorTopMarginRatio() override { return 0.068f; }
  float cursorBottomMarginRatio() override;
  void reloadBannerView() override;
  bool handleEnter() override;
  int indexFunctionSelectedByCursor() const { return *m_indexFunctionSelectedByCursor; }
  virtual void selectFunctionWithCursor(int functionIndex);
  virtual double defaultCursorAbscissa();
  virtual FunctionStore * functionStore() const;

private:
  constexpr static float k_viewHeight = 174.0f; // TODO Taken from Regresssion/graph_controller. Maybe we should compute and/or put in common ?

  virtual FunctionGraphView * functionGraphView() = 0;
  virtual FunctionCurveParameterController * curveParameterController() = 0;

  // InteractiveCurveViewController
  /* When y auto is ticked, we use a display margin to be ensure that the user
   * can move the cursor along the curve without panning the window */
  float displayTopMarginRatio() override { return 0.09f; } // cursorHeight/graphViewHeight
  float displayBottomMarginRatio() override;

  // InteractiveCurveViewRangeDelegate
  InteractiveCurveViewRangeDelegate::Range computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) override;
  float estimatedBannerHeight() const;
  virtual int estimatedBannerNumberOfLines() const { return 1; }

  // InteractiveCurveViewController
  void initRangeParameters() override;
  void initCursorParameters() override;
  bool moveCursorVertically(int direction) override;
  CurveView * curveView() override;
  uint32_t modelVersion() override;
  uint32_t rangeVersion() override;
  bool isCursorVisible() override;
  bool closestCurveIndexIsSuitable(int newIndex, int currentIndex) const override;
  double yValue(int curveIndex, double x, Poincare::Context * context) const override;
  int numberOfCurves() const override;

  InitialisationParameterController m_initialisationParameterController;
  Poincare::Preferences::AngleUnit * m_angleUnitVersion;
  int * m_indexFunctionSelectedByCursor;
};

}

#endif
