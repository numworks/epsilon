#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/buffer_table_cell_with_editable_text.h>
#include "../../shared/float_parameter_controller_without_button.h"
#include "../../shared/with_record.h"
#include "calculation_parameter_controller.h"
#include "banner_view.h"

namespace Graph {

class GraphController;

class CurveParameterController : public Shared::FloatParameterControllerWithoutButton<float,Escher::BufferTableCellWithEditableText>, public Shared::WithRecord {
public:
  CurveParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * graphRange, BannerView * bannerView, Shared::CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController);
  const char * title() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  float parameterAtIndex(int index) override;
  int reusableCellCount(int type) override {
    return type == k_parameterCellType ? numberOfParameters() : shouldDisplayCalculation();
  }
  bool setParameterAtIndex(int parameterIndex, float f) override {
    return confirmParameterAtIndex(parameterIndex, f);
  }
  void setRecord(Ion::Storage::Record record) override {
    Shared::WithRecord::setRecord(record);
    m_preimageGraphController.setRecord(record);
  }
  constexpr static int k_parameterCellType = 0;
  constexpr static int k_calculationCellType = 1;
  constexpr static int k_maxNumberOfParameters = 3;
  constexpr static int k_preimageIndex = 1;
  constexpr static int k_derivativeIndex = 2;
  bool editableParameter(int index);
  int typeAtIndex(int index) override;
  int numberOfParameters() const { return function()->numberOfCurveParameters() + shouldDisplayDerivative(); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;

  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  bool confirmParameterAtIndex(int parameterIndex, double f);
  bool shouldDisplayCalculation() const;
  bool shouldDisplayDerivative() const;
  bool isDerivative(int index) const;
  int cellIndex(int visibleCellIndex) const;
  GraphController * m_graphController;
  Shared::InteractiveCurveViewRange * m_graphRange;
  Shared::CurveViewCursor * m_cursor;
  PreimageGraphController m_preimageGraphController;
  Escher::BufferTableCellWithEditableText m_parameterCells[k_maxNumberOfParameters];
  Escher::MessageTableCellWithChevron m_calculationCell;
  CalculationParameterController m_calculationParameterController;
};

}

#endif
