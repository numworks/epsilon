#include <escher/simple_list_view_data_source.h>
#include <assert.h>

namespace Escher {

SimpleListViewDataSource::SimpleListViewDataSource() :
  m_memoizationLockedLevel(0)
{
  resetMemoization(true);
}

KDCoordinate SimpleListViewDataSource::rowHeight(int j) {
  if (j < m_memoizedIndexOffset || j >= m_memoizedIndexOffset + k_memoizedCellsCount) {
    // Update memoization index if new cell is called.
    setMemoizationIndex(j);
  }
  if (j >= m_memoizedIndexOffset && j < m_memoizedIndexOffset + k_memoizedCellsCount) {
    // Value might be memoized
    KDCoordinate memoizedRowHeight = m_memoizedCellHeight[getMemoizedIndex(j)];
    if (memoizedRowHeight == k_resetedMemoizedValue) {
      // Compute and memoize value
      memoizedRowHeight = nonMemoizedRowHeight(j);
      m_memoizedCellHeight[getMemoizedIndex(j)] = memoizedRowHeight;
    } else {
      assert(memoizedRowHeight == nonMemoizedRowHeight(j)); // TODO Hugo : Remove this very costly assert
    }
    return memoizedRowHeight;
  }
  /* Requested row is neither memoized, nor adjacent (unless list is under lock)
   * If such a configuration where to happen, memoization shift pattern should
   * be updated. */
  assert(m_memoizationLockedLevel > 0);
  return nonMemoizedRowHeight(j);
}

KDCoordinate SimpleListViewDataSource::cumulatedHeightFromIndex(int j) {
  if (j == numberOfRows() && m_memoizedTotalHeight != k_resetedMemoizedValue) {
    // Total Height is memoized to save time and preserve memoization.
    assert(m_memoizedTotalHeight == nonMemoizedCumulatedHeightFromIndex(j)); // TODO Hugo : Remove this very costly assert
    return m_memoizedTotalHeight;
  }
  /* Take advantage of m_memoizedCumulatedHeightOffset if j is closer to
   * m_memoizedIndexOffset than 0. */
  if (j >= m_memoizedIndexOffset / 2) {
    if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue) {
      // Memoized cumulatedHeight is required and must be recomputed
      m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
    } else {
      assert(m_memoizedCumulatedHeightOffset == nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset)); // TODO Hugo : Remove this very costly assert
    }
    // Search cumulatedHeight around m_memoizedCumulatedHeightOffset
    KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
    if (j >= m_memoizedIndexOffset) {
      for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
        if (i == j) {
          assert(nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight); // TODO Hugo : Remove this very costly assert
          return cumulatedHeight;
        }
        cumulatedHeight += rowHeight(i);
      }
      assert(nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight); // TODO Hugo : Remove this very costly assert
      assert(j == numberOfRows());
      // Update memoized total height
      m_memoizedTotalHeight = cumulatedHeight;
      return cumulatedHeight;
    } else {
      for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
        cumulatedHeight -= rowHeight(i);
        if (i == j) {
          assert(nonMemoizedCumulatedHeightFromIndex(j) == cumulatedHeight); // TODO Hugo : Remove this very costly assert
          return cumulatedHeight;
        }
      }
      assert(nonMemoizedCumulatedHeightFromIndex(j) == KDCoordinate(0)); // TODO Hugo : Remove this very costly assert
      assert(cumulatedHeight == 0 && j == 0);
      return KDCoordinate(0);
    }
  }
  return nonMemoizedCumulatedHeightFromIndex(j);
}

int SimpleListViewDataSource::indexFromCumulatedHeight(KDCoordinate offsetY) {
  /* Take advantage of m_memoizedCumulatedHeightOffset if offsetY is closer to
   * m_memoizedCumulatedHeightOffset than 0. */
  if (offsetY >= m_memoizedCumulatedHeightOffset / 2) {
    if (m_memoizedCumulatedHeightOffset == k_resetedMemoizedValue) {
      // Memoized cumulatedHeight is required and must be recomputed
      m_memoizedCumulatedHeightOffset = nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset);
    } else {
      assert(m_memoizedCumulatedHeightOffset == nonMemoizedCumulatedHeightFromIndex(m_memoizedIndexOffset)); // TODO Hugo : Remove this very costly assert
    }
    // Search index around m_memoizedIndexOffset
    KDCoordinate cumulatedHeight = m_memoizedCumulatedHeightOffset;
    if (offsetY > m_memoizedCumulatedHeightOffset) {
      for (int i = m_memoizedIndexOffset; i < numberOfRows(); i++) {
        cumulatedHeight += rowHeight(i);
        if (offsetY <= cumulatedHeight) {
          assert(nonMemoizedIndexFromCumulatedHeight(offsetY) == i); // TODO Hugo : Remove this very costly assert
          return i;
        }
      }
      assert(nonMemoizedIndexFromCumulatedHeight(offsetY) == numberOfRows()); // TODO Hugo : Remove this very costly assert
      return numberOfRows();
    } else {
      for (int i = m_memoizedIndexOffset - 1; i >= 0; i--) {
        cumulatedHeight -= rowHeight(i);
        if (offsetY > cumulatedHeight) {
          assert(nonMemoizedIndexFromCumulatedHeight(offsetY) == i); // TODO Hugo : Remove this very costly assert
          return i;
        }
      }
      assert(nonMemoizedIndexFromCumulatedHeight(offsetY) == 0); // TODO Hugo : Remove this very costly assert
      assert(cumulatedHeight == 0);
      return 0;
    }
  }
  return nonMemoizedIndexFromCumulatedHeight(offsetY);
}

