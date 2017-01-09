#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "banner_view.h"
#include "store.h"
#include "graph_view.h"
#include "initialisation_parameter_controller.h"
#include "prediction_parameter_controller.h"
#include "../interactive_curve_view_controller.h"

namespace Regression {

class GraphController : public InteractiveCurveViewController {

public:
  GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store);
  ViewController * initialisationParameterController() override;
  bool isEmpty() override;
  const char * emptyMessage() override;
private:
  constexpr static int k_maxNumberOfCharacters = 8;
  BannerView * bannerView() override;
  CurveView * curveView() override;
  InteractiveCurveViewRange * interactiveCurveViewRange() override;
  bool handleEnter() override;
  void reloadBannerView() override;
  void initRangeParameters() override;
  void initCursorParameters() override;
  int moveCursorHorizontally(int direction) override;
  int moveCursorVertically(int direction) override;
  uint32_t modelVersion() override;
  uint32_t rangeVersion() override;
  BannerView m_bannerView;
  GraphView m_view;
  Store * m_store;
  InitialisationParameterController m_initialisationParameterController;
  PredictionParameterController m_predictionParameterController;
  /* The selectedDotIndex is -1 when no dot is selected, m_numberOfPairs when
   * the mean dot is selected and the dot index otherwise */
  int m_selectedDotIndex;
};

}


#endif