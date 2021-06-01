#ifndef APPS_PROBABILITY_CONTROLLERS_TEST_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_TEST_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/responder.h>
#include <ion/events.h>

#include "probability/gui/selectable_cell_list_controller.h"
#include "probability/models/data.h"

namespace Probability {

class HypothesisController;
class CategoricalTypeController;
class TypeController;
class IntervalInputController;

constexpr static int k_numberOfTestCells = 5;

class TestController : public SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage,
                                                     k_numberOfTestCells> {
public:
  TestController(Escher::StackViewController * parentResponder,
                 HypothesisController * hypothesisController, TypeController * typeController,
                 CategoricalTypeController * categoricalController,
                 IntervalInputController * intervalInputController);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override { return "Test"; }

private:
  void selectRowAccordingToTest(Data::Test t);
  void initializeHypothesisParams(Data::Test t);

  constexpr static int k_indexOfOneProp = 0;
  constexpr static int k_indexOfOneMean = 1;
  constexpr static int k_indexOfTwoProps = 2;
  constexpr static int k_indexOfTwoMeans = 3;
  constexpr static int k_indexOfCategorical = 4;

  HypothesisController * m_hypothesisController;
  TypeController * m_typeController;
  IntervalInputController * m_intervalInputController;
  CategoricalTypeController * m_categoricalController;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_TEST_CONTROLLER_H */
