#include <escher/simple_list_view_data_source.h>
#include <assert.h>
// #include <iostream>

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
  if (j == m_memoizedIndexOffset - 1) {
    shiftMemoization(true);
  } else if (j == m_memoizedIndexOffset + k_memoizedCellsCount) {
    shiftMemoization(false);
  } else if (j == 0 && m_memoizedIndexOffset != 0) {
    resetMemoization(false);
  }
  if (j >= m_memoizedIndexOffset && j < m_memoizedIndexOffset + k_memoizedCellsCount) {
    // Value might is memoized
    KDCoordinate memoizedRowHeight = m_memoizedCellHeight[getMemoizedIndex(j)];
    if (memoizedRowHeight == k_resetedMemoizedValue) {
      // memoize value
      memoizedRowHeight = nonMemoizedRowHeight(j);
      // std::cout << "|";
      m_memoizedCellHeight[getMemoizedIndex(j)] = memoizedRowHeight;
    // } else {
      // std::cout << "_";
    }
    return memoizedRowHeight;
  }
  // std::cout << "\n" << m_memoizedIndexOffset << " --> " << j << "\n";
  return nonMemoizedRowHeight(j);
}

KDCoordinate SimpleListViewDataSource::cumulatedHeightFromIndex(int j) {
  if (j == numberOfRows() && m_memoizedTotalHeight != k_resetedMemoizedValue) {
    // Return a memoizedTotal Height to save computation time
    return m_memoizedTotalHeight;
  }
  // Recompute the memoized cumulatedHeight if needed
  if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue && j > m_memoizedIndexOffset / 2) {
    m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
  }
  // If advantageous, compute cumulated height from memoized one
  KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
  if (j >= m_memoizedIndexOffset) {
    for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
      if (i == j) {
        // assert(nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight);
        return cumulatedHeight;
      }
      cumulatedHeight += rowHeight(i);
    }
    // assert(nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight);
    assert(j == numberOfRows());
    m_memoizedTotalHeight = cumulatedHeight;
    return cumulatedHeight;
  } else if (j > m_memoizedIndexOffset / 2) {
    for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
      cumulatedHeight -= rowHeight(i);
      if (i == j) {
        // assert(nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight);
        return cumulatedHeight;
      }
    }
    // assert(nonMemoizedCumulatedHeightFromIndex(j) == 0);
    return KDCoordinate(0);
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
  if (offsetY > m_memoizedCumulatedHeightOffset) {
    for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
      cumulatedHeight += rowHeight(i);
      if (offsetY <= cumulatedHeight) {
        // assert(nonMemoizedIndexFromCumulatedHeight(offsetY) == i);
        return i;
      }
    }
    // assert(nonMemoizedIndexFromCumulatedHeight(offsetY) == numberOfRows());
    return numberOfRows();
  } else if (offsetY > m_memoizedCumulatedHeightOffset / 2) {
    for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
      cumulatedHeight -= rowHeight(i);
      if (offsetY > cumulatedHeight) {
        // assert(nonMemoizedIndexFromCumulatedHeight(offsetY) == i);
        return i;
      }
    }
    // assert(nonMemoizedIndexFromCumulatedHeight(offsetY) == 0);
    return 0;
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
    // std::cout << " < ";
    m_memoizedIndexOffset--;
    // Unknown value is the new one
    KDCoordinate height = nonMemoizedRowHeight(m_memoizedIndexOffset);
    // Compute and set memoized index height
    // std::cout << "|";
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = height;
    // Update cumulated height
    m_memoizedCumulatedHeightOffset-= height;
  } else {
    // std::cout << " > ";
    assert(m_memoizedIndexOffset <= numberOfRows() - k_memoizedCellsCount);
    m_memoizedCumulatedHeightOffset+=rowHeight(m_memoizedIndexOffset);
    // Unknown value is the previous one
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = k_resetedMemoizedValue;
    m_memoizedIndexOffset++;
  }
}

void SimpleListViewDataSource::resetMemoization(bool force) {
  // std::cout << "\n0\n";
  m_memoizedIndexOffset = 0;
  m_memoizedCumulatedHeightOffset = k_resetedMemoizedValue;
  if (force) {
    // No need to always reset total height
    m_memoizedTotalHeight = k_resetedMemoizedValue;
  }
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    m_memoizedCellHeight[i] = k_resetedMemoizedValue;
  }
}

void SimpleListViewDataSource::resetMemoizationForIndex(int j) {
  if (j >= m_memoizedIndexOffset && j < m_memoizedIndexOffset + k_memoizedCellsCount) {
    m_memoizedCellHeight[getMemoizedIndex(j)] = k_resetedMemoizedValue;
  }
}

}
