#ifndef POINCARE_DISTRIBUTION_FUNCTION_H
#define POINCARE_DISTRIBUTION_FUNCTION_H

#include <poincare/src/probability/distribution.h>
#include <poincare/src/probability/distribution_method.h>

#include "n_ary_expression.h"

namespace Poincare {

using Distribution = Internal::Distribution;
using DistributionMethod = Internal::DistributionMethod;

template <Distribution::Type T, DistributionMethod::Type U>
constexpr const char* getName();

template <>
constexpr const char*
getName<Distribution::Type::Normal, DistributionMethod::Type::CDF>() {
  return "normcdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Normal, DistributionMethod::Type::CDFRange>() {
  return "normcdfrange";
}
template <>
constexpr const char*
getName<Distribution::Type::Normal, DistributionMethod::Type::PDF>() {
  return "normpdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Normal, DistributionMethod::Type::Inverse>() {
  return "invnorm";
}

template <>
constexpr const char*
getName<Distribution::Type::Student, DistributionMethod::Type::CDF>() {
  return "tcdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Student, DistributionMethod::Type::CDFRange>() {
  return "tcdfrange";
}
template <>
constexpr const char*
getName<Distribution::Type::Student, DistributionMethod::Type::PDF>() {
  return "tpdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Student, DistributionMethod::Type::Inverse>() {
  return "invt";
}

template <>
constexpr const char*
getName<Distribution::Type::Binomial, DistributionMethod::Type::CDF>() {
  return "binomcdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Binomial, DistributionMethod::Type::PDF>() {
  return "binompdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Binomial, DistributionMethod::Type::Inverse>() {
  return "invbinom";
}

template <>
constexpr const char*
getName<Distribution::Type::Poisson, DistributionMethod::Type::CDF>() {
  return "poissoncdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Poisson, DistributionMethod::Type::PDF>() {
  return "poissonpdf";
}

template <>
constexpr const char*
getName<Distribution::Type::Geometric, DistributionMethod::Type::CDF>() {
  return "geomcdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Geometric, DistributionMethod::Type::CDFRange>() {
  return "geomcdfrange";
}
template <>
constexpr const char*
getName<Distribution::Type::Geometric, DistributionMethod::Type::PDF>() {
  return "geompdf";
}
template <>
constexpr const char*
getName<Distribution::Type::Geometric, DistributionMethod::Type::Inverse>() {
  return "invgeom";
}

template <>
constexpr const char*
getName<Distribution::Type::Hypergeometric, DistributionMethod::Type::CDF>() {
  return "hgeomcdf";
}
template <>
constexpr const char* getName<Distribution::Type::Hypergeometric,
                              DistributionMethod::Type::CDFRange>() {
  return "hgeomcdfrange";
}
template <>
constexpr const char*
getName<Distribution::Type::Hypergeometric, DistributionMethod::Type::PDF>() {
  return "hgeompdf";
}
template <>
constexpr const char* getName<Distribution::Type::Hypergeometric,
                              DistributionMethod::Type::Inverse>() {
  return "invhgeom";
}

class DistributionDispatcherNode : public NAryExpressionNode {
 public:
  // Simplication
  size_t size() const override { return sizeof(DistributionDispatcherNode); }

  const char* name() const { return m_name; }

  // Properties
  Type otype() const override { return Type::DistributionDispatcher; }

  OMG::Troolean isPositive(Context* context) const override {
    if (m_methodType != DistributionMethod::Type::Inverse) {
      return OMG::Troolean::True;
    }
    return OMG::Troolean::Unknown;
  }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    switch (m_distributionType) {
      case Distribution::Type::Normal:
        stream << "Norm";
        break;
      case Distribution::Type::Student:
        stream << "Student";
        break;
      case Distribution::Type::Binomial:
        stream << "Binom";
        break;
      case Distribution::Type::Poisson:
        stream << "Poisson";
        break;
      case Distribution::Type::Geometric:
        stream << "Geom";
        break;
      case Distribution::Type::Hypergeometric:
        stream << "Hypergeom";
        break;
      default:
        // Other distributions should not be accessible from calculation
        assert(false);
    }
    switch (m_methodType) {
      case DistributionMethod::Type::PDF:
        stream << "PDF";
        break;
      case DistributionMethod::Type::CDF:
        stream << "CDF";
        break;
      case DistributionMethod::Type::CDFRange:
        stream << "CDFRange";
        break;
      case DistributionMethod::Type::Inverse:
        stream << "Inv";
        break;
    }
  }
#endif

  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;

