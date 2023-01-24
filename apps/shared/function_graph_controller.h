#ifndef SHARED_FUNCTION_GRAPH_CONTROLLER_H
#define SHARED_FUNCTION_GRAPH_CONTROLLER_H

#include "function_banner_delegate.h"
#include "interactive_curve_view_controller.h"
#include "function_store.h"
#include "function_graph_view.h"
#include "with_record.h"

namespace Shared {

class FunctionGraphController : public InteractiveCurveViewController, public FunctionBannerDelegate {
public:
  FunctionGraphController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header,  InteractiveCurveViewRange * interactiveRange, AbstractPlotView * curveView, CurveViewCursor * cursor, int * selectedCurveIndex);

  // Responder
  void didBecomeFirstResponder() override;

  // ViewController
  void viewWillAppear() override;

  // InteractiveCurveViewRangeDelegate
  void tidyModels() override;

  // InteractiveCurveViewController
  int numberOfCurves() const override;

protected:

  class FunctionSelectionController : public CurveSelectionController {
  public:
    FunctionSelectionController(FunctionGraphController * graphController) : CurveSelectionController(graphController) {}
    const char * title() override { return I18n::translate(I18n::Message::GraphCalculus); }
    int numberOfRows() const override { return graphController()->functionStore()->numberOfActiveFunctions(); }
    void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
    void didBecomeFirstResponder() override;
  protected:
    KDCoordinate nonMemoizedRowHeight(int j) override;
    FunctionGraphController * graphController() const { return static_cast<FunctionGraphController *>(const_cast<InteractiveCurveViewController *>(m_graphController)); }
    virtual Poincare::Layout nameLayoutAtIndex(int j) const = 0;
  private:
    constexpr static KDFont::Size k_font = KDFont::Size::Large;
  };

  // ZoomCurveViewController
  AbstractPlotView * curveView() override;

  // SimpleInteractiveCurveViewController
  float cursorTopMarginRatio() const override { return 0.068f; }
  float cursorBottomMarginRatio() const override { return cursorBottomMarginRatioForBannerHeight(const_cast<FunctionGraphController *>(this)->bannerView()->minimalSizeForOptimalDisplay().height()); }
  void reloadBannerView() override;

  // InteractiveCurveViewController
  bool openMenuForCurveAtIndex(int curveIndex) override;
  void initCursorParameters(bool ignorMargins) override;
  bool moveCursorVertically(int direction) override;
  bool selectedModelIsValid() const override;
  Poincare::Coordinate2D<double> selectedModelXyValues(double t) const override;
  int selectedCurveIndex() const override { return *m_selectedCurveIndex; }
  Poincare::Coordinate2D<double> xyValues(int curveIndex, double t, Poincare::Context * context, int subCurveIndex = 0) const override;
  int numberOfSubCurves(int curveIndex) const override;
  bool isAlongY(int curveIndex) const override;

  void selectCurveAtIndex(int curveIndex, bool willBeVisible);
  virtual double defaultCursorT(Ion::Storage::Record record, bool ignoreMargins);
  virtual FunctionStore * functionStore() const;
  virtual int nextCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context, int currentSubCurveIndex, int * subCurveIndex) const {
    return closestCurveIndexVertically(goingUp, currentSelectedCurve, context, currentSubCurveIndex, subCurveIndex);
  }
  void yRangeForCursorFirstMove(Shared::InteractiveCurveViewRange * range) const;
  Ion::Storage::Record recordAtCurveIndex(int curveIndex) const { return functionStore()->activeRecordAtIndex(curveIndex); }
  Ion::Storage::Record recordAtSelectedCurveIndex() const { return recordAtCurveIndex(*m_selectedCurveIndex); }

private:
  virtual FunctionGraphView * functionGraphView() = 0;

  /* These two methods are likely to point to the same object but they are
   * separated to avoid diamond inheritance */
  virtual Escher::ViewController * curveParameterController() = 0;
  virtual WithRecord * curveParameterControllerWithRecord() = 0;

  void computeDefaultPositionForFunctionAtIndex(int index, double * t, Poincare::Coordinate2D<double> * xy, bool ignoreMargins);

  int * m_selectedCurveIndex;
};

}

#endif
