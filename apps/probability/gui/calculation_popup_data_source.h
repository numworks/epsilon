#ifndef APPS_PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H
#define APPS_PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H

#include <escher/image.h>
#include <escher/list_view_data_source.h>

#include "highlight_image_cell.h"

namespace Probability {

/* Provides the views shown to select the desired calculation. */
class CalculationPopupDataSource : public Escher::ListViewDataSource {
public:
  int numberOfRows() const override { return k_numberOfImages; }
  KDCoordinate rowHeight(int r) override { return 20; }
  int reusableCellCount(int type) override { return k_numberOfImages; }
  Escher::HighlightCell * reusableCell(int i, int type) override { return &m_imageCells[i]; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  constexpr static int k_numberOfImages = 4;
  HighlightImageCell m_imageCells[k_numberOfImages];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H */
