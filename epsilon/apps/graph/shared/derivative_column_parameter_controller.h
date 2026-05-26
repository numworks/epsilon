#pragma once

#include <apps/shared/color_parameter_controller.h>
#include <apps/shared/continuous_function.h>
#include <apps/shared/function_store.h>
#include <apps/shared/values_controller.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

namespace Graph {

class ParameterDelegate {
 public:
  enum DerivativeType { Plot, Values };
  virtual DerivativeType type() const = 0;
  virtual void hideDerivative(Ion::Storage::Record record) = 0;
};

class DerivativeColumnParameterController
    : public Shared::ColumnParameterController {
 public:
  DerivativeColumnParameterController(Escher::Responder* parentResponder);

  const char* title() const override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return 2; }
  const Escher::HighlightCell* cell(int row) const override;
  void setRecord(Ion::Storage::Record record);
  void setParameterDelegate(ParameterDelegate* parameterDelegate) {
    m_parameterDelegate = parameterDelegate;
  }

 private:
  Shared::ClearColumnHelper* clearColumnHelper() override;
  OMG::ExpiringPointer<Shared::ContinuousFunction> function() const;

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_colorCell;
  Escher::MenuCell<Escher::MessageTextView> m_hideCell;
  Shared::ColorParameterController m_colorParameterController;
  Ion::Storage::Record m_record;
  ParameterDelegate* m_parameterDelegate;
};

}  // namespace Graph
