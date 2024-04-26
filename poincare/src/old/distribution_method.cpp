#include <poincare/old/cdf_method.h>
#include <poincare/old/cdf_range_method.h>
#include <poincare/old/distribution_method.h>
#include <poincare/old/inv_method.h>
#include <poincare/old/pdf_method.h>

namespace Poincare {

const DistributionMethod* DistributionMethod::Get(Type type) {
  switch (type) {
    case Type::CDF:
      constexpr static CDFMethod cdf;
      return &cdf;
    case Type::PDF:
      constexpr static PDFMethod pdf;
      return &pdf;
    case Type::Inverse:
      constexpr static InverseMethod inverse;
      return &inverse;
    default:
      assert(type == Type::CDFRange);
      constexpr static CDFRangeMethod cdfRange;
      return &cdfRange;
  }
}

}  // namespace Poincare
