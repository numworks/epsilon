#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include <escher.h>
#include "graph_view.h"
#include "banner_view.h"
#include "curve_parameter_controller.h"
#include "initialisation_parameter_controller.h"
#include "../../interactive_curve_view_controller.h"
#include "../function_store.h"

namespace Graph {
class GraphController : public InteractiveCurveViewController, public InteractiveCurveViewRangeDelegate {
public:
  GraphController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header);
  View * view() override;
  void didBecomeFirstResponder() override;
  ViewController * initialisationParameterController() override;

  bool isEmpty() override;
  const char * emptyMessage() override;

  bool didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange) override;

private:
  constexpr static int k_maxNumberOfCharacters = 8;
  BannerView * bannerView() override;
  bool handleEnter() override;
  void reloadBannerView() override;
  void initRangeParameters() override;
  void initCursorParameters() override;
  int moveCursorHorizontally(int direction) override;
  int moveCursorVertically(int direction) override;
  uint32_t modelVersion() override;
  uint32_t rangeVersion() override;
  InteractiveCurveViewRange * interactiveCurveViewRange() override;
  CurveView * curveView() override;
  BannerView m_bannerView;
  GraphView m_view;
  InteractiveCurveViewRange m_graphRange;
  InitialisationParameterController m_initialisationParameterController;
  CurveParameterController m_curveParameterController;
  FunctionStore * m_functionStore;
  int m_indexFunctionSelectedByCursor;
};

}

#endif
