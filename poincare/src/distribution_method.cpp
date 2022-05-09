#include <poincare/distribution_method.h>
#include <poincare/cdf_function.h>
#include <poincare/cdf_range_function.h>
#include <poincare/pdf_function.h>
#include <poincare/inv_function.h>
#include <new>

namespace Poincare {

const DistributionMethod * DistributionMethod::Get(Type type) {
  switch (type) {
  case Type::CDF:
    static constexpr CDFFunction cdf;
    return &cdf;
  case Type::PDF:
    static constexpr PDFFunction pdf;
    return &pdf;
  case Type::Inverse:
    static constexpr InverseFunction inverse;
    return &inverse;
  case Type::CDFRange:
    static constexpr CDFRangeFunction cdfRange;
    return &cdfRange;
  }
}

}
