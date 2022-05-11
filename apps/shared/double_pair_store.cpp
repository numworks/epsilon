#include "double_pair_store.h"
#include "poincare_helpers.h"
#include <apps/apps_container.h>
#include <poincare/helpers.h>
#include <poincare/float.h>
#include <ion/storage/file_system.h>
#include <cmath>
#include <assert.h>
#include <stddef.h>
#include <ion.h>
#include <algorithm>

using namespace Poincare;
using namespace Ion::Storage;

namespace Shared {

DoublePairStore::DoublePairStore() :
  m_validSeries{false,false,false},
  m_updateFlag(true)
{
  for (int s = 0; s < k_numberOfSeries; s++) {
    for (int i = 0; i < k_numberOfColumnsPerSeries; i++) {
      m_dataLists[s][i] = List::Builder();
    }
  }
}

void DoublePairStore::initListsFromStorage() {
  char listName[k_columnNamesLength + 1];
  // Prevent update to avoid storing empty lists
  preventUpdate();
  for (int s = 0; s < k_numberOfSeries; s++) {
    for (int i = 0; i < k_numberOfColumnsPerSeries; i++) {
      // Get the data of X1, Y1, X2, Y2, V1, V2, etc. from storage
      fillColumnName(s, i, listName);
      Record r = Record(listName, lisExtension);
      Record::Data listData = r.value();
      if (listData.size == 0) {
        continue;
      }
      Expression e = Expression::ExpressionFromAddress(listData.buffer, listData.size);
      if (e.type() != ExpressionNode::Type::List) {
        continue;
      }
      setList(static_cast<List &>(e), s, i, i < k_numberOfColumnsPerSeries - 1 ? false : true);
    }
    memoizeValidSeries(s);
  }
  enableUpdate();
}

int DoublePairStore::fillColumnName(int series, int columnIndex, char * buffer) {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(columnIndex >= 0 && columnIndex < k_numberOfColumnsPerSeries);
  buffer[0] = columnNamePrefixAtIndex(columnIndex);
  buffer[1] = static_cast<char>('1' + series);
  buffer[2] = 0;
  return 2;
}

bool DoublePairStore::isColumnName(const char * name, int nameLen, int * returnSeries, int * returnColumn) {
  if (nameLen != 2 || name[1] < '1' || name[1] >= '1' + k_numberOfSeries) {
    return false;
  }
  for (int i = 0; i < k_numberOfColumnsPerSeries; i++) {
    if (name[0] == columnNamePrefixAtIndex(i)) {
      if (returnSeries != nullptr) {
        *returnSeries = name[1] - '1';
      }
      if (returnColumn != nullptr) {
        *returnColumn = i;
      }
      return true;
    }
  }
  return false;
}

double DoublePairStore::get(int series, int i, int j) const {
  assert(j < m_dataLists[series][i].numberOfChildren() && m_dataLists[series][i].childAtIndex(j).type() == ExpressionNode::Type::Double);
  Expression child = m_dataLists[series][i].childAtIndex(j);
  return static_cast<Float<double> &>(child).value();
}

void DoublePairStore::set(double f, int series, int i, int j) {
  assert(series >= 0 && series < k_numberOfSeries);
  if (j >= k_maxNumberOfPairs) {
    return;
  }
  assert(j <= numberOfPairsOfSeries(series));
  if (j == numberOfPairsOfSeries(series)) {
    m_dataLists[series][i].addChildAtIndexInPlace(Float<double>::Builder(f), j, j);
    int otherI = i == 0 ? 1 : 0;
    m_dataLists[series][otherI].addChildAtIndexInPlace(Float<double>::Builder(defaultValue(series, otherI, j)), j, j);
  } else {
    m_dataLists[series][i].replaceChildAtIndexInPlace(j, Float<double>::Builder(f));
  }
  updateStorageAndValidity(series);
}

void DoublePairStore::setList(List list, int series, int i, bool formatSeriesAfterwards) {
  /* Approximate the list to turn it into list of doubles since we do not
   * want to work with exact expressions in Regression and Statistics.*/
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i ==1);
  int newListLength = std::max(list.numberOfChildren(), m_dataLists[series][i].numberOfChildren());
  for (int j = 0; j < newListLength; j++) {
    if (j >= list.numberOfChildren()) {
      m_dataLists[series][i].replaceChildAtIndexInPlace(j, Float<double>::Builder(NAN));
      continue;
    }
    double evaluation = PoincareHelpers::ApproximateToScalar<double>(list.childAtIndex(j), AppsContainer::sharedAppsContainer()->globalContext());
    Expression newChild = Float<double>::Builder(evaluation);
    if (j >= m_dataLists[series][i].numberOfChildren()) {
      m_dataLists[series][i].addChildAtIndexInPlace(newChild, j, j);
      continue;
    }
    m_dataLists[series][i].replaceChildAtIndexInPlace(j, newChild);
  }
  if (formatSeriesAfterwards) {
    formatListsOfSeries(series);
  }
  updateStorageAndValidity(series);
}