 private:
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  friend class DistributionDispatcher;
  void setType(DistributionMethod::Type f) { m_methodType = f; }
  void setDistribution(Distribution::Type d) { m_distributionType = d; }
  void setName(const char* name) { m_name = name; }

 private:
  const char* m_name;
  DistributionMethod::Type m_methodType;
  Distribution::Type m_distributionType;
};

class DistributionDispatcher final : public NAryExpression {
 public:
  DistributionDispatcher(const DistributionDispatcherNode* n)
      : NAryExpression(n) {}
  static DistributionDispatcher Builder(const Tuple& children = {}) {
    return PoolHandle::NAryBuilder<DistributionDispatcher,
                                   DistributionDispatcherNode>(
        convert(children));
  }

  template <Distribution::Type T, DistributionMethod::Type U>
  static OExpression hook(OExpression children) {
    OExpression exp =
        UntypedBuilderMultipleChildren<DistributionDispatcher>(children);
    DistributionDispatcher dist = exp.convert<DistributionDispatcher>();
    dist.setDistribution(T);
    dist.setType(U);
    dist.setName(getName<T, U>());
    return std::move(dist);
  }

  OExpression shallowReduce(ReductionContext reductionContext,
                            bool* stopReduction = nullptr);
  void setType(DistributionMethod::Type f) {
    static_cast<DistributionDispatcherNode*>(node())->setType(f);
  }
  void setDistribution(Distribution::Type d) {
    static_cast<DistributionDispatcherNode*>(node())->setDistribution(d);
  }
  void setName(const char* name) {
    static_cast<DistributionDispatcherNode*>(node())->setName(name);
  }

  Distribution::Type distributionType() {
    return static_cast<DistributionDispatcherNode*>(node())->m_distributionType;
  }

  DistributionMethod::Type methodType() {
    return static_cast<DistributionDispatcherNode*>(node())->m_methodType;
  }
};

template <Distribution::Type T, DistributionMethod::Type U>
constexpr OExpression::FunctionHelper makeHelper() {
  return OExpression::FunctionHelper(
      getName<T, U>(),
      Distribution::numberOfParameters(T) +
          DistributionMethod::numberOfParameters(U),
      &DistributionDispatcher::hook<T, U>);
}

struct NormCDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Normal, DistributionMethod::Type::CDF>();
};
struct NormCDFRange {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Normal,
                 DistributionMethod::Type::CDFRange>();
};
struct NormPDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Normal, DistributionMethod::Type::PDF>();
};
struct InvNorm {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Normal,
                 DistributionMethod::Type::Inverse>();
};

struct StudentCDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Student, DistributionMethod::Type::CDF>();
};
struct StudentCDFRange {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Student,
                 DistributionMethod::Type::CDFRange>();
};
struct StudentPDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Student, DistributionMethod::Type::PDF>();
};
struct InvStudent {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Student,
                 DistributionMethod::Type::Inverse>();
};

struct PoissonCDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Poisson, DistributionMethod::Type::CDF>();
};
struct PoissonPDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Poisson, DistributionMethod::Type::PDF>();
};

struct BinomCDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Binomial, DistributionMethod::Type::CDF>();
};
struct BinomPDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Binomial, DistributionMethod::Type::PDF>();
};
struct InvBinom {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Binomial,
                 DistributionMethod::Type::Inverse>();
};

struct GeomCDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Geometric,
                 DistributionMethod::Type::CDF>();
};
struct GeomCDFRange {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Geometric,
                 DistributionMethod::Type::CDFRange>();
};
struct GeomPDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Geometric,
                 DistributionMethod::Type::PDF>();
};
struct InvGeom {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Geometric,
                 DistributionMethod::Type::Inverse>();
};

struct HypergeomCDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Hypergeometric,
                 DistributionMethod::Type::CDF>();
};
struct HypergeomCDFRange {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Hypergeometric,
                 DistributionMethod::Type::CDFRange>();
};
struct HypergeomPDF {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Hypergeometric,
                 DistributionMethod::Type::PDF>();
};
struct InvHypergeom {
  constexpr static OExpression::FunctionHelper s_functionHelper =
      makeHelper<Distribution::Type::Hypergeometric,
                 DistributionMethod::Type::Inverse>();
};

}  // namespace Poincare

#endif
