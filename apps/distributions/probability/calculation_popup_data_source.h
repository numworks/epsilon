#ifndef DISTRIBUTIONS_PROBABILITY_CALCULATION_POPUP_DATA_SOURCE_H
#define DISTRIBUTIONS_PROBABILITY_CALCULATION_POPUP_DATA_SOURCE_H

#include <escher/explicit_list_view_data_source.h>
#include <escher/highlight_image_cell.h>

#include "distributions/models/distribution/distribution.h"

namespace Distributions {

/* Provides the views shown to select the desired calculation. */
class CalculationPopupDataSource : public Escher::ExplicitListViewDataSource {
 public:
  CalculationPopupDataSource(Distribution* distribution);
  int numberOfRows() const override;
  Escher::HighlightCell* cell(int row) override {
    return &m_imageCells[absoluteRow(row)];
  }
  int absoluteRow(int currentRow) const;

 private:
  constexpr static int k_numberOfImages = 4;
  Escher::HighlightImageCell m_imageCells[k_numberOfImages];
  Distribution* m_distribution;
};

}  // namespace Distributions

#endif
