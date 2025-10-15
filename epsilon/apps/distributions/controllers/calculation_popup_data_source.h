#pragma once

#include <escher/explicit_list_view_data_source.h>
#include <escher/highlight_image_cell.h>

#include "distributions/models/distribution/distribution.h"

namespace Distributions {

/* Provides the views shown to select the desired calculation. */
class CalculationPopupDataSource : public Escher::ExplicitListViewDataSource {
 public:
  CalculationPopupDataSource(Distribution* distribution);
  int numberOfRows() const override;
  const Escher::HighlightCell* cell(int row) const override {
    return &m_imageCells[absoluteRow(row)];
  }
  int absoluteRow(int currentRow) const;

 private:
  constexpr static int k_numberOfImages = 4;
  Escher::HighlightImageCell m_imageCells[k_numberOfImages];
  Distribution* m_distribution;
};

}  // namespace Distributions
