#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include "distribution_controller.h"
#include "calculation_controller.h"
#include "parameters_controller.h"
#include "../shared/text_field_delegate_app.h"
#include "distribution/binomial_distribution.h"
#include "distribution/exponential_distribution.h"
#include "distribution/normal_distribution.h"
#include "distribution/poisson_distribution.h"
#include "distribution/uniform_distribution.h"
#include "calculation/left_integral_calculation.h"
#include "calculation/right_integral_calculation.h"
#include "calculation/finite_integral_calculation.h"
#include "../shared/shared_app.h"

constexpr static size_t max(const int * data, int seed = 0) {
  return (*data == 0 ? seed : max(data+1, *data > seed ? *data : seed));
}

namespace Probability {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
    public:
      I18n::Message name() override;
      I18n::Message upperName() override;
      App::Descriptor::ExaminationLevel examinationLevel() override;
      const Image * icon() override;
  };
  class Snapshot : public ::SharedApp::Snapshot {
  public:
    enum class Page {
      Distribution,
      Parameters,
      Calculations
    };
    Snapshot();
    ~Snapshot();
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
    void reset() override;
    Distribution * distribution();
    Calculation * calculation();
    Page activePage();
    void setActivePage(Page activePage);

  private:
    constexpr static int k_distributionSizes[] = {sizeof(BinomialDistribution),sizeof(ExponentialDistribution), sizeof(NormalDistribution), sizeof(PoissonDistribution), sizeof(UniformDistribution), 0};
    constexpr static size_t k_distributionSize = max(k_distributionSizes);
    constexpr static int k_calculationSizes[] = {sizeof(LeftIntegralCalculation),sizeof(FiniteIntegralCalculation), sizeof(RightIntegralCalculation), 0};
    constexpr static size_t k_calculationSize = max(k_calculationSizes);

    void deleteDistributionAndCalculation();
    void initializeDistributionAndCalculation();

#if (defined __EMSCRIPTEN__) || (defined _FXCG)
    constexpr static int k_distributionAlignments[] = {alignof(BinomialDistribution),alignof(ExponentialDistribution), alignof(NormalDistribution), alignof(PoissonDistribution), alignof(UniformDistribution), 0};
    constexpr static size_t k_distributionAlignment = max(k_distributionAlignments);
    constexpr static int k_calculationAlignments[] = {alignof(LeftIntegralCalculation),alignof(FiniteIntegralCalculation), alignof(RightIntegralCalculation), 0};
    constexpr static size_t k_calculationAlignment = max(k_calculationAlignments);
    alignas(k_distributionAlignment) char m_distribution[k_distributionSize];
    alignas(k_calculationAlignment) char m_calculation[k_calculationSize];
#else
    char m_distribution[k_distributionSize];
    char m_calculation[k_calculationSize];
#endif
    Page m_activePage;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  Snapshot * snapshot() const { return static_cast<Snapshot *>(::App::snapshot()); }
  TELEMETRY_ID("Probability");
private:
  App(Snapshot * snapshot);
  CalculationController m_calculationController;
  ParametersController m_parametersController;
  DistributionController m_distributionController;
  StackViewController m_stackViewController;
};

}

#endif
