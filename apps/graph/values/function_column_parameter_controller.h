#ifndef GRAPH_FUNCTION_COLUMN_PARAMETER_CONTROLLER_H
#define GRAPH_FUNCTION_COLUMN_PARAMETER_CONTROLLER_H

#include <escher/message_table_cell_with_switch.h>
#include "../../shared/expiring_pointer.h"
#include "../../shared/continuous_function.h"
#include "../shared/function_parameter_controller.h"
#include "../../shared/column_parameter_controller.h"

namespace Graph {

class ValuesController;

class FunctionColumnParameterController : public FunctionParameterController, public Shared::ColumnParameters {
public:
  FunctionColumnParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphController * graphController, ValuesController * valuesController);
private:
  Shared::ClearColumnHelper * clearColumnHelper() override;

  ValuesController * m_valuesController;
  Ion::Storage::Record m_record;
};

}

#endif
