#pragma once

#include <apps/inference/models/confidence_interval.h>
#include <apps/inference/models/inference_model.h>
#include <apps/inference/models/significance_test.h>

namespace Inference {

class ControllerContainer;

class InferenceController {
 public:
  InferenceController(InferenceModel* inferenceModel,
                      ControllerContainer* controllerContainer = nullptr)
      : m_inferenceModel(inferenceModel),
        m_controllerContainer(controllerContainer) {}

 protected:
  // Static conversions to specific inference models
  constexpr const SignificanceTest* significanceTestModel() const {
    return static_cast<const SignificanceTest*>(m_inferenceModel);
  }
  constexpr SignificanceTest* significanceTestModel() {
    return static_cast<SignificanceTest*>(m_inferenceModel);
  }

  constexpr const ConfidenceInterval* confidenceIntervalModel() const {
    return static_cast<const ConfidenceInterval*>(m_inferenceModel);
  }
  constexpr ConfidenceInterval* confidenceIntervalModel() {
    return static_cast<ConfidenceInterval*>(m_inferenceModel);
  }

  InferenceModel* m_inferenceModel;
  ControllerContainer* m_controllerContainer;
};

}  // namespace Inference
