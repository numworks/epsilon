#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/buffer_table_cell_with_editable_text.h>
#include <escher/spacer_cell.h>
#include "../../shared/explicit_float_parameter_controller.h"
#include "../../shared/with_record.h"
#include "calculation_parameter_controller.h"
#include "banner_view.h"

namespace Graph {

class GraphController;

class CurveParameterController : public Shared::ExplicitFloatParameterController, public Shared::WithRecord {
public:
  CurveParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * graphRange, BannerView * bannerView, Shared::CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController);
  const char * title() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_numberOfRows; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void viewWillAppear() override;
  TitlesDisplay titlesDisplay() override { return TitlesDisplay::DisplayLastTitle; }
private:
  constexpr static int k_numberOfRows = 6;
  float parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, float f) override {
    return confirmParameterAtIndex(parameterIndex, f);
  }
  void setRecord(Ion::Storage::Record record) override;
  bool editableParameter(int index);
  int numberOfParameters() const { return function()->properties().numberOfCurveParameters() + shouldDisplayDerivative(); }
  Escher::HighlightCell * cell(int index) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;

  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  bool confirmParameterAtIndex(int parameterIndex, double f);
  bool shouldDisplayCalculation() const;
  bool shouldDisplayDerivative() const;
  bool isDerivative(int index) { return cell(index) == &m_derivativeNumberCell && function()->properties().numberOfCurveParameters() == 2; };
  int cellIndex(int visibleCellIndex) const;
  /* max(Function::k_maxNameWithArgumentSize + CalculateOnFx, CalculateOnTheCurve + max(Color*Curve)) */
  static constexpr size_t k_titleSize = 40; // "Berechnen auf der türkisen Kurve"
  char m_title[k_titleSize];
  Escher::BufferTableCellWithEditableText m_abscissaCell;
  Escher::BufferTableCellWithEditableText m_imageCell;
  Escher::BufferTableCellWithEditableText m_derivativeNumberCell;
  Escher::SpacerCell m_spacer;
  Escher::MessageTableCellWithChevron m_calculationCell;
  Escher::MessageTableCellWithChevron m_optionsCell;
  GraphController * m_graphController;
  Shared::InteractiveCurveViewRange * m_graphRange;
  Shared::CurveViewCursor * m_cursor;
  PreimageGraphController m_preimageGraphController;
  CalculationParameterController m_calculationParameterController;
};

}

#endif
