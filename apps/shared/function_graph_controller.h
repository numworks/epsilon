#ifndef SHARED_FUNCTION_GRAPH_CONTROLLER_H
#define SHARED_FUNCTION_GRAPH_CONTROLLER_H

#include "function_banner_delegate.h"
#include "function_graph_view.h"
#include "function_store.h"
#include "interactive_curve_view_controller.h"

namespace Shared {

class FunctionGraphController : public InteractiveCurveViewController,
                                public FunctionBannerDelegate {
 public:
  FunctionGraphController(Escher::Responder* parentResponder,
                          Escher::ButtonRowController* header,
                          InteractiveCurveViewRange* interactiveRange,
                          AbstractPlotView* curveView, CurveViewCursor* cursor,
                          int* selectedCurveIndex);

  // ViewController
  void viewWillAppear() override;

  // InteractiveCurveViewRangeDelegate
  void tidyModels(Poincare::PoolObject* treePoolCursor) override;

  // InteractiveCurveViewController
  int numberOfCurves() const override;

 protected:
  class FunctionSelectionController : public CurveSelectionController {
   public:
    FunctionSelectionController(FunctionGraphController* graphController)
        : CurveSelectionController(graphController) {}
    const char* title() const override {
      return I18n::translate(I18n::Message::GraphCalculus);
    }
    int numberOfRows() const override {
      return graphController()->functionStore()->numberOfActiveFunctions();
    }
    void fillCellForRow(Escher::HighlightCell* cell, int row) override;

   protected:
    void handleResponderChainEvent(ResponderChainEvent event) override;
    KDCoordinate nonMemoizedRowHeight(int row) override;
    FunctionGraphController* graphController() const {
      return static_cast<FunctionGraphController*>(
          const_cast<InteractiveCurveViewController*>(m_graphController));
    }
    virtual const Poincare::Layout nameLayoutAtIndex(int j) const = 0;

   private:
    constexpr static KDFont::Size k_font = KDFont::Size::Large;
  };

  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

  // ZoomCurveViewController
  AbstractPlotView* curveView() override;

  // SimpleInteractiveCurveViewController
  float cursorTopMarginRatio() const override { return 0.068f; }
  float cursorBottomMarginRatio() const override {
    return cursorBottomMarginRatioForBannerHeight(
        const_cast<FunctionGraphController*>(this)
            ->bannerView()
            ->minimalSizeForOptimalDisplay()
            .height());
  }
  void reloadBannerView() override;

  // InteractiveCurveViewController
  void openMenuForCurveAtIndex(int curveIndex) override;
  void initCursorParameters(bool ignorMargins) override;
  bool moveCursorVertically(OMG::VerticalDirection direction) override;
  bool selectedModelIsValid() const override;
  Poincare::Coordinate2D<double> selectedModelXyValues(double t) const override;
  Poincare::Coordinate2D<double> xyValues(int curveIndex, double t,
                                          Poincare::Context* context,
                                          int subCurveIndex = 0) const override;
  int numberOfSubCurves(int curveIndex) const override;
  bool isAlongY(int curveIndex) const override;

  virtual void selectCurveAtIndex(int curveIndex, bool willBeVisible,
                                  int subCurveIndex = -1);
  virtual double defaultCursorT(Ion::Storage::Record record,
                                bool ignoreMargins);
  virtual FunctionStore* functionStore() const;
  virtual int nextCurveIndexVertically(OMG::VerticalDirection direction,
                                       int currentCurveIndex,
                                       Poincare::Context* context,
                                       int currentSubCurveIndex,
                                       int* subCurveIndex) const {
    return closestCurveIndexVertically(direction, currentCurveIndex, context,
                                       currentSubCurveIndex, subCurveIndex);
  }
  void yRangeForCursorFirstMove(Shared::InteractiveCurveViewRange* range) const;
  Ion::Storage::Record recordAtCurveIndex(int curveIndex) const {
    return functionStore()->activeRecordAtIndex(curveIndex);
  }
  Ion::Storage::Record recordAtSelectedCurveIndex() const {
    return recordAtCurveIndex(*m_selectedCurveIndex);
  }
  void moveCursorVerticallyToPosition(int nextCurve, int nextSubCurve,
                                      double nextT);

 private:
  virtual FunctionGraphView* functionGraphView() = 0;
  virtual void openMenuForSelectedCurve() = 0;

  void computeDefaultPositionForFunctionAtIndex(
      int index, double* t, Poincare::Coordinate2D<double>* xy,
      bool ignoreMargins);
};

}  // namespace Shared

#endif
