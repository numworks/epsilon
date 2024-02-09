#include <poincare/cdf_method.h>
#include <poincare/cdf_range_method.h>
#include <poincare/distribution_method.h>
#include <poincare/inv_method.h>
#include <poincare/pdf_method.h>

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
