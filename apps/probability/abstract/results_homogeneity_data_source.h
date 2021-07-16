#ifndef APPS_PROBABILITY_ABSTRACT_RESULTS_HOMOGENEITY_DATA_SOURCE_H
#define APPS_PROBABILITY_ABSTRACT_RESULTS_HOMOGENEITY_DATA_SOURCE_H

#include <escher/table_view_data_source.h>
#include "homogeneity_data_source.h"

namespace Probability {


class ResultsHomogeneityDataSource : public Escher::TableViewDataSource {
public:
  ResultsHomogeneityDataSource();
  int numberOfRows() const override { return HomogeneityTableDataSource::k_initialNumberOfRows; }
  int numberOfColumns() const override {
    return HomogeneityTableDataSource::k_initialNumberOfColumns;
  }
  KDCoordinate columnWidth(int i) override { return HomogeneityTableDataSource::k_columnWidth; }
  KDCoordinate rowHeight(int j) override { return HomogeneityTableDataSource::k_rowHeight; }
  int typeAtLocation(int i, int j) override { return 0; }
  HighlightCell * reusableCell(int i, int type) override;
  int reusableCellCount(int type) override { return numberOfRows() * numberOfColumns(); };

private:
  EvenOddBufferTextCell m_cells[HomogeneityTableDataSource::k_numberOfReusableCells];
};


} // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_RESULTS_HOMOGENEITY_DATA_SOURCE_H */
