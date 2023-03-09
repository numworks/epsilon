#ifndef GRAPH_CALCULATION_PARAMETER_CONTROLLER_H
#define GRAPH_CALCULATION_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/buffer_table_cell.h>
#include <escher/message_table_cell_with_chevron.h>

#include "area_between_curves_graph_controller.h"
#include "area_between_curves_parameter_controller.h"
#include "banner_view.h"
#include "extremum_graph_controller.h"
#include "graph_view.h"
#include "integral_graph_controller.h"
#include "intersection_graph_controller.h"
#include "preimage_parameter_controller.h"
#include "root_graph_controller.h"
#include "tangent_graph_controller.h"

namespace Graph {

class GraphController;

class CalculationParameterController
    : public Escher::ExplicitSelectableListViewController {
 public:
  CalculationParameterController(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      GraphView* graphView, BannerView* bannerView,
      Shared::InteractiveCurveViewRange* range,
      Shared::CurveViewCursor* cursor);
  const char* title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  TELEMETRY_ID("CalculationParameter");
  int numberOfRows() const override { return k_numberOfRows; }
  TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::DisplayLastTwoTitles;
  }

  Escher::HighlightCell* cell(int index) override;
  void setRecord(Ion::Storage::Record record);

 private:
  static constexpr int k_numberOfRows = 8;
  template <class T>
  void push(T* controller, bool pop);
  static bool ShouldDisplayIntersection();
  static bool ShouldDisplayAreaBetweenCurves();
  static bool ShouldDisplayChevronInAreaCell();
  // This class is used for the AreaBetweenCurves cell
  class BufferTableCellWithHideableChevron : public Escher::BufferTableCell {
   public:
    BufferTableCellWithHideableChevron()
        : Escher::BufferTableCell(), m_displayChevron(true) {}
    const Escher::View* accessoryView() const override {
      return m_displayChevron ? &m_accessoryView : nullptr;
    }
    void displayChevron(bool display) { m_displayChevron = display; }
    bool subviewsCanOverlap() const override { return true; }
    bool shouldEnterOnEvent(Ion::Events::Event event) {
      return m_displayChevron
                 ? Escher::MessageTableCellWithChevron::ShouldEnterOnEvent(
                       event)
                 : Escher::MessageTableCell::ShouldEnterOnEvent(event);
    }

   private:
    Escher::ChevronView m_accessoryView;
    bool m_displayChevron;
  };
  Escher::MessageTableCellWithChevron m_preimageCell;
  Escher::MessageTableCell m_intersectionCell;
  Escher::MessageTableCell m_minimumCell;
  Escher::MessageTableCell m_maximumCell;
  Escher::MessageTableCell m_integralCell;
  Escher::MessageTableCell m_tangentCell;
  Escher::MessageTableCell m_rootCell;
  BufferTableCellWithHideableChevron m_areaCell;
  Ion::Storage::Record m_record;
  PreimageParameterController m_preimageParameterController;
  PreimageGraphController m_preimageGraphController;
  TangentGraphController m_tangentGraphController;
  IntegralGraphController m_integralGraphController;
  AreaBetweenCurvesParameterController m_areaParameterController;
  AreaBetweenCurvesGraphController m_areaGraphController;
  MinimumGraphController m_minimumGraphController;
  MaximumGraphController m_maximumGraphController;
  RootGraphController m_rootGraphController;
  IntersectionGraphController m_intersectionGraphController;
};

}  // namespace Graph

#endif
