#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "banner_view.h"
#include "store.h"
#include "graph_view.h"
#include "initialisation_parameter_controller.h"
#include "prediction_parameter_controller.h"
#include "../shared/interactive_curve_view_controller.h"

namespace Regression {

class GraphController : public Shared::InteractiveCurveViewController {

public:
  GraphController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store);
  ViewController * initialisationParameterController() override;
  bool isEmpty() const override;
  const char * emptyMessage() override;
private:
  constexpr static float k_cursorTopMarginRatio = 0.025f;    // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorRightMarginRatio = 0.015f;  // (cursorWidth/2)/graphViewWidth
  constexpr static float k_cursorBottomMarginRatio = 0.19f;  // (cursorHeight/2+bannerHeigh)/graphViewHeight
  constexpr static float k_cursorLeftMarginRatio = 0.015f;   // (cursorWidth/2)/graphViewWidth

  constexpr static int k_maxNumberOfCharacters = 8;
  BannerView * bannerView() override;
  Shared::CurveView * curveView() override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override;
  bool handleEnter() override;
  void reloadBannerView() override;
  void initRangeParameters() override;
  void initCursorParameters() override;
  bool moveCursorHorizontally(int direction) override;
  bool moveCursorVertically(int direction) override;
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