void DoublePairStore::formatListsOfSeries(int series) {
  int seriesLength = std::max(m_dataLists[series][0].numberOfChildren(), m_dataLists[series][1].numberOfChildren());
  int j = 0;
  for (int n = 0; n < seriesLength; n++) {
    for (int i = 0; i < k_numberOfColumnsPerSeries; i++) {
      // Step 1: if one of the columns is shorter, add NAN at the end of it
      if (j >= m_dataLists[series][i].numberOfChildren()) {
        Expression e = Float<double>::Builder(NAN);
        m_dataLists[series][i].addChildAtIndexInPlace(e, j, j);
      }
      /* Step 2: Transform all the elements into Float<double> */
      Expression data = m_dataLists[series][i].childAtIndex(j);
      if (data.type() != ExpressionNode::Type::Double) {
        m_dataLists[series][i].replaceChildAtIndexInPlace(j, Float<double>::Builder(NAN));
      }
    }
    Expression dataX = m_dataLists[series][0].childAtIndex(j);
    Expression dataY = m_dataLists[series][1].childAtIndex(j);
    assert(dataX.type() == ExpressionNode::Type::Double && dataY.type() == ExpressionNode::Type::Double);
    /* Step 3: if both values are NAN, or if number of values exceeds
     * k_maxNumberOfPairs, remove the row. */
    if (j >= k_maxNumberOfPairs || (std::isnan(static_cast<Float<double> &>(dataX).value()) && std::isnan(static_cast<Float<double> &>(dataY).value()))) {
      m_dataLists[series][0].removeChildAtIndexInPlace(j);
      m_dataLists[series][1].removeChildAtIndexInPlace(j);
      j--;
    }
    j++;
  }

}


int DoublePairStore::numberOfPairs() const {
  int result = 0;
  for (int i = 0; i < k_numberOfSeries; i++) {
    result += numberOfPairsOfSeries(i);
  }
  return result;
}

bool DoublePairStore::deleteValueAtIndex(int series, int i, int j) {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(j >= 0 && j < numberOfPairsOfSeries(series));
  int otherI = i == 0 ? 1 : 0;
  if (std::isnan(get(series,otherI,j))) {
    deletePairOfSeriesAtIndex(series, j);
    return true;
  } else {
    set(NAN, series, i, j);
    return false;
  }
}

void DoublePairStore::deletePairOfSeriesAtIndex(int series, int j) {
  m_dataLists[series][0].removeChildAtIndexInPlace(j);
  m_dataLists[series][1].removeChildAtIndexInPlace(j);
  updateStorageAndValidity(series);
}

void DoublePairStore::deleteAllPairsOfSeries(int series) {
  assert(series >= 0 && series < k_numberOfSeries);
  preventUpdate();
  for (int i = 0 ; i < k_numberOfColumnsPerSeries ; i++) {
    deletePairOfSeriesAtIndex(series, i);
  }
  enableUpdate();
  updateStorageAndValidity(series);
}

void DoublePairStore::deleteAllPairs() {
  for (int i = 0; i < k_numberOfSeries; i ++) {
    deleteAllPairsOfSeries(i);
  }
}

