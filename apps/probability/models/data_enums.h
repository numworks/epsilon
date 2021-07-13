#ifndef APPS_PROBABILITY_MODELS_DATA_ENUMS_H
#define APPS_PROBABILITY_MODELS_DATA_ENUMS_H

namespace Probability {
namespace Data {

// Navigation

enum class Page {
  Menu,
  Distribution,
  Parameters,
  Calculations,
  Test,
  Type,
  Hypothesis,
  Categorical,
  Input,
  IntervalInput,
  InputGoodness,
  InputHomogeneity,
  Results,
  ResultsHomogeneity,
  ProbaGraph,
  Graph
};

enum class SubApp { Unset, Probability, Tests, Intervals };

class AppNavigation {
public:
  SubApp subapp() { return m_subApp; };
  SubApp * subappPointer() { return &m_subApp; };
  void setSubapp(SubApp app) { m_subApp = app; };
  Page page() const { return m_page; }
  void setPage(Page page) { m_page = page; }

private:
  SubApp m_subApp;
  Page m_page;
};

enum class Test { Unset, OneProp, OneMean, TwoProps, TwoMeans, Categorical };

inline bool isProportion(Test t) {
  return t == Test::OneProp || t == Test::TwoProps;
}

enum class TestType { TTest, PooledTTest, ZTest };

enum class CategoricalType { None, Goodness, Homogeneity };

}  // namespace Data

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_DATA_ENUMS_H */
