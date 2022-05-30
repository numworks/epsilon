#ifndef INFERENCE_PROBABILITY_DISTRIBUTION_CONTROLLER_H
#define INFERENCE_PROBABILITY_DISTRIBUTION_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>

#include <new>

#include "parameters_controller.h"
#include "inference/probability/distribution_cell.h"
#include "inference/models/probability/distribution/distribution.h"

namespace Inference {

class DistributionController : public Escher::SelectableListViewController<Escher::SimpleListViewDataSource> {
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
  Escher::HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override { return k_numberOfCells; }

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

}  // namespace Inference

#endif /* INFERENCE_PROBABILITY_DISTRIBUTION_CONTROLLER_H */