void DoublePairStore::deleteColumn(int series, int i) {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i == 1);
  preventUpdate();
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    if(deleteValueAtIndex(series, i, k)) {
      k--;
    }
  }
  enableUpdate();
  updateStorageAndValidity(series);
}

void DoublePairStore::resetColumn(int series, int i) {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i == 1);
  preventUpdate();
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    set(defaultValue(series, i, k), series, i, k);
  }
  enableUpdate();
  updateStorageAndValidity(series);
}

bool DoublePairStore::hasValidSeries(ValidSeries validSeries) const {
  for (int i = 0; i < k_numberOfSeries; i++) {
    if (validSeries(this, i)) {
      return true;
    }
  }
  return false;
}

bool DoublePairStore::seriesIsValid(int series) const {
  assert(series >= 0 && series < k_numberOfSeries);
  return m_validSeries[series];
}

void DoublePairStore::updateStorageAndValidity(int series) {
  if (!m_updateFlag) {
    return;
  }
  updateSeriesValidity(series);
  storeLists(series);
}

void DoublePairStore::storeLists(int series) {
  assert(series >= 0 && series < k_numberOfSeries);
  for (int i = 0; i < k_numberOfColumnsPerSeries ; i++) {
    List listToStore = m_dataLists[series][i];
    char name[k_columnNamesLength + 1];
    int length = fillColumnName(series, i, name);
    Symbol listSymbol = Symbol::Builder(name, length);
    AppsContainer::sharedAppsContainer()->globalContext()->setExpressionForSymbolAbstract(listToStore, listSymbol);
  }
}

int DoublePairStore::numberOfValidSeries(ValidSeries validSeries) const {
  int nonEmptySeriesCount = 0;
  for (int i = 0; i< k_numberOfSeries; i++) {
    if (validSeries(this, i)) {
      nonEmptySeriesCount++;
    }
  }
  return nonEmptySeriesCount;
}

int DoublePairStore::indexOfKthValidSeries(int k, ValidSeries validSeries) const {
  assert(k >= 0 && k < numberOfValidSeries());
  int validSeriesCount = 0;
  for (int i = 0; i < k_numberOfSeries; i++) {
    if (validSeries(this, i)) {
      if (validSeriesCount == k) {
        return i;
      }
      validSeriesCount++;
    }
  }
  assert(false);
  return 0;
}

void DoublePairStore::sortColumn(int series, int column) {
  assert(column == 0 || column == 1);
  static Poincare::Helpers::Swap swapRows = [](int a, int b, void * ctx, int numberOfElements) {
    // Swap X and Y values
    void ** pack = reinterpret_cast<void **>(ctx);
    DoublePairStore * store = reinterpret_cast<DoublePairStore *>(pack[0]);
    int * series = reinterpret_cast<int *>(pack[1]);
    double dataAx = store->get(*series, 0, a);
    double dataAy = store->get(*series, 1, a);
    store->set(store->get(*series, 0, b), *series, 0, a);
    store->set(store->get(*series, 1, b), *series, 1, a);
    store->set(dataAx, *series, 0, b);
    store->set(dataAy, *series, 1, b);
  };
  static Poincare::Helpers::Compare compare = [](int a, int b, void * ctx, int numberOfElements)->bool{
    void ** pack = reinterpret_cast<void **>(ctx);
    const DoublePairStore * store = reinterpret_cast<const DoublePairStore *>(pack[0]);
    int * series = reinterpret_cast<int *>(pack[1]);
    int * column = reinterpret_cast<int *>(pack[2]);
    double dataA = store->get(*series, *column, a);
    double dataB = store->get(*series, *column, b);
    return dataA >= dataB || std::isnan(dataA);
  };
  preventUpdate();
  void * context[] = { const_cast<DoublePairStore *>(this), &series, &column };
  Poincare::Helpers::Sort(swapRows, compare, context, numberOfPairsOfSeries(series));
  enableUpdate();
  updateStorageAndValidity(series);
}

