#ifndef SEQUENCE_SEQUENCE_COLUMN_PARAM_CONTROLLER_H
#define SEQUENCE_SEQUENCE_COLUMN_PARAM_CONTROLLER_H

#include <apps/shared/continuous_function_store.h>
#include <escher/message_table_cell_with_message_with_switch.h>
#include <escher/selectable_list_view_controller.h>

#include "../../shared/column_parameter_controller.h"

namespace Sequence {

class ValuesController;

class SequenceColumnParameterController
    : public Shared::ColumnParameterController {
 public:
  SequenceColumnParameterController(ValuesController* valuesController);

  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_totalNumberOfCell; }
  Escher::HighlightCell* reusableCell(int index, int type) override {
    assert(index == 0);
    return &m_showSumCell;
  }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }

 private:
  constexpr static int k_totalNumberOfCell = 1;
  Shared::ColumnNameHelper* columnNameHelper() override;
  Escher::MessageTableCellWithMessageWithSwitch m_showSumCell;
  Ion::Storage::Record m_record;
  ValuesController* m_valuesController;
};

}  // namespace Sequence

#endif
