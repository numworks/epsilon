#include <poincare/distribution_method.h>
#include <poincare/cdf_method.h>
#include <poincare/cdf_range_method.h>
#include <poincare/pdf_method.h>
#include <poincare/inv_method.h>

namespace Poincare {

const DistributionMethod * DistributionMethod::Get(Type type) {
  switch (type) {
  case Type::CDF:
    static constexpr CDFMethod cdf;
    return &cdf;
  case Type::PDF:
    static constexpr PDFMethod pdf;
    return &pdf;
  case Type::Inverse:
    static constexpr InverseMethod inverse;
    return &inverse;
  case Type::CDFRange:
    static constexpr CDFRangeMethod cdfRange;
    return &cdfRange;
  }
}

}
