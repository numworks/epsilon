#pragma once

#include <escher/view_controller.h>
#include <omg/unreachable.h>

#include "chi_square/categorical_type_controller.h"
#include "chi_square/input_goodness_controller.h"
#include "chi_square/input_homogeneity_controller.h"
#include "chi_square/results_goodness_controller.h"
#include "chi_square/results_homogeneity_controller.h"
#include "confidence_interval/interval_graph_controller.h"
#include "dataset_controller.h"
#include "input_controller.h"
#include "results_controller.h"
#include "significance_test/hypothesis_display_only_controller.h"
#include "significance_test/hypothesis_editable_controller.h"
#include "significance_test/test_graph_controller.h"
#include "store/input_store_controller.h"
#include "test_controller.h"
#include "type_controller.h"

namespace Inference {

class ControllerContainer {
 public:
  TestGraphController m_testGraphController;
  IntervalGraphController m_intervalGraphController;
  ResultsHomogeneityTabController m_homogeneityResultsController;
  InputHomogeneityController m_inputHomogeneityController;
  ResultsGoodnessTabController m_goodnessResultsController;
  InputGoodnessController m_inputGoodnessController;
  InputStoreController m_inputStoreController1;
  InputStoreController m_inputStoreController2;
  ResultsController m_resultsController;
  InputController m_inputController;
  TypeController m_typeController;
  CategoricalTypeController m_categoricalTypeController;
  HypothesisEditableController m_hypothesisEditableController;
  HypothesisDisplayOnlyController m_hypothesisDisplayOnlyController;
  DatasetController m_datasetController;
  TestController m_testController;
};

}  // namespace Inference
