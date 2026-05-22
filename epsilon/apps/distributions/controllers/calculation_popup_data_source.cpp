#include "calculation_popup_data_source.h"

#include "calculation1_light_icon.h"
#include "calculation2_light_icon.h"
#include "calculation3_light_icon.h"
#include "calculation4_light_icon.h"

namespace Distributions {

CalculationPopupDataSource::CalculationPopupDataSource(
    Distribution* distribution)
    : m_distribution(distribution) {
  /* We can init the image of all cells because there is a bijection of the
   * displayable cells and the source cells. */
  const Escher::Image* images[k_numberOfImages] = {
      ImageStore::Calculation1LightIcon, ImageStore::Calculation2LightIcon,
      ImageStore::Calculation3LightIcon, ImageStore::Calculation4LightIcon};
  for (int i = 0; i < k_numberOfImages; i++) {
    m_imageCells[i].setImage(images[i]);
  }
}

int CalculationPopupDataSource::numberOfRows() const {
  if (!m_distribution->allParametersAreInitialized()) {
    // Only display left and right integral
    return 2;
  }
  return k_numberOfImages - m_distribution->isContinuous();
}

int CalculationPopupDataSource::absoluteRow(int currentRow) const {
  if (!m_distribution->allParametersAreInitialized() && currentRow == 1) {
    // Only display left and right integral
    return 2;
  }
  return currentRow;
}

}  // namespace Distributions
