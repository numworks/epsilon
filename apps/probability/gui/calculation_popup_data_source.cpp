#include "calculation_popup_data_source.h"

#include "../images/calculation1_icon.h"
#include "../images/calculation2_icon.h"
#include "../images/calculation3_icon.h"
#include "../images/calculation4_icon.h"

namespace Probability {

CalculationPopupDataSource::CalculationPopupDataSource(Distribution * distribution) :
  m_distribution(distribution)
{
  // Initialize width to default
  m_imageCells[0].setImage(ImageStore::Calculation1Icon);
}

int CalculationPopupDataSource::numberOfRows() const {
  return k_numberOfImages - m_distribution->isContinuous();
}

void CalculationPopupDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  HighlightImageCell * myCell = static_cast<HighlightImageCell *>(cell);
  const Escher::Image * images[k_numberOfImages] = {ImageStore::Calculation1Icon,
                                                    ImageStore::Calculation2Icon,
                                                    ImageStore::Calculation3Icon,
                                                    ImageStore::Calculation4Icon};
  myCell->setImage(images[index]);
  myCell->setHighlighted(myCell->isHighlighted());
}

}  // namespace Probability
