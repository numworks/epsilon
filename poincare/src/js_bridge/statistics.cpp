#include <emscripten/bind.h>
#include <poincare/src/numeric/statistics_dataset.h>
#include <poincare/src/regression/dataset_adapter.h>
#include <poincare/src/regression/series.h>

#include "utils.h"

using namespace emscripten;
using namespace Poincare::Internal;
using namespace Poincare::Regression;

namespace Poincare::JSBridge {

class DatasetColumnFromJsArray : public DatasetColumn<double> {
 public:
  DatasetColumnFromJsArray() : m_array(FloatArray(val::undefined())) {}
  DatasetColumnFromJsArray(const FloatArray& array) : m_array(array) {}

  double valueAtIndex(int index) const override {
    return m_array[index].as<double>();
  };
  int length() const override { return m_array["length"].as<int>(); }

  const FloatArray getArray() const { return m_array; }

 private:
  const FloatArray m_array;
};

class StatisticsDatasetFromJsArrays : public StatisticsDataset<double> {
 public:
  StatisticsDatasetFromJsArrays(const FloatArray& values,
                                const FloatArray& weights, bool lnOfValues,
                                bool oppositeOfValue)
      : StatisticsDataset<double>(
            Utils::ArraysHaveSameLength(values, weights) ? &m_valuesArray
                                                         : nullptr,
            Utils::ArraysHaveSameLength(values, weights) ? &m_weightsArray
                                                         : nullptr,
            lnOfValues, oppositeOfValue),
        m_valuesArray(values),
        m_weightsArray(weights) {}

  StatisticsDatasetFromJsArrays(const FloatArray& values, bool lnOfValues,
                                bool oppositeOfValue)
      : StatisticsDataset<double>(&m_valuesArray, nullptr, lnOfValues,
                                  oppositeOfValue),
        m_valuesArray(values) {}

  StatisticsDatasetFromJsArrays(const FloatArray& values)
      : StatisticsDatasetFromJsArrays(values, false, false) {}

  StatisticsDatasetFromJsArrays(const FloatArray& values,
                                const FloatArray& weights)
      : StatisticsDatasetFromJsArrays(values, weights, false, false) {}

  StatisticsDatasetFromJsArrays() : StatisticsDataset<double>() {}

  const FloatArray& getValuesArray() const { return m_valuesArray.getArray(); }
  const FloatArray& getWeightsArray() const {
    return m_weightsArray.getArray();
  }

 private:
  const DatasetColumnFromJsArray m_valuesArray;
  const DatasetColumnFromJsArray m_weightsArray;
};

// Wrappers for default arguments

int indexAtCumulatedFrequencyWrapper(const StatisticsDataset<double>& dataset,
                                     double freq) {
  return dataset.indexAtCumulatedFrequency(freq);
}

int indexAtCumulatedWeightWrapper(const StatisticsDataset<double>& dataset,
                                  double weight) {
  return dataset.indexAtCumulatedWeight(weight);
}
int medianIndexWrapper(const StatisticsDataset<double>& dataset) {
  return dataset.medianIndex();
}

EMSCRIPTEN_BINDINGS(statistics) {
  class_<StatisticsDataset<double>>("PCR_VirtualStatisticsDataset")
      .function("isUndefined", &StatisticsDataset<double>::isUndefined)
      .function("setHasBeenModified",
                &StatisticsDataset<double>::setHasBeenModified)
      .function("indexAtSortedIndex",
                &StatisticsDataset<double>::indexAtSortedIndex)
      .function("totalWeight", &StatisticsDataset<double>::totalWeight)
      .function("weightedSum", &StatisticsDataset<double>::weightedSum)
      .function("offsettedSquaredSum",
                &StatisticsDataset<double>::offsettedSquaredSum)
      .function("squaredSum", &StatisticsDataset<double>::squaredSum)
      .function("squaredSumOffsettedByLinearTransformationOfDataset",
                &StatisticsDataset<
                    double>::squaredSumOffsettedByLinearTransformationOfDataset,
                allow_raw_pointers())
      .function("mean", &StatisticsDataset<double>::mean)
      .function("variance", &StatisticsDataset<double>::variance)
      .function("standardDeviation",
                &StatisticsDataset<double>::standardDeviation)
      .function("sampleStandardDeviation",
                &StatisticsDataset<double>::sampleStandardDeviation)
      .function("sortedElementAtCumulatedFrequency",
                &StatisticsDataset<double>::sortedElementAtCumulatedFrequency)
      .function("sortedElementAtCumulatedWeight",
                &StatisticsDataset<double>::sortedElementAtCumulatedWeight)
      .function("median", &StatisticsDataset<double>::median)
      .function("min", &StatisticsDataset<double>::min)
      .function("max", &StatisticsDataset<double>::max)
      .function("indexAtCumulatedFrequency", &indexAtCumulatedFrequencyWrapper)
      .function("indexAtCumulatedWeight", &indexAtCumulatedWeightWrapper)
      .function("medianIndex", &medianIndexWrapper);

  class_<StatisticsDatasetFromJsArrays, base<StatisticsDataset<double>>>(
      "PCR_StatisticsDataset")
      .constructor<const FloatArray&, const FloatArray&, bool, bool>()
      .constructor<const FloatArray&, bool, bool>()
      .constructor<const FloatArray&>()
      .constructor<const FloatArray&, const FloatArray&>()
      .constructor<>()
      .function("getValuesArray",
                &StatisticsDatasetFromJsArrays::getValuesArray)
      .function("getWeightsArray",
                &StatisticsDatasetFromJsArrays::getWeightsArray);
};

}  // namespace Poincare::JSBridge
