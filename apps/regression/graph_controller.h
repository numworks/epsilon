#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include "banner_view.h"
#include "store.h"
#include "graph_options_controller.h"
#include "graph_view.h"
#include "initialisation_parameter_controller.h"
#include "../shared/interactive_curve_view_controller.h"
#include "../shared/curve_view_cursor.h"
#include "../shared/cursor_view.h"
#include "../shared/round_cursor_view.h"

namespace Regression {

class GraphController : public Shared::InteractiveCurveViewController {

public:
  GraphController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, Store * store, Shared::CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * previousModelsVersions, uint32_t * rangeVersion, int * selectedDotIndex, int * selectedSeriesIndex);
  ViewController * initialisationParameterController() override;
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  void viewWillAppear() override;
  void selectRegressionCurve() { *m_selectedDotIndex = -1; }
  int selectedSeriesIndex() const { return *m_selectedSeriesIndex; }

  // moveCursorHorizontally and Vertically are public to be used in tests
  bool moveCursorHorizontally(int direction, bool fast = false) override;
  bool moveCursorVertically(int direction) override;

private:
  constexpr static int k_maxLegendLength = 16;
  constexpr static int k_maxNumberOfCharacters = 50;

  Poincare::Context * globalContext();

  // SimpleInteractiveCurveViewController
  void reloadBannerView() override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override;
  Shared::CurveView * curveView() override;
  bool handleEnter() override;

  // InteractiveCurveViewController
  void initCursorParameters() override;
  uint32_t modelVersion() override;
  uint32_t modelVersionAtIndex(int i) override;
  uint32_t rangeVersion() override;
  size_t numberOfMemoizedVersions() const override { return Store::k_numberOfSeries; }
  int selectedCurveIndex() const override { return *m_selectedSeriesIndex; }
  bool closestCurveIndexIsSuitable(int newIndex, int currentIndex) const override;
  Poincare::Coordinate2D<double> xyValues(int curveIndex, double x, Poincare::Context * context) const override;
  double yValue(int curveIndex, double x, Poincare::Context * context) const;
  bool suitableYValue(double y) const override;
  int numberOfCurves() const override;
  int estimatedBannerNumberOfLines() const override;

  // InteractiveCurveViewRangeDelegate
  Shared::InteractiveCurveViewRangeDelegate::Range computeYRange(Shared::InteractiveCurveViewRange * interactiveCurveViewRange) override;

  void setRoundCrossCursorView();
  Shared::CursorView m_crossCursorView;
  Shared::RoundCursorView m_roundCursorView;
  BannerView m_bannerView;
  GraphView m_view;
  Store * m_store;
  InitialisationParameterController m_initialisationParameterController;
  GraphOptionsController m_graphOptionsController;
  /* The selectedDotIndex is -1 when no dot is selected, m_numberOfPairs when
   * the mean dot is selected and the dot index otherwise */
  int * m_selectedDotIndex;
  int * m_selectedSeriesIndex;
  Model::Type m_modelType[Store::k_numberOfSeries];
};

}


#endif
