#include <emscripten/bind.h>
#include <poincare/expression.h>
#include <poincare/old/computation_context.h>
#include <poincare/old/empty_context.h>

#include <string>

#include "preferences.h"

using namespace emscripten;

namespace Poincare::JSBridge {

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

  class_<Context>("PCR_Context");
  class_<EmptyContext, base<Context>>("PCR_EmptyContext").constructor<>();

  class_<ComputationContext>("PCR_ComputationContext")
      .function("updateComplexFormat",
                &ComputationContext::updateComplexFormat);
  class_<ReductionContext, base<ComputationContext>>("PCR_ReductionContext")
      .class_function("Build", &BuildReductionContext, allow_raw_pointers());
}

}  // namespace Poincare::JSBridge
