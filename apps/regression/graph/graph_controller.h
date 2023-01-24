#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include "banner_view.h"
#include "../store.h"
#include "graph_options_controller.h"
#include "graph_view.h"
#include <apps/shared/interactive_curve_view_controller.h>
#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/round_cursor_view.h>

namespace Regression {

class GraphController : public Shared::InteractiveCurveViewController {

public:
  GraphController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header, Shared::InteractiveCurveViewRange * interactiveRange, Shared::CurveViewCursor * cursor, int * selectedDotIndex, int * selectedSeriesIndex, Store * store);

  // Responder
  void didBecomeFirstResponder() override;

  // ViewController
  void viewWillAppear() override;

  // AlternateEmptyViewDelegate
  bool isEmpty() const override { return !m_store->hasValidSeries(); }
  I18n::Message emptyMessage() override;

  // SimpleInteractiveCurveViewController
  bool moveCursorHorizontally(int direction, int scrollSpeed = 1) override;

  // InteractiveCurveViewController
  bool moveCursorVertically(int direction) override;

  // InteractiveCurveViewRangeDelegate
  Poincare::Range2D optimalRange(bool computeX, bool computeY, Poincare::Range2D originalRange) const override;
  void tidyModels() override {}

  void selectRegressionCurve() { *m_selectedDotIndex = -1; }
  int selectedSeriesIndex() const { return *m_selectedSeriesIndex; }
  Poincare::Context * globalContext() const;

private:
  constexpr static size_t k_bannerViewTextBufferSize = Shared::BannerView::k_maxLengthDisplayed + sizeof("ŷ");

  class SeriesSelectionController : public Shared::CurveSelectionController {
  public:
    SeriesSelectionController(GraphController * graphController) : Shared::CurveSelectionController(graphController) {}
    const char * title() override { return I18n::translate(I18n::Message::Regression); }
    int numberOfRows() const override { return graphController()->m_store->numberOfActiveSeries(); }
    CurveSelectionCellWithChevron * reusableCell(int index, int type) override { assert(index >= 0 && index < Store::k_numberOfSeries); return m_cells + index; }
    int reusableCellCount(int type) override { return Store::k_numberOfSeries; }
    void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  private:
    KDCoordinate nonMemoizedRowHeight(int j) override;
    GraphController * graphController() const { return static_cast<GraphController *>(const_cast<InteractiveCurveViewController *>(m_graphController)); }
    CurveSelectionCellWithChevron m_cells[Store::k_numberOfSeries];
  };

  // ZoomCurveViewController
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return const_cast<const GraphController *>(this)->interactiveCurveViewRange(); }
  Shared::AbstractPlotView * curveView() override { return &m_view; }

  // SimpleInteractiveCurveViewController
  float cursorBottomMarginRatio() const override { return cursorBottomMarginRatioForBannerHeight(m_bannerView.minimalSizeForOptimalDisplay().height()); }
  void reloadBannerView() override;

  // InteractiveCurveViewController
  bool openMenuForCurveAtIndex(int curveIndex) override;
  void initCursorParameters(bool ignoreMargins = false) override;
  bool selectedModelIsValid() const override;
  Poincare::Coordinate2D<double> selectedModelXyValues(double t) const override;
  SeriesSelectionController * curveSelectionController() const override { return const_cast<SeriesSelectionController *>(&m_seriesSelectionController); }
  bool closestCurveIndexIsSuitable(int newIndex, int currentIndex, int newSubIndex, int currentSubIndex) const override;
  int selectedCurveIndex(bool relativeIndex = true) const override;
  Poincare::Coordinate2D<double> xyValues(int curveIndex, double t, Poincare::Context * context, int subCurveIndex = 0) const override;
  bool suitableYValue(double y) const override;
  int numberOfCurves() const override;
  int numberOfSubCurves(int curveIndex) const override { return 1; }
  bool isAlongY(int curveIndex) const override { return false; }

  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() const;
  void setAbscissaInputAsFirstResponder();
  bool buildRegressionExpression(char * buffer, size_t bufferSize, Model::Type modelType, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const;
  bool selectedSeriesIsScatterPlot() const { return m_store->seriesRegressionType(*m_selectedSeriesIndex) == Model::Type::None; }
  void setRoundCrossCursorView();

  Shared::ToggleableRingRoundCursorView m_cursorView;
  BannerView m_bannerView;
  GraphView m_view;
  Store * m_store;
  GraphOptionsController m_graphOptionsController;
  SeriesSelectionController m_seriesSelectionController;
  /* The selectedDotIndex is -1 when no dot is selected, m_numberOfPairs when
   * the mean dot is selected and the dot index otherwise */
  int * m_selectedDotIndex;
  int * m_selectedSeriesIndex;
  Model::Type m_selectedModelType;
};

}


#endif
