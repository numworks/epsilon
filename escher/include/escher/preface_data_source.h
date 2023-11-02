#ifndef ESCHER_PREFACE_DATA_SOURCE_H
#define ESCHER_PREFACE_DATA_SOURCE_H

#include <escher/selectable_table_view.h>

namespace Escher {

class IntermediaryDataSource : public TableViewDataSource,
                               public ScrollViewDataSource {
 public:
  IntermediaryDataSource(TableViewDataSource* mainDataSource)
      : m_mainDataSource(mainDataSource) {}

  // TableViewDataSource
  int numberOfRows() const override { return m_mainDataSource->numberOfRows(); }
  int numberOfColumns() const override {
    return m_mainDataSource->numberOfColumns();
  }
  void fillCellForLocation(HighlightCell* cell, int column, int row) override {
    m_mainDataSource->fillCellForLocation(cell, columnInMainDataSource(column),
                                          rowInMainDataSource(row));
  }
  HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override {
    return m_mainDataSource->reusableCellCount(type);
  }
  int typeAtLocation(int column, int row) const override {
    return m_mainDataSource->typeAtLocation(columnInMainDataSource(column),
                                            rowInMainDataSource(row));
  }
  KDCoordinate separatorBeforeColumn(int column) override {
    return m_mainDataSource->separatorBeforeColumn(
        columnInMainDataSource(column));
  }
  KDCoordinate separatorBeforeRow(int row) override {
    return m_mainDataSource->separatorBeforeRow(rowInMainDataSource(row));
  }

 protected:
  KDCoordinate nonMemoizedColumnWidth(int column) override final;
  KDCoordinate nonMemoizedRowHeight(int row) override final;

  KDCoordinate nonMemoizedCumulatedWidthBeforeColumn(int column) override;
  KDCoordinate nonMemoizedCumulatedHeightBeforeRow(int row) override;

  /* WARNING: This method works only if columnInMainDataSource(i) == i.
   * Else, it should be overriden.*/
  int nonMemoizedColumnAfterCumulatedWidth(KDCoordinate offsetX) override;
  /* WARNING: This method works only if rowInMainDataSource(j) == j.
   * Else, it should be overriden.*/
  int nonMemoizedRowAfterCumulatedHeight(KDCoordinate offsetY) override;

  virtual int columnInMainDataSource(int i) const { return i; }
  virtual int rowInMainDataSource(int j) const { return j; }

  TableViewDataSource* m_mainDataSource;
};

class RowPrefaceDataSource : public IntermediaryDataSource {
 public:
  RowPrefaceDataSource(int prefaceRow, TableViewDataSource* mainDataSource)
      : IntermediaryDataSource(mainDataSource),
        m_prefaceRow(prefaceRow),
        m_rowHeigthManager(this) {}

  KDCoordinate cumulatedHeightAtPrefaceRow(bool after) const;
  int numberOfRows() const override { return 1; }
  KDCoordinate separatorBeforeRow(int row) override { return 0; }
  KDCoordinate separatorAfterPrefaceRow() {
    assert(m_prefaceRow >= 0);
    return m_mainDataSource->separatorBeforeRow(m_prefaceRow + 1);
  }

 private:
  KDCoordinate nonMemoizedCumulatedHeightBeforeRow(int row) override;
  int nonMemoizedRowAfterCumulatedHeight(KDCoordinate offsetY) override;

  int rowInMainDataSource(int j) const override {
    assert(j == 0 || j == 1);
    assert(m_prefaceRow >= 0);
    return m_prefaceRow + j;
  }

  TableSize1DManager* rowHeightManager() override {
    return &m_rowHeigthManager;
  }

  const int m_prefaceRow;
  MemoizedRowHeightManager<1> m_rowHeigthManager;
};

class ColumnPrefaceDataSource : public IntermediaryDataSource {
 public:
  ColumnPrefaceDataSource(int prefaceColumn,
                          TableViewDataSource* mainDataSource)
      : IntermediaryDataSource(mainDataSource),
        m_prefaceColumn(prefaceColumn) {}

  void setPrefaceColumn(int column) { m_prefaceColumn = column; }
  KDCoordinate cumulatedWidthAtPrefaceColumn(bool after) const;

  int numberOfColumns() const override { return 1; }
  KDCoordinate separatorBeforeColumn(int column) override { return 0; }
  KDCoordinate separatorAfterPrefaceColumn() {
    assert(m_prefaceColumn >= 0);
    return m_mainDataSource->separatorBeforeColumn(m_prefaceColumn + 1);
  }

 private:
  HighlightCell* reusableCell(int index, int type) override;
  KDCoordinate nonMemoizedCumulatedWidthBeforeColumn(int column) override;
  int nonMemoizedColumnAfterCumulatedWidth(KDCoordinate offsetX) override;

  int columnInMainDataSource(int i) const override {
    assert(i == 0 || i == 1);
    assert(m_prefaceColumn >= 0);
    return m_prefaceColumn + i;
  }

  int m_prefaceColumn;
};

class IntersectionPrefaceDataSource : public RowPrefaceDataSource {
 public:
  /* The implementation of this class (1 row and 1 column) is a hack to avoid
   * the diamond problem. Indeed, IntersectionPrefaceDataSource should inherit
   * from RowPrefaceDataSource (1 row) and ColumnPrefaceDataSource (1 column).
   * The hack we chose is to inherit from RowPrefaceDataSource and take a
   * ColumnPrefaceDataSource as m_mainDataSource. WARNING : we choose to
   * inherit from RowPrefaceDataSource because we don't need (current use in
   * Epsilon) to setPrefaceRow while with ColumnPrefaceDataSource we need to
   * setPrefaceColumn. Here, intersection will take directly the first cell of
   * ColumnPrefaceDataSource, with the right prefaceColumn. */
  IntersectionPrefaceDataSource(int prefaceRow,
                                ColumnPrefaceDataSource* columnDataSource)
      : RowPrefaceDataSource(prefaceRow, columnDataSource) {}
};

}  // namespace Escher

#endif
