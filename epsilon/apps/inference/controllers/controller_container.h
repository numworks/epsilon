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
#include "input_anova_controller.h"
#include "input_controller.h"
#include "results_anova_controller.h"
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
  ControllerContainer(Escher::StackViewController* stackViewController,
                      InferenceModel* inferenceModel)
      : m_testGraphController{stackViewController,
                              static_cast<SignificanceTest*>(inferenceModel)},
        m_intervalGraphController{
            stackViewController,
            static_cast<ConfidenceInterval*>(inferenceModel)},
        m_homogeneityResultsController{
            stackViewController, this,
            static_cast<HomogeneityTest*>(inferenceModel)},
        m_inputANOVAController{stackViewController, this,
                               static_cast<ANOVATest*>(inferenceModel)},
        m_resultsANOVAController{stackViewController, this,
                                 static_cast<ANOVATest*>(inferenceModel)},
        m_inputHomogeneityController{
            stackViewController, this,
            static_cast<HomogeneityTest*>(inferenceModel)},
        m_goodnessResultsController{stackViewController, this,
                                    static_cast<GoodnessTest*>(inferenceModel)},
        m_inputGoodnessController{stackViewController, this,
                                  static_cast<GoodnessTest*>(inferenceModel)},
        m_inputStoreController1{stackViewController, this, inferenceModel, 0},
        m_inputStoreController2{stackViewController, this, inferenceModel, 1},
        m_resultsController{stackViewController, inferenceModel, this},
        m_inputController{stackViewController, this, inferenceModel},
        m_typeController{stackViewController, this, inferenceModel},
        m_categoricalTypeController{
            stackViewController, static_cast<Chi2Test*>(inferenceModel), this},
        m_hypothesisEditableController{
            stackViewController, this,
            static_cast<SignificanceTest*>(inferenceModel)},
        m_hypothesisDisplayOnlyController{
            stackViewController, this,
            static_cast<SignificanceTest*>(inferenceModel)},
        m_datasetController{stackViewController, this, inferenceModel},
        m_testController{stackViewController, this, inferenceModel} {}

  TestGraphController m_testGraphController;
  IntervalGraphController m_intervalGraphController;
  ResultsHomogeneityTabController m_homogeneityResultsController;
  InputANOVAController m_inputANOVAController;
  ResultsANOVAController m_resultsANOVAController;
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
