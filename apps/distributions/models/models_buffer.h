#ifndef DISTRIBUTIONS_MODELS_BUFFER_H
#define DISTRIBUTIONS_MODELS_BUFFER_H

#include <new>

#include "probability/distribution/binomial_distribution.h"
#include "probability/distribution/chi_squared_distribution.h"
#include "probability/distribution/exponential_distribution.h"
#include "probability/distribution/fisher_distribution.h"
#include "probability/distribution/geometric_distribution.h"
#include "probability/distribution/hypergeometric_distribution.h"
#include "probability/distribution/normal_distribution.h"
#include "probability/distribution/poisson_distribution.h"
#include "probability/distribution/student_distribution.h"
#include "probability/distribution/uniform_distribution.h"

namespace Distributions {

// Buffers for dynamic allocation

union DistributionBuffer {
public:
  DistributionBuffer() {
    new (&m_binomialDistribution) BinomialDistribution();
    distribution()->calculation()->compute(0);
  }
  ~DistributionBuffer() { distribution()->~Distribution(); }
  // Rule of 5
  DistributionBuffer(const DistributionBuffer& other) = delete;
  DistributionBuffer(DistributionBuffer&& other) = delete;
  DistributionBuffer& operator=(const DistributionBuffer& other) = delete;
  DistributionBuffer& operator=(DistributionBuffer&& other) = delete;

  Distribution * distribution() { return reinterpret_cast<Distribution *>(this); }

private:
  BinomialDistribution m_binomialDistribution;
  UniformDistribution m_uniformDistribution;
  ExponentialDistribution m_exponentDistribution;
  NormalDistribution m_normalDistribution;
  ChiSquaredDistribution m_chiSquaredDistribution;
  StudentDistribution m_studentDistribution;
  GeometricDistribution m_geometricDistribution;
  HypergeometricDistribution m_hypergeometricDistribution;
  PoissonDistribution m_poissonDistribution;
  FisherDistribution m_fisherDistribution;
};

union ModelBuffer {
public:
  ModelBuffer() { new (&m_distributionBuffer) DistributionBuffer(); }
  ~ModelBuffer() { inference()->~Inference(); }
  Inference * inference() { return reinterpret_cast<Inference *>(this); }
  Distribution * distribution() { return m_distributionBuffer.distribution(); }
  Calculation * calculation() { return distribution()->calculation(); }

private:
  DistributionBuffer m_distributionBuffer;
};

}  // namespace Distributions

#endif /* DISTRIBUTIONS_MODELS_BUFFER_H */
