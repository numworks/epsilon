#ifndef PROBABILITY_CONTROLLERS_DISTRIBUTION_CONTROLLER_H
#define PROBABILITY_CONTROLLERS_DISTRIBUTION_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>

#include <new>

#include "parameters_controller.h"
#include "probability/gui/distribution_cell.h"
#include "probability/models/probability/distribution/distribution.h"

namespace Probability {

class DistributionController : public Escher::SelectableListViewController {
public:
  DistributionController(Escher::StackViewController * parentResponder,
                         Distribution * distribution,
                         ParametersController * parametersController);
  const char * title() override {
    return I18n::translate(I18n::Message::DistributionControllerTitle);
  }
  void stackOpenPage(Escher::ViewController * nextPage) override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("Distribution");
  int numberOfRows() const override { return k_totalNumberOfModels; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override {
    assert(type == 0);
    return k_numberOfCells;
  }

private:
  void setDistributionAccordingToIndex(int index);
  constexpr static int k_totalNumberOfModels = 9;
  constexpr static int k_numberOfCells =
      (Ion::Display::Height - Escher::Metric::TitleBarHeight - 14) /
          Escher::TableCell::k_minimalLargeFontCellHeight +
      2;  // Remaining cell can be above and below so we add +2, 14 for the small font height
  DistributionCell m_cells[k_numberOfCells];
  Distribution * m_distribution;
  ParametersController * m_parametersController;
};

}  // namespace Probability

#endif /* PROBABILITY_CONTROLLERS_DISTRIBUTION_CONTROLLER_H */
