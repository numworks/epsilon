#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/old/junior_expression.h>
#include <poincare/preferences.h>

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
    ComplexFormat complexFormat, const JuniorExpression& exp,
    Context* context) {
  if (complexFormat == ComplexFormat::Real && exp.hasComplexNodes(context)) {
    return k_defautComplexFormatIfNotReal;
  }
  return complexFormat;
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