void SimpleListViewDataSource::prepareCellForHeightCalculation(HighlightCell * cell, int index) {
  // A non-null implementation of cellWidth is required to compute cell height.
  assert(cellWidth() != 0);
  // Set cell's frame width
  cell->setSize(KDSize(cellWidth(), cell->bounds().height()));
  // Setup cell as if it was to be displayed
  willDisplayCellForIndex(cell, index);
}

void SimpleListViewDataSource::resetMemoization(bool force) {
  if (m_memoizationLockedLevel > 0) {
    /* Prevent any memoization reset. List should not be locked if called from
     * outside (with force). */
    assert(!force);
    return;
  }
  // Reset memoized index and corresponding cumulated height.
  m_memoizedIndexOffset = 0;
  m_memoizedCumulatedHeightOffset = 0;
  if (force) {
    // Preserve total height
    m_memoizedTotalHeight = k_resetedMemoizedValue;
  }
  // Reset cell heights
  for (int i = 0; i < k_memoizedCellsCount; i++) {
    m_memoizedCellHeight[i] = k_resetedMemoizedValue;
  }
}

void SimpleListViewDataSource::resetMemoizationForIndex(int j) {
  // Memoization shouldn't be under lock
  assert(m_memoizationLockedLevel == 0);
  // reset cell height if memoized
  if (j >= m_memoizedIndexOffset && j < m_memoizedIndexOffset + k_memoizedCellsCount) {
    m_memoizedCellHeight[getMemoizedIndex(j)] = k_resetedMemoizedValue;
  } else if (j < m_memoizedIndexOffset) {
    // Cumulated Height depends on reset cell
    m_memoizedCumulatedHeightOffset = k_resetedMemoizedValue;
  }
  // Total Height depends on reset cell
  m_memoizedTotalHeight = k_resetedMemoizedValue;
}

KDCoordinate SimpleListViewDataSource::nonMemoizedRowHeight(int j) {
  /* This default implementation must be used carefully, only when all cells are
   * stored as reusable cells.
   * TODO Hugo : Find an assert that could catch that, such as :
   * Sum of every reusableCellCount for every types equals number of rows */
  assert(j < numberOfRows());
  /* Most nonMemoizedRowHeight implementations boils down to instantiating a
   * temporary cell on which calling prepareCellForHeightCalculation, and return
   * minimalSizeForOptimalDisplay. However, temporary cell must be instantiated
   * in the type expected in willDisplayCellAtIndex(), which is unknown here. */
  Escher::HighlightCell * cell = reusableCell(j, typeAtIndex(j));
  /* prepareCellForHeightCalculation will set up cell's width, which is required
   * to compute minimal height for optimal display. */
  prepareCellForHeightCalculation(cell, j);
  return cell->minimalSizeForOptimalDisplay().height();
}

int SimpleListViewDataSource::getMemoizedIndex(int index) {
  /* Values are memoized in a circular way : m_memoizedCellHeight[i] only
   * stores values for index such that index%k_memoizedCellsCount == i
   * Eg : 0 1 2 3 4 5 6 shifts to 7 1 2 3 4 5 6 which shifts to 7 8 2 3 4 5 6
   * Shifting the memoization is then more optimized. */
  assert(index >= m_memoizedIndexOffset && index < m_memoizedIndexOffset + k_memoizedCellsCount);
  int circularOffset = m_memoizedIndexOffset % k_memoizedCellsCount;
  return (circularOffset + index - m_memoizedIndexOffset) % k_memoizedCellsCount;
}

void SimpleListViewDataSource::setMemoizationIndex(int index) {
  if (m_memoizationLockedLevel > 0 || index == m_memoizedIndexOffset) {
    return;
  }

  if (index < m_memoizedIndexOffset / 2 && m_memoizedIndexOffset - index > k_memoizedCellsCount) {
    resetMemoization(false);
    m_memoizedIndexOffset = index;
  }
  bool newCellIsUnder = m_memoizedIndexOffset > index;
  int indexDelta = newCellIsUnder ? m_memoizedIndexOffset - index : index - m_memoizedIndexOffset;
  for (int i = 0; i < indexDelta; ++i) {
    shiftMemoization(newCellIsUnder);
  }
}

void SimpleListViewDataSource::shiftMemoization(bool newCellIsUnder) {
  // Only a limited number of cells' height are memoized.
  if (m_memoizationLockedLevel > 0) {
    // Memoization is locked, do not shift
    return;
  }
  // The new unknown value must be reseted.
  if (newCellIsUnder) {
    assert(m_memoizedIndexOffset > 0);
    m_memoizedIndexOffset--;
    // Unknown value is the new one
    KDCoordinate height = nonMemoizedRowHeight(m_memoizedIndexOffset);
    // Compute and set memoized index height
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = height;
    // Update cumulated height
    if (m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue) {
      m_memoizedCumulatedHeightOffset-= height;
      assert(m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue);
    }
  } else {
    assert(m_memoizedIndexOffset <= numberOfRows() - k_memoizedCellsCount);
    if (m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue) {
      m_memoizedCumulatedHeightOffset+=rowHeight(m_memoizedIndexOffset);
      assert(m_memoizedCumulatedHeightOffset != k_resetedMemoizedValue);
    }
    // Unknown value is the previous one
    m_memoizedCellHeight[getMemoizedIndex(m_memoizedIndexOffset)] = k_resetedMemoizedValue;
    m_memoizedIndexOffset++;
  }
}

bool SimpleListViewDataSource::updateMemoizationLock(bool lockUp) {
  /* This lock system is mainly used during development, to avoid infinite loops
   * an volatile memoized values when asserting that a memoized height was
   * correct, which heavily slows down navigation. TODO Hugo : Remove it. */
  m_memoizationLockedLevel += (lockUp ? 1 : -1);
  assert(m_memoizationLockedLevel >= 0);
  return m_memoizationLockedLevel >= 0;
}

}
