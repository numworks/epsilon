#ifndef DISTRIBUTIONS_DISTRIBUTION_BUFFER_H
#define DISTRIBUTIONS_DISTRIBUTION_BUFFER_H

#include <new>

#include "distribution/binomial_distribution.h"
#include "distribution/chi_squared_distribution.h"
#include "distribution/exponential_distribution.h"
#include "distribution/fisher_distribution.h"
#include "distribution/geometric_distribution.h"
#include "distribution/hypergeometric_distribution.h"
#include "distribution/normal_distribution.h"
#include "distribution/poisson_distribution.h"
#include "distribution/student_distribution.h"
#include "distribution/uniform_distribution.h"

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

  Distribution* distribution() { return reinterpret_cast<Distribution*>(this); }

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

}  // namespace Distributions

#endif /* DISTRIBUTIONS_DISTRIBUTION_BUFFER_H */
