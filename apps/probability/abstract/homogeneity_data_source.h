#ifndef PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H

#include <apps/i18n.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>

#include "probability/abstract/chained_selectable_table_view_delegate.h"
#include "probability/abstract/dynamic_cells_data_source.h"
#include "probability/gui/bordered_table_view_data_source.h"
#include "probability/gui/solid_color_cell.h"
#include "probability/models/statistic/homogeneity_test.h"

using namespace Escher;

namespace Probability {

/* This class wraps a TableViewDataSource by adding a Row & Column header around it.
 * Specifically meant for InputHomogeneity and HomogeneityResults. */
class HomogeneityTableDataSource : public BorderedTableViewDataSource, public DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>, public SelectableTableViewDataSource, public ChainedSelectableTableViewDelegate {
public:
  HomogeneityTableDataSource(Escher::SelectableTableViewDelegate * tableDelegate, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> * dynamicDataSourceDelegate);
  int numberOfRows() const override { return innerNumberOfRows() + 1; }
  int numberOfColumns() const override { return innerNumberOfColumns() + 1; }
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  HighlightCell * reusableCell(int i, int type) override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

  KDCoordinate columnWidth(int i) override { return k_columnWidth; }
  KDCoordinate verticalBorderWidth() override { return k_borderBetweenColumns; }
  KDCoordinate rowHeight(int j) override { return k_rowHeight; }

  void tableViewDidChangeSelection(SelectableTableView * t,
                                   int previousSelectedCellX,
                                   int previousSelectedCellY,
                                   bool withinTemporarySelection = false) override;

  constexpr static int k_columnWidth = 80;
  constexpr static int k_borderBetweenColumns = 1;
  constexpr static int k_rowHeight = 20;

  constexpr static int k_maxNumberOfColumns = HomogeneityTest::k_maxNumberOfColumns;
  constexpr static int k_maxNumberOfRows = HomogeneityTest::k_maxNumberOfRows;
  constexpr static int k_numberOfReusableRows = (Ion::Display::Height -
                                                 Escher::Metric::TitleBarHeight -
                                                 Escher::Metric::StackTitleHeight) /
                                                    HomogeneityTableDataSource::k_rowHeight +
                                                2;
  constexpr static int k_numberOfReusableColumns = Ion::Display::Width /
                                                       HomogeneityTableDataSource::k_columnWidth +
                                                   2;
  constexpr static int k_numberOfReusableCells = k_numberOfReusableRows * k_numberOfReusableColumns;

protected:
  virtual int innerNumberOfRows() const = 0;
  virtual int innerNumberOfColumns() const = 0;
  virtual void willDisplayInnerCellAtLocation(Escher::HighlightCell * cell, int column, int row) = 0;
  virtual Escher::HighlightCell * innerCell(int i) = 0;

private:
  constexpr static int k_typeInnerCells = 17;
  constexpr static int k_typeOfHeaderCells = 18;
  constexpr static int k_typeOfTopLeftCell = 19;
  constexpr static int k_headerTranslationBuffer = 20;

  I18n::Message m_headerPrefix;
  SolidColorCell m_topLeftCell;
};

}  // namespace Probability

#endif /* PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H */
