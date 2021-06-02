#ifndef APPS_PROBABILITY_MODELS_TEST_RESULTS_DATA_SOURCE_H
#define APPS_PROBABILITY_MODELS_TEST_RESULTS_DATA_SOURCE_H

#include <escher/message_table_cell_with_editable_text.h>

namespace Probability {

class TestResultsDataSource {
public:
  virtual ~TestResultsDataSource() {}
  /* Compute results */
  virtual void compute() = 0;
  virtual float z() = 0;
  virtual float pValue() = 0;
  virtual bool hasDegreeOfFreedom() = 0;
  virtual float degreeOfFreedom() { return -1; };
};

// Temporary, for testing only
class MockupDataSource : public TestResultsDataSource {
public:
  void compute() override {}
  float z() override { return 2.3; }
  float pValue() override { return 0.867; }
  bool hasDegreeOfFreedom() { return false; }
};

}

#endif /* APPS_PROBABILITY_MODELS_TEST_RESULTS_DATA_SOURCE_H */
