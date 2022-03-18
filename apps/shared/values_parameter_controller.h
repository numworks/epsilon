#ifndef SHARED_VALUES_PARAM_CONTROLLER_H
#define SHARED_VALUES_PARAM_CONTROLLER_H

#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/selectable_list_view_controller.h>
#include "column_parameter_controller.h"
#include "interval_parameter_controller.h"

namespace Shared {

class ValuesController;

class ValuesParameterController : public ColumnParameterController {
public:
  ValuesParameterController(Escher::Responder * parentResponder, ValuesController * valuesController);
  int numberOfRows() const override { return k_totalNumberOfCell; }
  bool handleEvent(Ion::Events::Event event) override;
  int reusableCellCount() const override { return k_totalNumberOfCell; }
  Escher::HighlightCell * reusableCell(int index) override;
  void initializeColumnParameters() override;
private:
  EditableCellTableViewController * editableCellTableViewController() override;
  constexpr static int k_totalNumberOfCell = 2;
  constexpr static int k_indexOfClearColumn = 0;
  constexpr static int k_indexOfSetInterval = k_indexOfClearColumn + 1;

  Escher::MessageTableCell m_clearColumn;
  Escher::MessageTableCellWithChevron m_setInterval;

  ValuesController * m_valuesController;
};

}

#endif
