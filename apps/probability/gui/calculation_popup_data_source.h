#ifndef PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H
#define PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

#include "highlight_image_cell.h"
#include "probability/models/distribution/distribution.h"

namespace Probability {

/* Provides the views shown to select the desired calculation. */
class CalculationPopupDataSource : public Escher::ListViewDataSource {
public:
  CalculationPopupDataSource(Distribution * distribution);
  int numberOfRows() const override;
  KDCoordinate rowHeight(int r) override {
    assert(false); /* Not needed because DropdownPopupController takes care of it */
    return 1;
  }
  int reusableCellCount(int type) override { return k_numberOfImages; }
  HighlightImageCell * reusableCell(int i, int type) override { return &m_imageCells[i]; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  constexpr static int k_numberOfImages = 4;

private:
  HighlightImageCell m_imageCells[k_numberOfImages];
  Distribution * m_distribution;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H */