void DoublePairStore::sortIndexByColumn(uint8_t * sortedIndex, int series, int column, int startIndex, int endIndex) const {
  assert(startIndex < endIndex);
  void * pack[] = { const_cast<DoublePairStore *>(this), sortedIndex + startIndex, &series, &column };
  Poincare::Helpers::Sort(
      [](int i, int j, void * ctx, int n) { // Swap method
        void ** pack = reinterpret_cast<void **>(ctx);
        uint8_t * sortedIndex = reinterpret_cast<uint8_t *>(pack[1]);
        uint8_t t = sortedIndex[i];
        sortedIndex[i] = sortedIndex[j];
        sortedIndex[j] = t;
      },
      [](int i, int j, void * ctx, int n) { // Comparison method
        void ** pack = reinterpret_cast<void **>(ctx);
        const DoublePairStore * store = reinterpret_cast<const DoublePairStore *>(pack[0]);
        uint8_t * sortedIndex = reinterpret_cast<uint8_t *>(pack[1]);
        int series = *reinterpret_cast<int *>(pack[2]);
        int column = *reinterpret_cast<int *>(pack[3]);
        return store->get(series, column, sortedIndex[i]) >= store->get(series, column, sortedIndex[j]);
      }, pack, endIndex - startIndex);
}

double DoublePairStore::sumOfColumn(int series, int i, bool lnOfSeries) const {
  assert(series >= 0 && series < k_numberOfSeries);
  assert(i == 0 || i == 1);
  double result = 0;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    result += lnOfSeries ? log(get(series, i, k)) : get(series, i, k);
  }
  return result;
}

bool DoublePairStore::seriesNumberOfAbscissaeGreaterOrEqualTo(int series, int i) const {
  assert(series >= 0 && series < k_numberOfSeries);
  int count = 0;
  for (int j = 0; j < numberOfPairsOfSeries(series); j++) {
    if (count >= i) {
      return true;
    }
    double currentAbsissa = get(series, 0, j);
    bool firstOccurence = true;
    for (int k = 0; k < j; k++) {
      if (get(series, 0, k) == currentAbsissa) {
        firstOccurence = false;
        break;
      }
    }
    if (firstOccurence) {
      count++;
    }
  }
  return count >= i;
}

uint32_t DoublePairStore::storeChecksum() const {
  uint32_t checkSumPerSeries[k_numberOfSeries];
  for (int i = 0; i < k_numberOfSeries; i++) {
    checkSumPerSeries[i] = storeChecksumForSeries(i);
  }
  return Ion::crc32Word(checkSumPerSeries, k_numberOfSeries);
}

uint32_t DoublePairStore::storeChecksumForSeries(int series) const {
  if (numberOfPairsOfSeries(series) == 0) {
    return 0;
  }
  /* Columns of a same series should be consecutive in pool since they are
   * built consecutively in init(). So to compute the CRC32 of a series, we
   * just need to compute the CRC32 of the bytes at the adress of the first
   * column, with a length of the two column combined.
   * */
  // Assert that the two columns are consecutive in pool.
   assert((char *)(m_dataLists[series][0].addressInPool()) + m_dataLists[series][0].size() / sizeof(char) == (char *)m_dataLists[series][1].addressInPool());
  /* The size of each column is needed to compute its CRC32. Since the method
   * size() has a linear complexity with the number of children of a TreeNode,
   * we do a workaround to compute the size in constant time.
   * This relies on the fact that the lists contain only FloatNodes, and that
   * the columns have the same number of elements. */
  size_t dataLengthOfSeries = 2 * (m_dataLists[series][0].sizeOfNode() + m_dataLists[series][0].childAtIndex(0).sizeOfNode() * numberOfPairsOfSeries(series));
  /* Assert that the computed size is the real size.
   * It can be false if not all elements are floatNode for example.
   * */
  assert(dataLengthOfSeries == m_dataLists[series][0].size() + m_dataLists[series][1].size());
  // Assert that dataLengthInBytes is a multiple of 4
  assert((dataLengthOfSeries & 0x3) == 0);
  return Ion::crc32Word((uint32_t *)(m_dataLists[series][0].addressInPool()), dataLengthOfSeries / sizeof(uint32_t));

}

double DoublePairStore::defaultValue(int series, int i, int j) const {
  assert(series >= 0 && series < k_numberOfSeries);
  return 0.0;
}

}
