#include <assert.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/projection.h>

namespace Poincare {

constexpr int Preferences::DefaultNumberOfPrintedSignificantDigits;
constexpr int Preferences::VeryLargeNumberOfSignificantDigits;
constexpr int Preferences::LargeNumberOfSignificantDigits;
constexpr int Preferences::MediumNumberOfSignificantDigits;
constexpr int Preferences::ShortNumberOfSignificantDigits;
constexpr int Preferences::VeryShortNumberOfSignificantDigits;
Preferences::Interface* Preferences::s_preferences = nullptr;
const Preferences Preferences::PreferencesInstance;

ComplexFormat Preferences::UpdatedComplexFormatWithExpressionInput(
    ComplexFormat complexFormat, const Internal::Tree* exp,
    const SymbolContext& symbolContext, SymbolicComputation replaceSymbols) {
  Internal::ProjectionContext projectionContext = {
      .m_complexFormat = complexFormat,
      .m_symbolic = replaceSymbols,
      .m_context = symbolContext,
  };
  Internal::Projection::UpdateComplexFormatWithExpressionInput(
      exp, &projectionContext);
  return projectionContext.m_complexFormat;
}

}  // namespace Poincare
