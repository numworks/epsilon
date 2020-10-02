#ifndef SHARED_FUNCTION_GRAPH_CONTROLLER_H
#define SHARED_FUNCTION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "function_banner_delegate.h"
#include "interactive_curve_view_controller.h"
#include "function_store.h"
#include "function_graph_view.h"
#include "function_curve_parameter_controller.h"

namespace Shared {

class FunctionGraphController : public InteractiveCurveViewController, public FunctionBannerDelegate {
public:
  FunctionGraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header,  InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * rangeVersion, Poincare::Preferences::AngleUnit * angleUnitVersion);
  bool isEmpty() const override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  void interestingRanges(Shared::InteractiveCurveViewRange * range) const override;

protected:
  float cursorTopMarginRatio() override { return 0.068f; }
  void reloadBannerView() override;
  bool handleEnter() override;
  int indexFunctionSelectedByCursor() const { return *m_indexFunctionSelectedByCursor; }
  virtual void selectFunctionWithCursor(int functionIndex);
  virtual double defaultCursorT(Ion::Storage::Record record);
  virtual FunctionStore * functionStore() const;

  // Closest vertical curve helper
  virtual int nextCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context) const {
    return closestCurveIndexVertically(goingUp, currentSelectedCurve, context);
  }
  bool closestCurveIndexIsSuitable(int newIndex, int currentIndex) const override;
  int selectedCurveIndex() const override { return *m_indexFunctionSelectedByCursor; }
  Poincare::Coordinate2D<double> xyValues(int curveIndex, double t, Poincare::Context * context) const override;
  int numberOfCurves() const override;
  void initCursorParameters() override;
  CurveView * curveView() override;

  Range computeYRange(Shared::InteractiveCurveViewRange * interactiveCurveViewRange) override;
  void privateComputeRanges(bool tuneXRange, Shared::InteractiveCurveViewRange * range) const;
  void yRangeForCursorFirstMove(Shared::InteractiveCurveViewRange * range) const;

private:
  virtual FunctionGraphView * functionGraphView() = 0;
  virtual FunctionCurveParameterController * curveParameterController() = 0;

  // InteractiveCurveViewController
  bool moveCursorVertically(int direction) override;
  uint32_t rangeVersion() override;

  Poincare::Preferences::AngleUnit * m_angleUnitVersion;
  int * m_indexFunctionSelectedByCursor;
};

}

#endif
