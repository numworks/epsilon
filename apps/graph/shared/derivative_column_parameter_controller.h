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
  DerivativeColumnParameterController(Escher::Responder* parentResponder);

  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return 2; }
  Escher::HighlightCell* cell(int row) override;
  void setRecord(Ion::Storage::Record record, int derivationOrder);

 private:
  Shared::ColumnNameHelper* columnNameHelper() override {
    return valuesController();
  }
  Shared::ExpiringPointer<Shared::ContinuousFunction> function();
  Shared::ValuesController* valuesController();

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_colorCell;
  Escher::MenuCell<Escher::MessageTextView> m_hideColumn;
  Shared::ColorParameterController m_colorParameterController;
  Ion::Storage::Record m_record;
  int m_derivationOrder;
};

}  // namespace Graph

#endif
