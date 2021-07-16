#include "results_homogeneity_data_source.h"

namespace Probability {

ResultsHomogeneityDataSource::ResultsHomogeneityDataSource() {
  for (int i = 0; i < HomogeneityTableDataSource::k_numberOfReusableCells; i++) {
    m_cells[i].setFont(KDFont::SmallFont);
    m_cells[i].setText("1");
  }
}

HighlightCell * ResultsHomogeneityDataSource::reusableCell(int i, int type) {
  return &m_cells[i];
}

}  // namespace Probability
