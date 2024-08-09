#include <emscripten/bind.h>
#include <poincare/old/computation_context.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>

#include <string>
using namespace emscripten;

namespace Poincare::JSBridge {

ReductionContext DefaultReductionContext(Context* ctx, bool keepSymbols) {
  return ReductionContext(
      ctx, Preferences::ComplexFormat::Cartesian,
      Preferences::AngleUnit::Radian, Preferences::UnitFormat::Metric,
      ReductionTarget::User,
      keepSymbols
          ? SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition
          : SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      UnitConversion::Default);
}

EMSCRIPTEN_BINDINGS(computation_context) {
  enum_<Preferences::ComplexFormat>("ComplexFormat")
      .value("Real", Preferences::ComplexFormat::Real)
      .value("Cartesian", Preferences::ComplexFormat::Cartesian)
      .value("Polar", Preferences::ComplexFormat::Polar);
  enum_<Preferences::AngleUnit>("AngleUnit")
      .value("Radian", Preferences::AngleUnit::Radian)
      .value("Degree", Preferences::AngleUnit::Degree)
      .value("Gradian", Preferences::AngleUnit::Gradian);
  enum_<Preferences::UnitFormat>("UnitFormat")
      .value("Metric", Preferences::UnitFormat::Metric)
      .value("Imperial", Preferences::UnitFormat::Imperial);
  enum_<ReductionTarget>("ReductionTarget")
      .value("SystemForApproximation", ReductionTarget::SystemForApproximation)
      .value("SystemForAnalysis", ReductionTarget::SystemForAnalysis)
      .value("User", ReductionTarget::User);
  enum_<SymbolicComputation>("SymbolicComputation")
      .value("ReplaceAllSymbolsWithDefinitionsOrUndefined",
             SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined)
      .value("ReplaceAllDefinedSymbolsWithDefinition",
             SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition)
      .value("ReplaceDefinedFunctionsWithDefinitions",
             SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions)
      .value("ReplaceAllSymbolsWithUndefined",
             SymbolicComputation::ReplaceAllSymbolsWithUndefined)
      .value("DoNotReplaceAnySymbol",
             SymbolicComputation::DoNotReplaceAnySymbol);
  enum_<UnitConversion>("UnitConversion")
      .value("None", UnitConversion::None)
      .value("Default", UnitConversion::Default)
      .value("InternationalSystem", UnitConversion::InternationalSystem);

  class_<Context>("PCR_Context");
  class_<EmptyContext, base<Context>>("PCR_EmptyContext").constructor<>();

  class_<ComputationContext>("PCR_ComputationContext")
      .function("updateComplexFormat",
                &ComputationContext::updateComplexFormat);
  class_<ReductionContext, base<ComputationContext>>("PCR_ReductionContext")
      .constructor<Context*, Preferences::ComplexFormat, Preferences::AngleUnit,
                   Preferences::UnitFormat, ReductionTarget,
                   SymbolicComputation, UnitConversion>()
      .class_function("Default", &DefaultReductionContext,
                      allow_raw_pointers());
}

}  // namespace Poincare::JSBridge
