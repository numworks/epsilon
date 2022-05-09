#include <poincare/distribution_method.h>
#include <poincare/cdf_function.h>
#include <poincare/cdf_range_function.h>
#include <poincare/pdf_function.h>
#include <poincare/inv_function.h>
#include <new>

namespace Poincare {

void DistributionMethod::Initialize(DistributionMethod * distribution, Type type) {
  switch (type) {
  case Type::CDF:
    new (distribution) CDFFunction();
    break;
  case Type::PDF:
    new (distribution) PDFFunction();
    break;
  case Type::Inverse:
    new (distribution) InverseFunction();
    break;
  case Type::CDFRange:
    new (distribution) CDFRangeFunction();
    break;
  }
}

}
