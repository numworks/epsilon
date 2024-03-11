#include <assert.h>
#include <ion/storage/file_system.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>
#include <poincare/unit.h>

namespace Poincare {

constexpr int Preferences::DefaultNumberOfPrintedSignificantDigits;
constexpr int Preferences::VeryLargeNumberOfSignificantDigits;
constexpr int Preferences::LargeNumberOfSignificantDigits;
constexpr int Preferences::MediumNumberOfSignificantDigits;
constexpr int Preferences::ShortNumberOfSignificantDigits;
constexpr int Preferences::VeryShortNumberOfSignificantDigits;

Preferences::Preferences()
    : m_calculationPreferences{
          .angleUnit = AngleUnit::Radian,
          .displayMode = Preferences::PrintFloatMode::Decimal,
          .editionMode = EditionMode::Edition2D,
          .complexFormat = Preferences::ComplexFormat::Real,
          .numberOfSignificantDigits =
              Preferences::DefaultNumberOfPrintedSignificantDigits} {}

static Preferences* fetchFromStorage() {
  Ion::Storage::Record record =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtension(
          Preferences::k_recordName, Ion::Storage::systemExtension);
  assert(!record.isNull());
  Ion::Storage::Record::Data data = record.value();
  assert(data.size == sizeof(Preferences));
  return static_cast<Preferences*>(const_cast<void*>(data.buffer));
}

Preferences* Preferences::SharedPreferences() {
  static Preferences* ptr = fetchFromStorage();
  assert(fetchFromStorage() == ptr);
  return ptr;
}

Preferences::ComplexFormat Preferences::UpdatedComplexFormatWithExpressionInput(
    ComplexFormat complexFormat, const Expression& exp, Context* context) {
  if (complexFormat == ComplexFormat::Real && exp.hasComplexI(context)) {
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
  m_examMode = mode;
  Ion::ExamMode::set(mode);
}

}  // namespace Poincare
