#include <emscripten/bind.h>
#include <poincare/old/computation_context.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>

#include <string>
using namespace emscripten;

namespace Poincare::JSBridge {

struct ReductionPreferences {
  Preferences::ComplexFormat complexFormat;
  Preferences::AngleUnit angleUnit;
  Preferences::UnitFormat unitFormat;
};

ReductionContext BuildReductionContext(
    Context* ctx, ReductionPreferences preferences,
    SymbolicComputation symbolicComputation) {
  return ReductionContext(ctx, preferences.complexFormat, preferences.angleUnit,
                          preferences.unitFormat, ReductionTarget::User,
                          symbolicComputation, UnitConversion::Default);
}

EMSCRIPTEN_BINDINGS(computation_context) {
  enum_<SymbolicComputation>("SymbolicComputation")
      .value("ReplaceAllSymbols", SymbolicComputation::ReplaceAllSymbols)
      .value("ReplaceDefinedSymbols",
             SymbolicComputation::ReplaceDefinedSymbols)
      .value("ReplaceDefinedFunctions",
             SymbolicComputation::ReplaceDefinedFunctions)
      .value("ReplaceAllSymbolsWithUndefined",
             SymbolicComputation::ReplaceAllSymbolsWithUndefined)
      .value("KeepAllSymbols", SymbolicComputation::KeepAllSymbols);

  value_object<ReductionPreferences>("ReductionPreferences")
      .field("complexFormat", &ReductionPreferences::complexFormat)
      .field("angleUnit", &ReductionPreferences::angleUnit)
      .field("unitFormat", &ReductionPreferences::unitFormat);

  class_<Context>("PCR_Context");
  class_<EmptyContext, base<Context>>("PCR_EmptyContext").constructor<>();

  class_<ComputationContext>("PCR_ComputationContext")
      .function("updateComplexFormat",
                &ComputationContext::updateComplexFormat);
  class_<ReductionContext, base<ComputationContext>>("PCR_ReductionContext")
      .class_function("Build", &BuildReductionContext, allow_raw_pointers());
}

}  // namespace Poincare::JSBridge
