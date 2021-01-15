#include <escher/simple_list_view_data_source.h>
#include <assert.h>

namespace Escher {

SimpleListViewDataSource::SimpleListViewDataSource() {
  resetMemoization();
}

void SimpleListViewDataSource::prepareCellForHeightCalculation(HighlightCell * cell, int index) {
  // A non-null implementation of cellWidth is required to compute cell height.
  assert(cellWidth() != 0);
  // Set cell's frame width
  cell->setSize(KDSize(cellWidth(), cell->bounds().height()));
  // Setup cell as if it was to be displayed
  willDisplayCellForIndex(cell, index);
}

KDCoordinate SimpleListViewDataSource::rowHeight(int j) {
  // if (j < m_memoizedIndexOffset || j >= m_memoizedIndexOffset + k_memoizedCellsCount) {
    // setMemoizedOffset(min(numberOfRows() - k_memoizedCellsCount, max(0, j))); TODO : ! infinite loops
  // }
  if (j >= m_memoizedIndexOffset && j < m_memoizedIndexOffset + k_memoizedCellsCount) {
    // Value might is memoized
    KDCoordinate memoizedRowHeight = m_memoizedCellHeight[getMemoizedIndex(j)];
    if (memoizedRowHeight == k_resetedMemoizedValue) {
      // memoize value
      memoizedRowHeight = nonMemoizedRowHeight(j);
      m_memoizedCellHeight[getMemoizedIndex(j)] = memoizedRowHeight;
    }
    return memoizedRowHeight;
  }
  // TODO Hugo : Consider updating m_memoizedIndexOffset
  return nonMemoizedRowHeight(j);
}

KDCoordinate SimpleListViewDataSource::cumulatedHeightFromIndex(int j) {
  // Recompute the memoized cumulatedHeight if needed
  if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue && j > m_memoizedIndexOffset / 2) {
    m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
  }
  // If advantageous, compute cumulated height from memoized one
  KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
  if (j >= m_memoizedIndexOffset) {
    for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
      if (i == j) {
        return cumulatedHeight;
      }
      cumulatedHeight += rowHeight(i);
    }
    assert(false);
  } else if (j > m_memoizedIndexOffset / 2) {
    for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
      cumulatedHeight -= rowHeight(i);
      if (i == j) {
        return cumulatedHeight;
      }
    }
    assert(false);
  }
  return nonMemoizedCumulatedHeightFromIndex(j);
}

int SimpleListViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  // Recompute the memoized cumulatedHeight if needed
  if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue) {
    m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
  }
  // If advantageous, search index around m_memoizedIndexOffset
  KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
  if (offsetY >= m_memoizedCumulatedHeightOffset) {
    for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
      cumulatedHeight += rowHeight(i);
      if (offsetY < cumulatedHeight) {
        return i;
      }
    }
    assert(false);
  } else if (offsetY > m_memoizedCumulatedHeightOffset / 2) {
    for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
      cumulatedHeight -= rowHeight(i);
      if (offsetY >= cumulatedHeight) {
        return i;
      }
    }
    assert(false);
  }
  return nonMemoizedIndexFromCumulatedHeight(offsetY);
}


KDCoordinate SimpleListViewDataSource::nonMemoizedRowHeight(int j) {
  // Must be overridden if not all cells are stored as reusable.
  assert(j < numberOfRows());
  assert(numberOfColumns() == 1);
  assert(reusableCellCount(typeAtIndex(j)) == numberOfRows());
  // TODO Hugo : If possible, instantiate a temporary cell here (! type)
  Escher::HighlightCell * cell = reusableCell(j, typeAtIndex(j));
  prepareCellForHeightCalculation(cell, j);
  return cell->minimalSizeForOptimalDisplay().height();
}

int SimpleListViewDataSource::getMemoizedIndex(int index) {
  // Values are memoized in a circular way :  m_memoizedCellHeight[i] only
  // stores values for index such that index%k_memoizedCellsCount == i
  // It is then much simpler to shift the memoization.
  assert(index >= m_memoizedIndexOffset && index < m_memoizedIndexOffset + k_memoizedCellsCount);
  int circularShift = m_memoizedIndexOffset % k_memoizedCellsCount;
  return (index - m_memoizedIndexOffset + circularShift) % k_memoizedCellsCount;
}

// int SimpleListViewDataSource::setMemoizedOffset(int offset) {
  // if (offset > m_memoizedIndexOffset) {
  //   if () {
  // !!! Loops
  //   }
  // }
  // if (offset - m_memoizedIndexOffset >= 7 || m_memoizedIndexOffset - offset >= 7) {
  //   KDCoordinate cumulatedHeightOffset = cumulatedHeightFromIndex
  //   resetMemoization()
  // }
// }

void SimpleListViewDataSource::shiftMemoization(bool newCellIsUnder) {
  // The new unknown value must be reseted.
  if (newCellIsUnder) {
    assert(m_memoizedIndexOffset > 0);
    m_memoizedIndexOffset--;
    // Unknown value is the new one
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = k_resetedMemoizedValue;
    m_memoizedCumulatedHeightOffset-=rowHeight(m_memoizedIndexOffset);
  } else {
    assert(m_memoizedIndexOffset <= numberOfRows() - k_memoizedCellsCount);
    m_memoizedCumulatedHeightOffset+=rowHeight(m_memoizedIndexOffset);
    // Unknown value is the previous one
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = k_resetedMemoizedValue;
    m_memoizedIndexOffset++;
  }
}

void SimpleListViewDataSource::resetMemoization() {
  m_memoizedIndexOffset = 0;
  m_memoizedCumulatedHeightOffset = k_resetedMemoizedValue;
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    m_memoizedCellHeight[i] = k_resetedMemoizedValue;
  }
}

}
