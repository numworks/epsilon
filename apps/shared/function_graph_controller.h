#ifndef SHARED_FUNCTION_GRAPH_CONTROLLER_H
#define SHARED_FUNCTION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "initialisation_parameter_controller.h"
#include "interactive_curve_view_controller.h"
#include "function_store.h"
#include "function_graph_view.h"
#include "function_curve_parameter_controller.h"

namespace Shared {

class FunctionGraphController : public InteractiveCurveViewController, public InteractiveCurveViewRangeDelegate {
public:
  FunctionGraphController(Responder * parentResponder, HeaderViewController * header,  InteractiveCurveViewRange * interactiveRange, CurveView * curveView);
  bool isEmpty() const override;
  ViewController * initialisationParameterController() override;
  void viewWillAppear() override;
  bool didChangeRange(Shared::InteractiveCurveViewRange * interactiveCurveViewRange) override;
protected:
  constexpr static float k_cursorTopMarginRatio = 0.07f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorRightMarginRatio = 0.04f; // (cursorWidth/2)/graphViewWidth
  constexpr static float k_cursorBottomMarginRatio = 0.15f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  constexpr static float k_cursorLeftMarginRatio = 0.04f;  // (cursorWidth/2)/graphViewWidth
  constexpr static int k_maxNumberOfCharacters = 8;
  void reloadBannerView() override;
  bool handleEnter() override;
  int m_indexFunctionSelectedByCursor;
private:
  /* When y auto is ticked, we use a display margin to be ensure that the user
   * can move the cursor along the curve without panning the window */
  constexpr static float k_displayTopMarginRatio = 0.09f;
  constexpr static float k_displayBottomMarginRatio = 0.2f;

  void initRangeParameters() override;
  bool moveCursorVertically(int direction) override;
  CurveView * curveView() override;
  uint32_t modelVersion() override;
  uint32_t rangeVersion() override;
  virtual FunctionStore * functionStore() const = 0;
  virtual FunctionGraphView * functionGraphView() = 0;
  virtual FunctionCurveParameterController * curveParameterController() = 0;
  InitialisationParameterController m_initialisationParameterController;
  Poincare::Expression::AngleUnit m_angleUnitVersion;
};

}

#endif
