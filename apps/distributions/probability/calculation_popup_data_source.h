#ifndef DISTRIBUTIONS_PROBABILITY_CALCULATION_POPUP_DATA_SOURCE_H
#define DISTRIBUTIONS_PROBABILITY_CALCULATION_POPUP_DATA_SOURCE_H

#include <escher/highlight_image_cell.h>
#include <escher/list_view_data_source.h>

#include "distributions/models/distribution/distribution.h"

namespace Distributions {

/* Provides the views shown to select the desired calculation. */
class CalculationPopupDataSource : public Escher::ListViewDataSource {
 public:
  CalculationPopupDataSource(Distribution* distribution);
  int numberOfRows() const override;
  int reusableCellCount(int type) override { return numberOfRows(); }
  Escher::HighlightImageCell* reusableCell(int i, int type) override {
    return m_imageCells + absoluteRowIndex(i);
  }
  int absoluteRowIndex(int currentRowIndex) const;

  constexpr static int k_numberOfImages = 4;

 private:
  KDCoordinate nonMemoizedRowHeight(int r) override {
    assert(false); /* Not needed because DropdownPopupController takes care of
                      it */
    return 1;
  }
  Escher::HighlightImageCell m_imageCells[k_numberOfImages];
  Distribution* m_distribution;
};

}  // namespace Distributions

#endif
