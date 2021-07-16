#include "calculation_popup_data_source.h"

#include "../images/calcul1_icon.h"
#include "../images/calcul2_icon.h"
#include "../images/calcul3_icon.h"
#include "../images/calcul4_icon.h"

namespace Probability {

int CalculationPopupDataSource::numberOfRows() const {
  return k_numberOfImages - m_distribution->isContinuous();
}

void CalculationPopupDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  HighlightImageCell * myCell = static_cast<HighlightImageCell *>(cell);
  const Escher::Image * images[k_numberOfImages] = {ImageStore::Calcul1Icon,
                                                    ImageStore::Calcul2Icon,
                                                    ImageStore::Calcul3Icon,
                                                    ImageStore::Calcul4Icon};
  myCell->setImage(images[index]);
  myCell->setHighlighted(myCell->isHighlighted());
}

}  // namespace Probability
