#pragma once

#include <apps/shared/color_parameter_controller.h>
#include <apps/shared/continuous_function.h>
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
  constexpr static int k_isStandaloneDerivative = -1;
  virtual bool usePersonalizedTitle() const = 0;
  virtual bool showColorCell() const = 0;
  virtual I18n::Message hideDerivativeMessage() const = 0;
  virtual void hideDerivative(Ion::Storage::Record record, int derivationOrder);
};

/* This controller handles parameters for two kinds of derivative columns:
 * - Standalone derivative records: m_derivationOrder ==
 *   k_isStandaloneDerivative, m_record holds the derivative record itself.
 *   (used in ListController and CurveParameterController)
 * - Derivative values columns dependent on a parent: m_derivationOrder == 1
 *   or 2, m_record holds the parent function record.
 *   (used in ValuesController)
 */
class DerivativeColumnParameterController
    : public Shared::ColumnParameterController {
 public:
  DerivativeColumnParameterController(Escher::Responder* parentResponder);

  const char* title() const override;
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return 2; }
  const Escher::HighlightCell* cell(int row) const override;
  void setRecord(
      Ion::Storage::Record record,
      int derivationOrder = ParameterDelegate::k_isStandaloneDerivative);
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
  int m_derivationOrder;
  ParameterDelegate* m_parameterDelegate;
};

}  // namespace Graph
