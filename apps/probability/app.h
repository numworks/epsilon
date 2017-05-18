#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include <poincare.h>
#include "law_controller.h"
#include "../shared/text_field_delegate_app.h"
#include "law/binomial_law.h"
#include "law/exponential_law.h"
#include "law/normal_law.h"
#include "law/poisson_law.h"
#include "law/uniform_law.h"

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
      const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    Snapshot();
    ~Snapshot();
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
    Law * law();
  private:
    constexpr static int k_lawSizes[] = {sizeof(BinomialLaw),sizeof(ExponentialLaw), sizeof(NormalLaw), sizeof(PoissonLaw), sizeof(UniformLaw), 0};
    constexpr static size_t k_lawSize = max(k_lawSizes);
    char m_law[k_lawSize];
  };
private:
  App(Container * container, Snapshot * snapshot);
  LawController m_lawController;
  StackViewController m_stackViewController;
};

}

#endif
