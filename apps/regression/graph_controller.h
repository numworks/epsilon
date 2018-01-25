#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "banner_view.h"
#include "store.h"
#include "graph_view.h"
#include "initialisation_parameter_controller.h"
#include "prediction_parameter_controller.h"
#include "../shared/interactive_curve_view_controller.h"
#include "../shared/curve_view_cursor.h"
#include "../shared/cursor_view.h"
#include "../shared/round_cursor_view.h"

namespace Regression {

class GraphController : public Shared::InteractiveCurveViewController {

public:
  GraphController(Responder * parentResponder, ButtonRowController * header, Store * store, Shared::CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion, int * selectedDotIndex);
  ViewController * initialisationParameterController() override;
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  void viewWillAppear() override;
  void selectRegressionCurve();
private:
  constexpr static float k_cursorTopMarginRatio = 0.07f;    // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorBottomMarginRatio = 0.3f;  // (cursorHeight/2+bannerHeigh)/graphViewHeight
  constexpr static int k_maxLegendLength = 16;
  constexpr static int k_maxNumberOfCharacters = 50;
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
  bool isCursorVisible() override;
  Shared::CursorView m_crossCursorView;
  Shared::RoundCursorView m_roundCursorView;
  BannerView m_bannerView;
  GraphView m_view;
  Store * m_store;
  InitialisationParameterController m_initialisationParameterController;
  PredictionParameterController m_predictionParameterController;
  /* The selectedDotIndex is -1 when no dot is selected, m_numberOfPairs when
   * the mean dot is selected and the dot index otherwise */
  int * m_selectedDotIndex;
};

}


#endif
