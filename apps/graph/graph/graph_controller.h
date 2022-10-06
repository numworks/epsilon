#ifndef GRAPH_GRAPH_CONTROLLER_H
#define GRAPH_GRAPH_CONTROLLER_H

#include "banner_view.h"
#include "curve_parameter_controller.h"
#include "graph_controller_helper.h"
#include "graph_view.h"
#include "points_of_interest_list.h"
#include "../continuous_function_store.h"
#include <apps/shared/with_record.h>
#include <apps/shared/function_graph_controller.h>
#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/round_cursor_view.h>
#include <apps/shared/interactive_curve_view_range.h>

namespace Graph {

class GraphController : public Shared::FunctionGraphController, public GraphControllerHelper {
public:
  GraphController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * rangeVersion, Escher::ButtonRowController * header);

  I18n::Message emptyMessage() override;
  void viewWillAppear() override;
  bool canShrinkWhenNormalizing() const override { return true; }

  bool displayDerivativeInBanner() const;
  PointsOfInterestList * pointsOfInterest() { return &m_pointsOfInterest; }

private:
  class FunctionSelectionController : public Shared::FunctionGraphController::FunctionSelectionController {
  public:
    FunctionSelectionController(GraphController * graphController) : Shared::FunctionGraphController::FunctionSelectionController(graphController) {}

    CurveSelectionCellWithChevron * reusableCell(int index, int type) override { assert(index >= 0 && index < k_maxNumberOfDisplayableFunctions); return m_cells + index; }
    int reusableCellCount(int type) override { return k_maxNumberOfDisplayableFunctions; }

  private:
    constexpr static int k_maxNumberOfDisplayableFunctions = 7;

    Poincare::Layout nameLayoutAtIndex(int j) const override;

    CurveSelectionCellWithChevron m_cells[k_maxNumberOfDisplayableFunctions];
  };

  // GraphControllerHelper
  BannerView * bannerView() override { return &m_bannerView; }
  void jumpToLeftRightCurve(double t, int direction, int functionsCount, Ion::Storage::Record record) override;

  void selectFunctionWithCursor(int functionIndex, bool willBeVisible) override;
  void reloadBannerView() override;
  bool moveCursorHorizontally(int direction, int scrollSpeed = 1) override;
  int nextCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context, int currentSubCurveIndex, int * nextSubCurveIndex) const override;
  double defaultCursorT(Ion::Storage::Record record) override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_graphRange; }
  GraphView * functionGraphView() override { return &m_view; }
  CurveParameterController * curveParameterController() override { return &m_curveParameterController; }
  Shared::WithRecord * curveParameterControllerWithRecord() override { return &m_curveParameterController; }
  FunctionSelectionController * curveSelectionController() const override { return const_cast<FunctionSelectionController *>(&m_functionSelectionController); }
  ContinuousFunctionStore * functionStore() const override { return static_cast<ContinuousFunctionStore *>(Shared::FunctionGraphController::functionStore()); }
  bool defaultRangeIsNormalized() const override;

  void interestingFunctionRange(Shared::ExpiringPointer<Shared::ContinuousFunction> f, float tMin, float tMax, float step, float * xm, float * xM, float * ym, float * yM) const;

  PointsOfInterestList m_pointsOfInterest;
  Shared::RoundCursorView m_cursorView;
  BannerView m_bannerView;
  GraphView m_view;
  Shared::InteractiveCurveViewRange * m_graphRange;
  CurveParameterController m_curveParameterController;
  FunctionSelectionController m_functionSelectionController;
};

}

#endif
