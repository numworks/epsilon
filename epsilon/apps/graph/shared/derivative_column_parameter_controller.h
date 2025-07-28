#ifndef GRAPH_DERIVATIVE_COLUMN_PARAMETER_CONTROLLER_H
#define GRAPH_DERIVATIVE_COLUMN_PARAMETER_CONTROLLER_H

#include <apps/shared/color_parameter_controller.h>
#include <apps/shared/function_store.h>
#include <apps/shared/values_controller.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

namespace Graph {

/* TODO: ParameterDelegate should only ask if the parameter is about Values or
 * Plot, but first we would need to factorise inside ContinuousFunction with an
 * enum {Plot,Values}. Most of the delegate's methods could then be factorized.
 */
class ParameterDelegate {
 public:
  virtual bool usePersonalizedTitle() const = 0;
  virtual bool showColorCell() const = 0;
  virtual I18n::Message hideDerivativeMessage() const = 0;
  virtual void hideDerivative(Ion::Storage::Record record,
                              int derivationOrder) = 0;
};

class DerivativeColumnParameterController
    : public Shared::ColumnParameterController {
 public:
  DerivativeColumnParameterController(Escher::Responder* parentResponder);

  const char* title() const override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return 2; }
  Escher::HighlightCell* cell(int row) override;
  void setRecord(Ion::Storage::Record record, int derivationOrder);
  void setParameterDelegate(ParameterDelegate* parameterDelegate) {
    m_parameterDelegate = parameterDelegate;
  }

 private:
  Shared::ClearColumnHelper* clearColumnHelper() override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_colorCell;
  Escher::MenuCell<Escher::MessageTextView> m_hideCell;
  Shared::ColorParameterController m_colorParameterController;
  Ion::Storage::Record m_record;
  int m_derivationOrder;
  ParameterDelegate* m_parameterDelegate;
};

}  // namespace Graph

#endif
