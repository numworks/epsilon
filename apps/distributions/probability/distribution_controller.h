#ifndef DISTRIBUTIONS_PROBABILITY_DISTRIBUTION_CONTROLLER_H
#define DISTRIBUTIONS_PROBABILITY_DISTRIBUTION_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/table_view_with_frozen_header.h>

#include <new>

#include "distributions/models/distribution/distribution.h"
#include "distributions/probability/distribution_cell.h"
#include "parameters_controller.h"

namespace Distributions {

class DistributionController : public Escher::SelectableListViewController<
                                   Escher::SimpleListViewDataSource> {
 public:
  DistributionController(Escher::StackViewController* parentResponder,
                         Distribution* distribution,
                         ParametersController* parametersController);

  TELEMETRY_ID("Distribution");

  Escher::View* view() override { return &m_contentView; }
  void stackOpenPage(Escher::ViewController* nextPage) override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override { return k_totalNumberOfModels; }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  Escher::HighlightCell* reusableCell(int index) override;
  int reusableCellCount() const override { return k_numberOfCells; }
  KDCoordinate defaultRowHeight() override;

 private:
  constexpr static int k_totalNumberOfModels = 10;
  constexpr static int k_numberOfCells =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::TableCell::k_minimalLargeFontCellHeight,
          KDFont::GlyphHeight(KDFont::Size::Small));

  void setDistributionAccordingToIndex(int index);

  Escher::TableViewWithFrozenHeader m_contentView;
  DistributionCell m_cells[k_numberOfCells];
  Distribution* m_distribution;
  ParametersController* m_parametersController;
};

}  // namespace Distributions

#endif
