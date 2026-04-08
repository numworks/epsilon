#pragma once

#include <poincare/coordinate_2D.h>

#include <algorithm>

#include "numeric_helper.h"

namespace Poincare::NumericSolver {

enum class GrowthSpeed { Precise, Fast };

/* NOTE: Contains the basic logic to move a three point window along an
 * interval. However, the way the next point is computed is provided by
 * TSpecialization::nextX. */
template <typename TFloat, typename TSpecialization>
class StepperBase {
 public:
  using XY = Coordinate2D<TFloat>;

  StepperBase(TFloat start, TFloat end)
      : m_start(start),
        m_end(end),
        m_searchStep(std::fabs(end - start) / static_cast<TFloat>(100)) {}

  TFloat start() const { return m_start; }
  TFloat end() const { return m_end; }

  XY point(int i) const { return m_points[i]; }

  template <typename TNumericHelper>
  bool isAdmissibleSolution(TFloat x) const {
    TFloat minStep =
        TSpecialization::template MinimalStep<TNumericHelper>(m_start);
    /* NAN is implicitly handled by the comparisons. */
    return m_start < m_end ? m_start + minStep < x && x < m_end
                           : m_end < x && x < m_start - minStep;
  }

  void setSearchStep(TFloat step) { m_searchStep = step; }

  void setStart(TFloat x) {
    m_start = x;
    for (XY& p : m_points) {
      p = {};
    }
  }

  void stretch(bool onlyEnd = false) {
    TFloat sign =
        m_start < m_end ? static_cast<TFloat>(1) : static_cast<TFloat>(-1);
    if (!onlyEnd) {
      m_start = m_start - m_searchStep * sign;
    }
    m_end += m_searchStep * sign;
  }

  template <typename TEvaluator>
  void init(const TEvaluator& f) {
    m_points[2] = XY(m_start, f(m_start));
    stepImpl(f);  // skip the check, as m_points[2].x() == m_start
  }

  template <typename TEvaluator>
  bool step(const TEvaluator& f) {
    if ((m_start < m_points[2].x()) != (m_points[2].x() < m_end)) {
      return false;
    }

    stepImpl(f);
    return true;
  }

 protected:
  XY m_points[3] = {{}, {}, {}};
  TFloat m_start;
  TFloat m_end;
  TFloat m_searchStep;

 private:
  template <typename TEvaluator>
  void stepImpl(TEvaluator f) {
    TFloat x = static_cast<TSpecialization*>(this)->nextX();
    assert(std::isfinite(x));
    m_points[0] = m_points[1];
    m_points[1] = m_points[2];
    m_points[2] = XY(x, f(x));
  }
};

template <typename TFloat>
class BasicStepper : public StepperBase<TFloat, BasicStepper<TFloat>> {
  using Base = StepperBase<TFloat, BasicStepper<TFloat>>;

 public:
  using Base::Base;

  TFloat nextX() const {
    return (this->m_start < this->m_end)
               ? this->m_points[2] + this->m_searchStep
               : this->m_points[2] - this->m_searchStep;
  }

  template <typename TNumericHelper = DefaultNumericHelper<TFloat>>
  static TFloat MinimalStep(TFloat, TFloat = static_cast<TFloat>(1)) {
    return TNumericHelper::k_minimalPracticalStep;
  }
};

template <typename TFloat>
class AdaptiveLogStepper
    : public StepperBase<TFloat, AdaptiveLogStepper<TFloat>> {
  using Base = StepperBase<TFloat, AdaptiveLogStepper<TFloat>>;

 public:
  using Base::Base;

  void setGrowthSpeed(GrowthSpeed g) { m_growthSpeed = g; }

  // Called by StepperBase::step via CRTP
  TFloat nextX() const {
    TFloat slope;
    if constexpr (sizeof(TFloat) == sizeof(float)) {
      slope = 1.f;
    } else {
      TFloat dx = this->m_points[2].x() - this->m_points[1].x();
      slope = (this->m_points[2].y() - this->m_points[1].y()) / dx;
      if (std::isnan(slope)) {
        slope = static_cast<TFloat>(1);
      }
    }

    return nextXImpl(this->m_points[2].x(), this->m_end, slope);
  }

  template <typename TNumericHelper = DefaultNumericHelper<TFloat>>
  static TFloat MinimalStep(TFloat x, TFloat slope = static_cast<TFloat>(1)) {
    /* We make the minimal step dependent on the slope because if a function is
     * too flat, taking a step too small could lead to not detecting minimums
     * and maximums. */
    TFloat minimalStep = TNumericHelper::k_minimalPracticalStep;
    if constexpr (sizeof(TFloat) == sizeof(double)) {
      slope = std::fabs(slope);
      minimalStep *= std::exp(
          static_cast<TFloat>(7) *
          std::max(static_cast<TFloat>(0), static_cast<TFloat>(1) - slope));
    }
    return std::max(minimalStep,
                    std::fabs(x) * TNumericHelper::k_relativePrecision);
  }

 private:
  GrowthSpeed m_growthSpeed = sizeof(TFloat) == sizeof(float)
                                  ? GrowthSpeed::Fast
                                  : GrowthSpeed::Precise;

  TFloat nextXImpl(TFloat x, TFloat direction, TFloat slope) const {
    TFloat baseGrowthSpeed = (m_growthSpeed == GrowthSpeed::Fast)
                                 ? static_cast<TFloat>(1.05)
                                 : static_cast<TFloat>(1.01);
    constexpr TFloat k_maximalGrowthSpeed = static_cast<TFloat>(10);
    constexpr TFloat k_growthSpeedAcceleration = static_cast<TFloat>(1e-2);
    constexpr TFloat k_lowerTypicalMagnitude = static_cast<TFloat>(-1);
    constexpr TFloat k_upperTypicalMagnitude = static_cast<TFloat>(3);

    TFloat maxStep = this->m_searchStep;
    TFloat minStep = MinimalStep(x, slope);
    TFloat stepSign =
        x < direction ? static_cast<TFloat>(1) : static_cast<TFloat>(-1);

    TFloat magnitude = std::log10(std::fabs(x));
    if (!std::isfinite(magnitude) || minStep >= maxStep) {
      return x + stepSign * minStep;
    }

    TFloat ratio;
    if (k_lowerTypicalMagnitude <= magnitude &&
        magnitude <= k_upperTypicalMagnitude) {
      ratio = baseGrowthSpeed;
    } else {
      TFloat magnitudeDelta = magnitude < k_lowerTypicalMagnitude
                                  ? k_lowerTypicalMagnitude - magnitude
                                  : magnitude - k_upperTypicalMagnitude;
      ratio = k_maximalGrowthSpeed -
              (k_maximalGrowthSpeed - baseGrowthSpeed) *
                  std::exp(k_growthSpeedAcceleration *
                           -std::pow(magnitudeDelta, static_cast<TFloat>(3)));
    }
    TFloat x2 =
        (x < direction) == (x < static_cast<TFloat>(0)) ? x / ratio : x * ratio;
    if (std::fabs(x - x2) > maxStep) {
      x2 = x + stepSign * maxStep;
    }
    if (std::fabs(x - x2) < minStep) {
      x2 = x + stepSign * minStep;
    }
    return x2;
  }
};

}  // namespace Poincare::NumericSolver
