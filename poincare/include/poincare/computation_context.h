#ifndef POINCARE_COMPUTATION_CONTEXT_H
#define POINCARE_COMPUTATION_CONTEXT_H

#include <poincare/context.h>
#include <poincare/preferences.h>

namespace Poincare {

enum class ReductionTarget {
  /* Minimal reduction: this at least reduces rationals operations as
   * "1-0.3-0.7 --> 0" */
  SystemForApproximation = 0,
  /* Expansion of Newton multinome to be able to identify polynoms */
  SystemForAnalysis,
  /* Additional features as:
   * - factorizing on a common denominator
   * - turning complex expression into the form a+ib
   * - identifying tangent in cos/sin polynoms ... */
  User
};

enum class SymbolicComputation {
  ReplaceAllSymbolsWithDefinitionsOrUndefined = 0,
  ReplaceAllDefinedSymbolsWithDefinition = 1,
  ReplaceDefinedFunctionsWithDefinitions = 2,
  ReplaceAllSymbolsWithUndefined = 3, // Used in UnitConvert::shallowReduce
  DoNotReplaceAnySymbol = 4
};

enum class UnitConversion {
  None = 0,
  Default,
  InternationalSystem
};

class ComputationContext {
public:
  ComputationContext(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) :
    m_context(context),
    m_complexFormat(complexFormat),
    m_angleUnit(angleUnit)
  {}

  Context * context() const { return m_context; }
  void setContext(Context * context) { m_context = context; }
  Preferences::ComplexFormat complexFormat() const { return m_complexFormat; }
  void setComplextFormat(Preferences::ComplexFormat complexFormat) { m_complexFormat = complexFormat; }
  Preferences::AngleUnit angleUnit() const { return m_angleUnit; }
  void setAngleUnit(Preferences::AngleUnit angleUnit) { m_angleUnit = angleUnit; }

private:
  Context * m_context;
  Preferences::ComplexFormat m_complexFormat;
  Preferences::AngleUnit m_angleUnit;
};

class ReductionContext : public ComputationContext {
public:
  static ReductionContext NonInvasiveReductionContext(const ReductionContext& reductionContext) {
    return ReductionContext(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), reductionContext.unitFormat(), reductionContext.target(), SymbolicComputation::DoNotReplaceAnySymbol, UnitConversion::None);
  }
  static ReductionContext DefaultReductionContextForAnalysis(Context * context) {
    return ReductionContext(context, Preferences::ComplexFormat::Cartesian, Preferences::AngleUnit::Radian, Preferences::UnitFormat::Metric, ReductionTarget::SystemForAnalysis, SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, UnitConversion::None );
  }

  ReductionContext(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ReductionTarget target, SymbolicComputation symbolicComputation = SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, UnitConversion unitConversion = UnitConversion::Default, bool shouldExpandMultiplication = true, bool shouldCheckMatrices = true, bool shouldExpandLogarithm = true) :
    ComputationContext(context, complexFormat, angleUnit),
    m_unitFormat(unitFormat),
    m_target(target),
    m_symbolicComputation(symbolicComputation),
    m_unitConversion(unitConversion),
    m_shouldExpandMultiplication(shouldExpandMultiplication),
    m_shouldCheckMatrices(shouldCheckMatrices),
    m_shouldExpandLogarithm(shouldExpandLogarithm)
  {}
  ReductionContext() : ReductionContext(nullptr, Preferences::ComplexFormat::Cartesian, Preferences::AngleUnit::Radian, Preferences::UnitFormat::Metric, ReductionTarget::User) {}

  Preferences::UnitFormat unitFormat() const { return m_unitFormat; }
  ReductionTarget target() const { return m_target; }
  void setTarget(ReductionTarget target) { m_target = target; }
  SymbolicComputation symbolicComputation() const { return m_symbolicComputation; }
  void setSymbolicComputation(SymbolicComputation symbolicComputation) { m_symbolicComputation = symbolicComputation; }
  void setUnitConversion(UnitConversion unitConversion) { m_unitConversion = unitConversion; }
  UnitConversion unitConversion() const { return m_unitConversion; }
  void setExpandMultiplication(bool shouldExpandMultiplication) { m_shouldExpandMultiplication = shouldExpandMultiplication; }
  bool shouldExpandMultiplication() const { return m_shouldExpandMultiplication; }
  void setCheckMatrices(bool shouldCheckMatrices) { m_shouldCheckMatrices = shouldCheckMatrices; }
  bool shouldCheckMatrices() const { return m_shouldCheckMatrices; }
  void setExpandLogarithm(bool shouldExpandLogarithm) { m_shouldExpandLogarithm = shouldExpandLogarithm; }
  bool shouldExpandLogarithm() const { return m_shouldExpandLogarithm; }

private:
  Preferences::UnitFormat m_unitFormat;
  ReductionTarget m_target;
  SymbolicComputation m_symbolicComputation;
  UnitConversion m_unitConversion;
  bool m_shouldExpandMultiplication;
  bool m_shouldCheckMatrices;
  bool m_shouldExpandLogarithm;
};

class ApproximationContext : public ComputationContext {
public:
  ApproximationContext(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce = false) :
    ComputationContext(context, complexFormat, angleUnit),
    m_withinReduce(withinReduce)
  {}
  ApproximationContext(const ReductionContext& reductionContext, bool withinReduce) : ApproximationContext(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), withinReduce) {}

  bool withinReduce() const { return m_withinReduce; }

private:
  bool m_withinReduce;
};

}

#endif
