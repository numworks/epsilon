#ifndef SOLVER_CONTROLLERS_FINANCE_RESULT_CONTROLLER_H
#define SOLVER_CONTROLLERS_FINANCE_RESULT_CONTROLLER_H

#include <escher/message_table_cell_with_message_with_buffer.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "../gui/selectable_cell_list_controller.h"
#include "../model/data.h"


namespace Solver {

// TODO Hugo : Add "Calculated value" intro message just like in SimpleInterestMenuController
// TODO Hugo : Factorize with compound interest result controller

constexpr static int k_numberOfResultCells = 1;

class FinanceResultController : public SelectableCellListPage<Escher::MessageTableCellWithMessageWithBuffer,
                                                     k_numberOfResultCells> {
public:
  FinanceResultController(Escher::StackViewController * parentResponder, FinanceData * data);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastAndThirdToLast; }
  int numberOfRows() const override { return k_numberOfResultCells; }
private:
  SimpleInterestData * simpleInterestData() const { assert(m_data->isSimpleInterest); return &(m_data->m_data.m_simpleInterestData); }
  CompoundInterestData * compoundInterestData() const { assert(!m_data->isSimpleInterest); return &(m_data->m_data.m_compoundInterestData); }
  int stackTitleStyleStep() const override { return 1; }
  FinanceData * m_data;
};

}  // namespace Solver

#endif /* SOLVER_CONTROLLERS_FINANCE_RESULT_CONTROLLER_H */
