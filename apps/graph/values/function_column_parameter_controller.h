#ifndef GRAPH_FUNCTION_COLUMN_PARAMETER_CONTROLLER_H
#define GRAPH_FUNCTION_COLUMN_PARAMETER_CONTROLLER_H

#include <escher/message_table_cell_with_switch.h>
#include "../../shared/expiring_pointer.h"
#include "../../shared/continuous_function.h"
#include "../../shared/column_parameter_controller.h"

namespace Graph {

class ValuesController;

class FunctionColumnParameterController : public Shared::ColumnParameterController {
public:
  FunctionColumnParameterController(ValuesController * valuesController);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }
private:
  constexpr static int k_totalNumberOfCell = 1;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function();
  Shared::ClearColumnHelper * clearColumnHelper() override;

  Escher::MessageTableCellWithSwitch m_displayDerivativeColumn;
  ValuesController * m_valuesController;
  Ion::Storage::Record m_record;
};

}

#endif
