#include <poincare/distribution_method.h>
#include <poincare/cdf_function.h>
#include <poincare/cdf_range_function.h>
#include <poincare/pdf_function.h>
#include <poincare/inv_function.h>
#include <new>

namespace Poincare {

DistributionMethod * DistributionMethod::Get(Type type) {
  switch (type) {
  case Type::CDF:
    static CDFFunction cdf;
    return &cdf;
  case Type::PDF:
    static PDFFunction pdf;
    return &pdf;
  case Type::Inverse:
    static InverseFunction inverse;
    return &inverse;
  case Type::CDFRange:
    static CDFRangeFunction cdfRange;
    return &cdfRange;
  }
}

}
