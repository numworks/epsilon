#ifndef SEQUENCE_SUM_COLUMN_PARAM_CONTROLLER_H
#define SEQUENCE_SUM_COLUMN_PARAM_CONTROLLER_H

#include <apps/shared/function_store.h>
#include <apps/shared/values_controller.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

namespace Sequence {

class SumColumnParameterController : public Shared::ColumnParameterController {
 public:
  SumColumnParameterController(Shared::ValuesController* valuesController);

  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_totalNumberOfCell; }
  Escher::HighlightCell* cell(int row) override {
    assert(row == 0);
    return &m_hideColumn;
  }
  void setRecord(Ion::Storage::Record record) { m_record = record; }

 protected:
  Shared::ClearColumnHelper* clearColumnHelper() override {
    return m_valuesController;
  }
  constexpr static int k_totalNumberOfCell = 1;
  Escher::MenuCell<Escher::MessageTextView> m_hideColumn;
  Ion::Storage::Record m_record;
  Shared::ValuesController* m_valuesController;
};

}  // namespace Sequence

#endif
