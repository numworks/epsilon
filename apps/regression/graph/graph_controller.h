#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/interactive_curve_view_controller.h>
#include <apps/shared/round_cursor_view.h>

#include "../store.h"
#include "banner_view.h"
#include "graph_options_controller.h"
#include "graph_view.h"

namespace Regression {

/* Warning: we tried to use only curve indexes instead of series indexes for
   code clarity. However, if number of series/curves should be inscreased, it
   might lead to performance decrease because we convert often curve index into
   series index. */

class GraphController : public Shared::InteractiveCurveViewController {
 public:
  GraphController(Escher::Responder *parentResponder,
                  Escher::InputEventHandlerDelegate *inputEventHandlerDelegate,
                  Escher::ButtonRowController *header,
                  Shared::InteractiveCurveViewRange *interactiveRange,
                  Shared::CurveViewCursor *cursor, int *selectedDotIndex,
                  int *selectedCurveIndex, Store *store);

  // Responder
  void didBecomeFirstResponder() override;

  // ViewController
  void viewWillAppear() override;

  // AlternateEmptyViewDelegate
  I18n::Message emptyMessage() override;

  // SimpleInteractiveCurveViewController
  bool moveCursorHorizontally(OMG::HorizontalDirection direction,
                              int scrollSpeed = 1) override;

  // InteractiveCurveViewController
  bool moveCursorVertically(OMG::VerticalDirection direction) override;

  // InteractiveCurveViewRangeDelegate
  Poincare::Range2D optimalRange(
      bool computeX, bool computeY,
      Poincare::Range2D originalRange) const override;
  void tidyModels() override {}

  void selectRegressionCurve() { *m_selectedDotIndex = -1; }
  int selectedSeriesIndex() const {
    return seriesIndexFromCurveIndex(*m_selectedCurveIndex);
  }
  Poincare::Context *globalContext() const;

 private:
  constexpr static size_t k_bannerViewTextBufferSize =
      Shared::BannerView::k_maxLengthDisplayed + sizeof("ŷ");

  class CurveSelectionController : public Shared::CurveSelectionController {
   public:
    CurveSelectionController(GraphController *graphController)
        : Shared::CurveSelectionController(graphController) {}
    const char *title() override {
      return I18n::translate(I18n::Message::Regression);
    }
    int numberOfRows() const override {
      return graphController()->numberOfCurves();
    }
    Shared::CurveSelectionCellWithChevron *reusableCell(int index,
                                                        int type) override {
      assert(index >= 0 && index < Store::k_numberOfSeries);
      return m_cells + index;
    }
    int reusableCellCount(int type) override { return Store::k_numberOfSeries; }
    void willDisplayCellForIndex(Escher::HighlightCell *cell,
                                 int index) override;

   private:
    KDCoordinate nonMemoizedRowHeight(int j) override;
    GraphController *graphController() const {
      return static_cast<GraphController *>(
          const_cast<InteractiveCurveViewController *>(m_graphController));
    }
    Shared::CurveSelectionCellWithChevron m_cells[Store::k_numberOfSeries];
  };

  // ZoomCurveViewController
  Shared::InteractiveCurveViewRange *interactiveCurveViewRange() override {
    return const_cast<const GraphController *>(this)
        ->interactiveCurveViewRange();
  }
  Shared::AbstractPlotView *curveView() override { return &m_view; }

  // SimpleInteractiveCurveViewController
  float cursorBottomMarginRatio() const override {
    return cursorBottomMarginRatioForBannerHeight(
        m_bannerView.minimalSizeForOptimalDisplay().height());
  }
  void reloadBannerView() override;

  // InteractiveCurveViewController
  void openMenuForCurveAtIndex(int curveIndex) override;
  void initCursorParameters(bool ignoreMargins = false) override;
  bool selectedModelIsValid() const override;
  Poincare::Coordinate2D<double> selectedModelXyValues(double t) const override;
  CurveSelectionController *curveSelectionController() const override {
    return const_cast<CurveSelectionController *>(&m_curveSelectionController);
  }
  Poincare::Coordinate2D<double> xyValues(int curveIndex, double t,
                                          Poincare::Context *context,
                                          int subCurveIndex = 0) const override;
  bool suitableYValue(double y) const override;
  int numberOfCurves() const override {
    return m_store->numberOfActiveSeries();
  }
  int numberOfSubCurves(int curveIndex) const override { return 1; }
  bool isAlongY(int curveIndex) const override { return false; }

  Shared::InteractiveCurveViewRange *interactiveCurveViewRange() const;
  void setAbscissaInputAsFirstResponder();
  bool buildRegressionExpression(
      char *buffer, size_t bufferSize, Model::Type modelType,
      int significantDigits,
      Poincare::Preferences::PrintFloatMode displayMode) const;
  void setRoundCrossCursorView();
  int seriesIndexFromCurveIndex(int curveIndex) const {
    return m_store->seriesIndexFromActiveSeriesIndex(curveIndex);
  }
  int curveIndexFromSeriesIndex(int seriesIndex) const {
    return m_store->activeSeriesIndexFromSeriesIndex(seriesIndex);
  }
  Model::Type regressionTypeOfCurve(int curveIndex) const {
    return m_store->seriesRegressionType(seriesIndexFromCurveIndex(curveIndex));
  }
  bool curveIsScatterPlot(int curveIndex) const {
    return regressionTypeOfCurve(curveIndex) == Model::Type::None;
  }
  int numberOfDotsOfCurve(int curveIndex) const {
    return m_store->numberOfPairsOfSeries(
        seriesIndexFromCurveIndex(curveIndex));
  }
  double dotAbscissa(int curveIndex, int dotIndex) const {
    return dotCoordinate(curveIndex, dotIndex, 0);
  }
  double dotOrdinate(int curveIndex, int dotIndex) const {
    return dotCoordinate(curveIndex, dotIndex, 1);
  }
  double dotCoordinate(int curveIndex, int dotIndex, int coordinate) const;
  double yValue(int curveIndex, double t, Poincare::Context *context) const {
    return m_store->yValueForXValue(seriesIndexFromCurveIndex(curveIndex), t,
                                    context);
  }

  Shared::ToggleableRingRoundCursorView m_cursorView;
  BannerView m_bannerView;
  GraphView m_view;
  Store *m_store;
  GraphOptionsController m_graphOptionsController;
  CurveSelectionController m_curveSelectionController;
  /* The selectedDotIndex is -1 when no dot is selected, m_numberOfPairs when
   * the mean dot is selected and the dot index otherwise */
  int *m_selectedDotIndex;
  Model::Type m_selectedModelType;
};

}  // namespace Regression

#endif
