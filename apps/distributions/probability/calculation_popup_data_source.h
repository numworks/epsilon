#ifndef DISTRIBUTIONS_PROBABILITY_CALCULATION_POPUP_DATA_SOURCE_H
#define DISTRIBUTIONS_PROBABILITY_CALCULATION_POPUP_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

#include <escher/highlight_image_cell.h>
#include "distributions/models/probability/distribution/distribution.h"

namespace Inference {

/* Provides the views shown to select the desired calculation. */
class CalculationPopupDataSource : public Escher::ListViewDataSource {
public:
  CalculationPopupDataSource(Distribution * distribution);
  int numberOfRows() const override;
  KDCoordinate rowHeight(int r) override {
    assert(false); /* Not needed because DropdownPopupController takes care of it */
    return 1;
  }
  int reusableCellCount(int type) override { return numberOfRows(); }
  Escher::HighlightImageCell * reusableCell(int i, int type) override { return &m_imageCells[i]; }

  constexpr static int k_numberOfImages = 4;

private:
  Escher::HighlightImageCell m_imageCells[k_numberOfImages];
  Distribution * m_distribution;
};

}  // namespace Inference

#endif /* DISTRIBUTIONS_PROBABILITY_CALCULATION_POPUP_DATA_SOURCE_H */
