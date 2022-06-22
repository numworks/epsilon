#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_table_cell_with_buffer.h>
#include "../../shared/float_parameter_controller_without_button.h"
#include "../../shared/with_record.h"
#include "calculation_parameter_controller.h"
#include "banner_view.h"

namespace Graph {

class GraphController;

class CurveParameterController : public Shared::FloatParameterControllerWithoutButton<float>, public Shared::WithRecord {
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
  static constexpr int k_parameterCellType = 0;
  static constexpr int k_calculationCellType = 1;
  int typeAtIndex(int index) override;
  int numberOfParameters() const { return 2; }
  Escher::HighlightCell * reusableCell(int index, int type);
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;

  Shared::ExpiringPointer<Shared::ContinuousFunction> function();
  Shared::InteractiveCurveViewRange * m_graphRange;
  Shared::CurveViewCursor * m_cursor;
  bool confirmParameterAtIndex(int parameterIndex, double f);
  bool shouldDisplayCalculation() const;
  int cellIndex(int visibleCellIndex) const;
  static constexpr int k_maxNumberOfParameters = 3;
  PreimageGraphController m_preimageGraphController;
  Escher::MessageTableCellWithEditableText m_parameterCells[k_maxNumberOfParameters];
  Escher::MessageTableCellWithChevron m_calculationCell;
  CalculationParameterController m_calculationParameterController;
};

}

#endif
