#pragma once

#include <apps/shared/continuous_function.h>
#include <apps/shared/continuous_function_store.h>
#include <apps/shared/function_graph_view.h>
#include <poincare/numeric_solver/solver.h>

#include "interest_view.h"

namespace Graph {

class GraphView : public Shared::FunctionGraphView,
                  public Shared::PlotPolicy::WithCurves {
  friend class InterestView;

 public:
  constexpr static int k_maxNumberOfInequalitiesInIntersectionRegion = 8;
  GraphView(Shared::InteractiveCurveViewRange* graphRange,
            Shared::CurveViewCursor* cursor, Shared::BannerView* bannerView,
            Shared::MemoizedCursorView* cursorView);

  // FunctionGraphView
  void reload(bool resetInterrupted = false, bool force = false,
              bool forceRedrawAxes = false) override;
  void drawRect(KDContext* ctx, KDRect rect) const override;
  int selectedRecordIndex() const override;

  /* We override setAreaHighlightColor to make it reload nothing as the
   * highlightColor and the non-highlightColor are identical in the graph view
   * of the application graph. We thereby avoid to uselessly reload some part
   * of the graph where the area under the curve is colored. */
  void setAreaHighlightColor(bool highlightColor) override{};
  int numberOfDrawnRecords() const override;
  void drawRecord(Ion::Storage::Record record, int index, KDContext* ctx,
                  KDRect rect, bool firstDrawnRecord) const override;
  // Color the area intersected by all active inequalities
  void drawInequalitiesIntersection(KDContext* ctx, KDRect rect) const override;
  void tidyModel(int i,
                 const Poincare::PoolObject* treePoolCursor) const override;
  void setFocus(bool focus) override;
  Shared::ContinuousFunctionStore* functionStore() const override;

  void setInterest(Poincare::Solver<double>::Interest interest) {
    m_interest = interest;
  }
  void resumePointsOfInterestDrawing();

  void setTangentDisplay(bool display) { m_tangentDisplay = display; }

  void setIntersectionRegionDisplay(bool display) {
    m_intersectionRegion = display;
  }

 private:
  constexpr static int k_externRectMargin = 2;
  constexpr static Shared::Dots::Size k_dotSize = Shared::Dots::Size::Tiny;

  static bool FunctionIsDiscontinuousOnFloatInterval(float minBound,
                                                     float maxBound,
                                                     const void* model);
  Escher::View* ornamentView() const override {
    return const_cast<InterestView*>(&m_interestView);
  }
  void drawCartesian(KDContext* ctx, KDRect rect, Shared::ContinuousFunction* f,
                     Ion::Storage::Record record, float tMin, float tMax,
                     float tStep, DiscontinuityTest discontinuity,
                     OMG::Axis axis) const;
  void drawPolar(KDContext* ctx, KDRect rect, Shared::ContinuousFunction* f,
                 float tMin, float tMax, float tStep,
                 DiscontinuityTest discontinuity) const;
  void drawFunction(KDContext* ctx, KDRect rect, Shared::ContinuousFunction* f,
                    float tMin, float tMax, float tStep,
                    DiscontinuityTest discontinuity) const;
  void drawScatterPlot(KDContext* ctx, KDRect rect,
                       Shared::ContinuousFunction* f) const;
  void drawPointsOfInterest(KDContext* ctx, KDRect rect);
  void drawPointsOfInterest(KDContext* ctx, KDRect rect,
                            Ion::Storage::Record record,
                            bool shouldComputePoints);
  void drawTangent(KDContext* ctx, KDRect rect,
                   Ion::Storage::Record record) const;

  KDRect boundsWithoutBanner() const;

  void drawAxesAndGrid(KDContext* ctx, KDRect rect) const override;

  template <typename T>
  Curve2DEvaluation<T> subCurveEvaluation(Shared::ContinuousFunction* f,
                                          int subCurveIndex) const;

  InterestView m_interestView;
  mutable int m_areaIndex;
  mutable int m_nextPointOfInterestIndex;

  Poincare::Solver<double>::Interest m_interest;
  bool m_computePointsOfInterest;
  bool m_tangentDisplay;
  bool m_intersectionRegion;
};

}  // namespace Graph
