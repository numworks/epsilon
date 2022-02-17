#ifndef PROBABILITY_CONTROLLERS_MENU_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_MENU_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/table_cell.h>

#include "probability/controllers/distribution_controller.h"
#include "probability/controllers/test_controller.h"
#include <escher/centering_view.h>
#include <escher/subapp_cell.h>
#include "probability/models/models_buffer.h"

namespace Probability {

constexpr int k_numberOfCells = 3;

// Controller
class MenuController : public Escher::SelectableCellListPage<Escher::SubappCell, k_numberOfCells> {
public:
  MenuController(Escher::StackViewController * parentResponder,
                 DistributionController * distributionController,
                 TestController * testController,
                 Inference * inference);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Escher::View * view() override { return &m_contentView; }

  constexpr static int k_indexOfProbability = 0;
  constexpr static int k_indexOfTest = 1;
  constexpr static int k_indexOfInterval = 2;

private:
  DistributionController * m_distributionController;
  TestController * m_testController;

  Inference * m_inference;

  Escher::CenteringView m_contentView;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_MENU_CONTROLLER_H */
