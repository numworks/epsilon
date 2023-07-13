#ifndef SEQUENCE_SEQUENCE_COLUMN_PARAM_CONTROLLER_H
#define SEQUENCE_SEQUENCE_COLUMN_PARAM_CONTROLLER_H

#include <apps/shared/column_parameter_controller.h>
#include <apps/shared/continuous_function_store.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/switch_view.h>

namespace Sequence {

class ValuesController;

class SequenceColumnParameterController
    : public Shared::ColumnParameterController {
 public:
  SequenceColumnParameterController(ValuesController* valuesController);

  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_totalNumberOfCell; }
  Escher::HighlightCell* cell(int index) override {
    assert(index == 0);
    return &m_showSumCell;
  }
  void viewWillAppear() override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }

 private:
  constexpr static int k_totalNumberOfCell = 1;
  Shared::ColumnNameHelper* columnNameHelper() override;
  void updateShowSumSwitch();

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_showSumCell;
  Ion::Storage::Record m_record;
  ValuesController* m_valuesController;
};

}  // namespace Sequence

#endif
