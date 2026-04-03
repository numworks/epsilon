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
      : m_start(start, static_cast<TFloat>(NAN)),
        m_end(end),
        m_searchStep(std::fabs(end - start) / static_cast<TFloat>(100)) {}

  TFloat start() const { return m_start.x(); }
  TFloat end() const { return m_end; }

  XY first() const { return m_start; }
  XY second() const { return m_pointA; }
  XY third() const { return m_pointB; }

  template <typename TNumericHelper>
  bool isAdmissibleSolution(TFloat x) const {
    TFloat minStep =
        TSpecialization::template MinimalStep<TNumericHelper>(m_start.x());
    /* NAN is implicitly handled by the comparisons. */
    return m_start.x() < m_end ? m_start.x() + minStep < x && x < m_end
                               : m_end < x && x < m_start.x() - minStep;
  }

  void setSearchStep(TFloat step) { m_searchStep = step; }

  void setStart(TFloat x) {
    m_start = XY{x, NAN};
    m_pointA = XY{};
    m_pointB = XY{};
  }

  void stretch(bool onlyEnd = false) {
    TFloat sign =
        m_start.x() < m_end ? static_cast<TFloat>(1) : static_cast<TFloat>(-1);
    if (!onlyEnd) {
      m_start.setX(m_start.x() - m_searchStep * sign);
    }
    m_end += m_searchStep * sign;
  }

  template <typename TEvaluator>
  void init(const TEvaluator& f) {
    m_pointB = XY(m_start.x(), f(m_start.x()));
    step(f);
    step(f);
  }

  template <typename TEvaluator>
  bool step(const TEvaluator& f) {
    TFloat x = static_cast<TSpecialization*>(this)->nextX();
    bool withinBounds =
        (m_start.x() < m_end) == (m_pointB.x() < x && x < m_end);
    m_start = m_pointA;
    m_pointA = m_pointB;
    m_pointB = XY(x, f(x));
    return withinBounds;
  }

 protected:
  XY m_start;
  XY m_pointA = {};
  XY m_pointB = {};
  TFloat m_end;
  TFloat m_searchStep;
};

template <typename TFloat>
class AdaptiveLogStepper
    : public StepperBase<TFloat, AdaptiveLogStepper<TFloat>> {
 public:
  using Base = StepperBase<TFloat, AdaptiveLogStepper<TFloat>>;
  friend Base;

  using Base::Base;  // inherit constructor

  void setGrowthSpeed(GrowthSpeed g) { m_growthSpeed = g; }

  // Called by StepperBase::step via CRTP
  TFloat nextX() const {
    TFloat x = this->m_pointB.x();
    TFloat direction = this->m_end;
    // Compute slope; fall back to 1 during init (m_pointA is NaN)
    TFloat slope;
    if (std::isnan(this->m_pointA.x())) {
      slope = static_cast<TFloat>(1);
    } else {
      TFloat dx = this->m_pointB.x() - this->m_pointA.x();
      slope = dx != static_cast<TFloat>(0)
                  ? (this->m_pointB.y() - this->m_pointA.y()) / dx
                  : static_cast<TFloat>(1);
    }
    return nextXImpl(x, direction, slope);
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
  GrowthSpeed m_growthSpeed = GrowthSpeed::Precise;

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
