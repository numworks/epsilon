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
      keepSymbols ? SymbolicComputation::ReplaceDefinedSymbols
                  : SymbolicComputation::ReplaceAllSymbols,
      UnitConversion::Default);
}

EMSCRIPTEN_BINDINGS(computation_context) {
  enum_<ReductionTarget>("ReductionTarget")
      .value("SystemForApproximation", ReductionTarget::SystemForApproximation)
      .value("SystemForAnalysis", ReductionTarget::SystemForAnalysis)
      .value("User", ReductionTarget::User);
  enum_<SymbolicComputation>("SymbolicComputation")
      .value("ReplaceAllSymbols", SymbolicComputation::ReplaceAllSymbols)
      .value("ReplaceDefinedSymbols",
             SymbolicComputation::ReplaceDefinedSymbols)
      .value("ReplaceDefinedFunctions",
             SymbolicComputation::ReplaceDefinedFunctions)
      .value("ReplaceAllSymbolsWithUndefined",
             SymbolicComputation::ReplaceAllSymbolsWithUndefined)
      .value("KeepAllSymbols", SymbolicComputation::KeepAllSymbols);
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
