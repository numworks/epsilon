#include "calculation_popup_data_source.h"

#include <apps/distributions/images/calculation1_icon.h>
#include <apps/distributions/images/calculation2_icon.h>
#include <apps/distributions/images/calculation3_icon.h>
#include <apps/distributions/images/calculation4_icon.h>
#include <escher/transparent_themed_image_view.h>

namespace Distributions {

CalculationPopupDataSource::CalculationPopupDataSource(
    Distribution* distribution)
    : m_distribution(distribution) {
  updateIcons();
}

void CalculationPopupDataSource::updateIcons() {
  const Escher::ThemedImage* images[k_numberOfImages] = {
      ImageStore::Calculation1Icon, ImageStore::Calculation2Icon,
      ImageStore::Calculation3Icon, ImageStore::Calculation4Icon};
  for (int i = 0; i < k_numberOfImages; i++) {
    m_imageCells[i].setImage(
        Escher::TransparentThemedImageView(*images[i]).currentImage());
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
