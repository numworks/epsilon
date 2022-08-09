#include "calculation_popup_data_source.h"

#include "../images/calculation1_icon.h"
#include "../images/calculation2_icon.h"
#include "../images/calculation3_icon.h"
#include "../images/calculation4_icon.h"

namespace Distributions {

CalculationPopupDataSource::CalculationPopupDataSource(Distribution * distribution) :
  m_distribution(distribution)
{
  /* We can init the image of all cells because there is a bijection of the
   * displayable cells and the source cells. */
  const Escher::Image * images[k_numberOfImages] = {ImageStore::Calculation1Icon,
                                                    ImageStore::Calculation2Icon,
                                                    ImageStore::Calculation3Icon,
                                                    ImageStore::Calculation4Icon};
  for (int i = 0; i < k_numberOfImages; i++) {
    m_imageCells[i].setImage(images[i]);
  }
}

int CalculationPopupDataSource::numberOfRows() const {
  return k_numberOfImages - m_distribution->isContinuous();
}

}
