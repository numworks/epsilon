#ifndef PROBABILITY_CONTROLLERS_TEST_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_TEST_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "probability/models/statistic/statistic.h"

namespace Probability {

class HypothesisController;
class CategoricalTypeController;
class TypeController;
class InputController;

class TestController : public Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, Statistic::k_numberOfSignificanceTestType> {
public:
  TestController(Escher::StackViewController * parentResponder,
                 HypothesisController * hypothesisController,
                 TypeController * typeController,
                 CategoricalTypeController * categoricalController,
                 InputController * inputController,
                 Statistic * statistic);
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override;
  int numberOfRows() const override;

  constexpr static int k_indexOfOneProp = 0;
  constexpr static int k_indexOfOneMean = 1;
  constexpr static int k_indexOfTwoProps = 2;
  constexpr static int k_indexOfTwoMeans = 3;
  constexpr static int k_indexOfCategorical = 4;

private:
  HypothesisController * m_hypothesisController;
  TypeController * m_typeController;
  InputController * m_inputController;
  CategoricalTypeController * m_categoricalController;
  Statistic * m_statistic;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_TEST_CONTROLLER_H */
