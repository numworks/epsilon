#ifndef GRAPH_CALCULATION_PARAMETER_CONTROLLER_H
#define GRAPH_CALCULATION_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

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
  class HideableChevron : public Escher::ChevronView {
   public:
    HideableChevron() : Escher::ChevronView(), m_displayChevron(true) {}
    const View* view() const override {
      return m_displayChevron ? this : nullptr;
    }
    bool enterOnEvent(Ion::Events::Event event) const override {
      return m_displayChevron && event == Ion::Events::Right;
    }

    void displayChevron(bool display) { m_displayChevron = display; }

   private:
    bool m_displayChevron;
  };

  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::ChevronView>
      m_preimageCell;
  Escher::MenuCell<Escher::MessageTextView> m_intersectionCell;
  Escher::MenuCell<Escher::MessageTextView> m_minimumCell;
  Escher::MenuCell<Escher::MessageTextView> m_maximumCell;
  Escher::MenuCell<Escher::MessageTextView> m_integralCell;
  Escher::MenuCell<Escher::MessageTextView> m_tangentCell;
  Escher::MenuCell<Escher::MessageTextView> m_rootCell;
  Escher::MenuCell<Escher::BufferTextView, Escher::EmptyCellWidget,
                   HideableChevron>
      m_areaCell;
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
