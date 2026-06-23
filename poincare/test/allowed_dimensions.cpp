#include <poincare/allowed_dimensions.h>
#include <quiz.h>

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(pcj_allowed_dimensions) {
  constexpr AllowedDimensions any = AllowedDimensions::Any();
  quiz_assert(any.allows(DimensionType::Scalar));
  quiz_assert(any.allows(DimensionType::Matrix));
  quiz_assert(any.allows(DimensionType::Unit));
  quiz_assert(any.allows(DimensionType::Boolean));
  quiz_assert(any.allows(DimensionType::Point));
  quiz_assert(any.listAllowed());

  constexpr AllowedDimensions anyNoUnit = AllowedDimensions::AnyNoUnit();
  quiz_assert(anyNoUnit.allows(DimensionType::Scalar));
  quiz_assert(anyNoUnit.allows(DimensionType::Matrix));
  quiz_assert(!anyNoUnit.allows(DimensionType::Unit));
  quiz_assert(anyNoUnit.allows(DimensionType::Boolean));
  quiz_assert(anyNoUnit.allows(DimensionType::Point));
  quiz_assert(anyNoUnit.listAllowed());

  constexpr AllowedDimensions pureScalar = AllowedDimensions::PureScalar();
  quiz_assert(pureScalar.allows(DimensionType::Scalar));
  quiz_assert(!pureScalar.allows(DimensionType::Matrix));
  quiz_assert(!pureScalar.allows(DimensionType::Unit));
  quiz_assert(!pureScalar.allows(DimensionType::Boolean));
  quiz_assert(!pureScalar.allows(DimensionType::Point));
  quiz_assert(!pureScalar.listAllowed());
}
