#ifndef INFERENCE_MODELS_STATISTIC_SUBAPP_H
#define INFERENCE_MODELS_STATISTIC_SUBAPP_H

#include <poincare/statistics/inference.h>

namespace Inference {

enum class SubApp {
  SignificanceTest,
  ConfidenceInterval,
};

constexpr static int k_numberOfSubApps = 2;

using TestType = Poincare::Inference::TestType;
using StatisticType = Poincare::Inference::StatisticType;
using CategoricalType = Poincare::Inference::CategoricalType;

namespace Params = Poincare::Inference::Params;
namespace SignificanceTest = Poincare::Inference::SignificanceTest;
namespace ConfidenceInterval = Poincare::Inference::ConfidenceInterval;
}  // namespace Inference

#endif
