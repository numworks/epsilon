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

class FunctionGraphController : public InteractiveCurveViewController, public InteractiveCurveViewRangeDelegate, public FunctionBannerDelegate {
public:
  FunctionGraphController(Responder * parentResponder, ButtonRowController * header,  InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Poincare::Expression::AngleUnit * angleUnitVersion);
  bool isEmpty() const override;
  ViewController * initialisationParameterController() override;
  void viewWillAppear() override;
protected:
  constexpr static float k_cursorTopMarginRatio = 0.068f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorBottomMarginRatio = 0.15f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  void reloadBannerView() override;
  bool handleEnter() override;
  int indexFunctionSelectedByCursor() const {
    return *m_indexFunctionSelectedByCursor;
  }
  virtual void selectFunctionWithCursor(int functionIndex);
private:
  /* When y auto is ticked, we use a display margin to be ensure that the user
   * can move the cursor along the curve without panning the window */
  constexpr static float k_displayTopMarginRatio = 0.09f;
  constexpr static float k_displayBottomMarginRatio = 0.2f;

  InteractiveCurveViewRangeDelegate::Range computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) override;
  float addMargin(float x, float range, bool isMin) override;

  void initRangeParameters() override;
  bool moveCursorVertically(int direction) override;
  CurveView * curveView() override;
  uint32_t modelVersion() override;
  uint32_t rangeVersion() override;
  bool isCursorVisible() override;
  virtual FunctionGraphView * functionGraphView() = 0;
  virtual View * cursorView() = 0;
  virtual FunctionStore * functionStore() const = 0;
  virtual FunctionCurveParameterController * curveParameterController() = 0;
  InitialisationParameterController m_initialisationParameterController;
  Poincare::Expression::AngleUnit * m_angleUnitVersion;
  int * m_indexFunctionSelectedByCursor;
};

}

#endif
