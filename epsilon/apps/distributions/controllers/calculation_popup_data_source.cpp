#include "calculation_popup_data_source.h"

#include <apps/global_preferences.h>

#include "calculation1_dark_icon.h"
#include "calculation1_light_icon.h"
#include "calculation2_dark_icon.h"
#include "calculation2_light_icon.h"
#include "calculation3_dark_icon.h"
#include "calculation3_light_icon.h"
#include "calculation4_dark_icon.h"
#include "calculation4_light_icon.h"

namespace Distributions {

CalculationPopupDataSource::CalculationPopupDataSource(
    Distribution* distribution)
    : m_distribution(distribution) {
  updateIcons();
}

void CalculationPopupDataSource::updateIcons() {
  const bool dark = SharedGlobalPrefs().theme() == Escher::Theme::Dark;
  const Escher::Image* images[k_numberOfImages] = {
      dark ? ImageStore::Calculation1DarkIcon
           : ImageStore::Calculation1LightIcon,
      dark ? ImageStore::Calculation2DarkIcon
           : ImageStore::Calculation2LightIcon,
      dark ? ImageStore::Calculation3DarkIcon
           : ImageStore::Calculation3LightIcon,
      dark ? ImageStore::Calculation4DarkIcon
           : ImageStore::Calculation4LightIcon};
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
