#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/projection.h>

namespace Poincare {

constexpr int Preferences::DefaultNumberOfPrintedSignificantDigits;
constexpr int Preferences::VeryLargeNumberOfSignificantDigits;
constexpr int Preferences::LargeNumberOfSignificantDigits;
constexpr int Preferences::MediumNumberOfSignificantDigits;
constexpr int Preferences::ShortNumberOfSignificantDigits;
constexpr int Preferences::VeryShortNumberOfSignificantDigits;

Preferences::Preferences()
    : m_version(k_version),
      m_calculationPreferences{
          .angleUnit = AngleUnit::Radian,
          .displayMode = Preferences::PrintFloatMode::Decimal,
          .editionMode = EditionMode::Edition2D,
          .complexFormat = Preferences::ComplexFormat::Real,
          .numberOfSignificantDigits =
              Preferences::DefaultNumberOfPrintedSignificantDigits},
      m_forceExamModeReload(false) {}

Preferences::ComplexFormat Preferences::UpdatedComplexFormatWithExpressionInput(
    ComplexFormat complexFormat, const Internal::Tree* exp, Context* context,
    SymbolicComputation replaceSymbols) {
  Internal::ProjectionContext projectionContext = {
      .m_complexFormat = complexFormat,
      .m_symbolic = replaceSymbols,
      .m_context = context,
  };
  Internal::Projection::UpdateComplexFormatWithExpressionInput(
      exp, &projectionContext);
  return projectionContext.m_complexFormat;
}

ExamMode Preferences::examMode() const {
  if (m_examMode.isUninitialized()) {
    m_examMode = Ion::ExamMode::get();
  }
  return m_examMode;
}

void Preferences::setExamMode(ExamMode mode) {
  m_forceExamModeReload = false;
  m_examMode = mode;
  Ion::ExamMode::set(mode);
}

}  // namespace Poincare
