#ifndef GRAPH_DERIVATIVE_COLUMN_PARAMETER_CONTROLLER_H
#define GRAPH_DERIVATIVE_COLUMN_PARAMETER_CONTROLLER_H

#include <apps/shared/color_parameter_controller.h>
#include <apps/shared/function_store.h>
#include <apps/shared/values_controller.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

namespace Graph {

class DerivativeColumnParameterController
    : public Shared::ColumnParameterController {
 public:
  DerivativeColumnParameterController(
      Shared::ValuesController* valuesController);

  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return 2; }
  Escher::HighlightCell* cell(int row) override;
  void setDerivationOrder(int derivationOrder) {
    assert(derivationOrder == 1 || derivationOrder == 2);
    m_derivationOrder = derivationOrder;
  }
  void setRecord(Ion::Storage::Record record) { m_record = record; }

 private:
  Shared::ColumnNameHelper* columnNameHelper() override {
    return m_valuesController;
  }
  Shared::ExpiringPointer<Shared::ContinuousFunction> function();

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_colorCell;
  Escher::MenuCell<Escher::MessageTextView> m_hideColumn;
  Shared::ColorParameterController m_colorParameterController;
  Ion::Storage::Record m_record;
  int m_derivationOrder;
  Shared::ValuesController* m_valuesController;
};

}  // namespace Graph

#endif
