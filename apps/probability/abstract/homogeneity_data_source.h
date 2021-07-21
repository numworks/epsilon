#ifndef APPS_PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H
#define APPS_PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H

#include <apps/i18n.h>
#include <apps/shared/parameter_text_field_delegate.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>
#include <escher/text_field_delegate.h>

#include "probability/abstract/chained_selectable_table_view_delegate.h"
#include "probability/gui/bordered_table_view_data_source.h"
#include "probability/gui/solid_color_cell.h"
#include "probability/models/statistic/homogeneity_statistic.h"

using namespace Escher;

namespace Probability {

/* This class wraps a TableViewDataSource by adding a Row & Column header around it.
 * Specifically meant for InputHomogeneity and HomogeneityResults. */
// TODO memoize
class HomogeneityTableDataSource : public BorderedTableViewDataSource,
                                   public SelectableTableViewDataSource,
                                   public ChainedSelectableTableViewDelegate {
public:
  HomogeneityTableDataSource(TableViewDataSource * contentTable,
                             Escher::SelectableTableViewDelegate * tableDelegate,
                             I18n::Message headerPrefix = I18n::Message::Group);
  int numberOfRows() const override { return m_contentTable->numberOfRows() + 1; }
  int numberOfColumns() const override { return m_contentTable->numberOfColumns() + 1; }
  int reusableCellCount(int type) override {
    return type == 0 ? m_contentTable->reusableCellCount(0) + numberOfColumns() + numberOfRows() - 1
                     : m_contentTable->reusableCellCount(type);
  }
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

  constexpr static int k_initialNumberOfRows = 4;
  constexpr static int k_initialNumberOfColumns = 3;
  constexpr static int k_maxNumberOfColumns = HomogeneityStatistic::k_maxNumberOfColumns;
  constexpr static int k_maxNumberOfRows = HomogeneityStatistic::k_maxNumberOfRows;
  constexpr static int k_numberOfReusableRows = (Ion::Display::Height -
                                                 Escher::Metric::TitleBarHeight -
                                                 Escher::Metric::StackTitleHeight) /
                                                    HomogeneityTableDataSource::k_rowHeight +
                                                1;
  constexpr static int k_numberOfReusableColumns = Ion::Display::Width /
                                                       HomogeneityTableDataSource::k_columnWidth +
                                                   1;
  constexpr static int k_numberOfReusableCells = k_numberOfReusableRows * k_numberOfReusableColumns;

protected:
  TableViewDataSource * m_contentTable;

private:
  constexpr static int k_typeOfRowHeader = 17;
  constexpr static int k_typeOfColumnHeader = 18;
  constexpr static int k_typeOfTopLeftCell = 19;

  I18n::Message m_headerPrefix;

  SolidColorCell m_topLeftCell;
  EvenOddBufferTextCell m_rowHeader[k_maxNumberOfRows];
  EvenOddBufferTextCell m_colHeader[k_maxNumberOfColumns];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H */
