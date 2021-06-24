#ifndef APPS_PROBABILITY_CONTROLLERS_MENU_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_MENU_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/table_cell.h>
#include <escher/view_controller.h>

#include "probability/gui/page_controller.h"
#include "probability/gui/subapp_cell.h"
#include "probability/gui/centering_view.h"
#include "probability/models/data.h"

namespace Probability {

// Controller
class MenuController : public SelectableListViewPage {
public:
  MenuController(Escher::StackViewController * parentResponder,
                 Escher::ViewController * distributionController,
                 Escher::ViewController * testController,
                 Data::SubApp * globalSubapp,
                 Distribution * globalDistribution,
                 Calculation * globalCalculation);
  int numberOfRows() const override { return k_numberOfCells; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Escher::View * view() override { return &m_contentView; }

private:
  void initializeProbaData();
  ViewController * m_distributionController;
  ViewController * m_testController;

  constexpr static int k_numberOfCells = 3;
  constexpr static int k_indexOfDistribution = 0;
  constexpr static int k_indexOfTest = 1;
  constexpr static int k_indexOfInterval = 2;
  SubappCell m_cells[k_numberOfCells];
  Data::SubApp * m_globalSubapp;
  Distribution * m_globalDistribution;
  Calculation * m_globalCalculation;

  CenteringView m_contentView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_MENU_CONTROLLER_H */
