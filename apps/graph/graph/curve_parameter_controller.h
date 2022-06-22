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
  float parameterAtIndex(int index) override {
    return m_cursor->t();
  }
  int reusableParameterCellCount(int type) override {
    return 1;//k_maxNumberOfValues;
  }
  bool setParameterAtIndex(int parameterIndex, float f) override {
    return confirmParameterAtIndex(parameterIndex, f);
  }
  Escher::HighlightCell * reusableParameterCell(int index, int type) override {
    assert(index < k_maxNumberOfValues);
    return &m_parameterCells[index];
  }
  Escher::HighlightCell * reusableCell(int index, int type) override {
    if (type == k_buttonCellType) {
      return &m_calculationCell;
    }
    return reusableParameterCell(index, type);
  }
  static constexpr int k_parameterCellType = 0;
  static constexpr int k_buttonCellType = 1;
  int typeAtIndex(int index) override {
    if (index < 1) {
      return k_parameterCellType;
    }
    return k_buttonCellType;
  }
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;

  Shared::ExpiringPointer<Shared::ContinuousFunction> function();
  Shared::InteractiveCurveViewRange * m_graphRange;
  Shared::CurveViewCursor * m_cursor;
  bool confirmParameterAtIndex(int parameterIndex, double f);
  bool shouldDisplayCalculation() const;
  int cellIndex(int visibleCellIndex) const;
  static constexpr int k_maxNumberOfValues = 3;
//  PreimageGraphController * m_preimageGraphController;
  Escher::MessageTableCellWithEditableText m_parameterCells[k_maxNumberOfValues];
  Escher::MessageTableCellWithChevron m_calculationCell;
  CalculationParameterController m_calculationParameterController;
};

}

#endif
