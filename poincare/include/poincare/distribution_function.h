#ifndef POINCARE_DISTRIBUTION_FUNCTION_H
#define POINCARE_DISTRIBUTION_FUNCTION_H

#include <poincare/distribution.h>
#include <poincare/distribution_method.h>
#include <poincare/n_ary_expression.h>

namespace Poincare {

template<Distribution::Type T, DistributionMethod::Type U>
constexpr const char * getName();

template<> constexpr const char * getName<Distribution::Type::Normal, DistributionMethod::Type::CDF>() { return "normcdf"; }
template<> constexpr const char * getName<Distribution::Type::Normal, DistributionMethod::Type::CDFRange>() { return "normcdfrange"; }
template<> constexpr const char * getName<Distribution::Type::Normal, DistributionMethod::Type::PDF>() { return "normpdf"; }
template<> constexpr const char * getName<Distribution::Type::Normal, DistributionMethod::Type::Inverse>() { return "invnorm"; }

template<> constexpr const char * getName<Distribution::Type::Student, DistributionMethod::Type::CDF>() { return "tcdf"; }
template<> constexpr const char * getName<Distribution::Type::Student, DistributionMethod::Type::CDFRange>() { return "tcdfrange"; }
template<> constexpr const char * getName<Distribution::Type::Student, DistributionMethod::Type::PDF>() { return "tpdf"; }
template<> constexpr const char * getName<Distribution::Type::Student, DistributionMethod::Type::Inverse>() { return "invt"; }

template<> constexpr const char * getName<Distribution::Type::Binomial, DistributionMethod::Type::CDF>() { return "binomcdf"; }
template<> constexpr const char * getName<Distribution::Type::Binomial, DistributionMethod::Type::PDF>() { return "binompdf"; }
template<> constexpr const char * getName<Distribution::Type::Binomial, DistributionMethod::Type::Inverse>() { return "invbinom"; }

template<> constexpr const char * getName<Distribution::Type::Poisson, DistributionMethod::Type::CDF>() { return "poissoncdf"; }
template<> constexpr const char * getName<Distribution::Type::Poisson, DistributionMethod::Type::PDF>() { return "poissonpdf"; }

template<> constexpr const char * getName<Distribution::Type::Geometric, DistributionMethod::Type::CDF>() { return "geomcdf"; }
template<> constexpr const char * getName<Distribution::Type::Geometric, DistributionMethod::Type::CDFRange>() { return "geomcdfrange"; }
template<> constexpr const char * getName<Distribution::Type::Geometric, DistributionMethod::Type::PDF>() { return "geompdf"; }
template<> constexpr const char * getName<Distribution::Type::Geometric, DistributionMethod::Type::Inverse>() { return "invgeom"; }

class DistributionFunctionNode : public NAryExpressionNode {
public:
  // Simplication
  size_t size() const override { return sizeof(DistributionFunctionNode); }

  const char * name() const {
    return m_name;
  }

  // Properties
  Type type() const override {
    return Type::DistributionFunction;
    // TODO: do we need more precise info ?
    // for example to simplify two binomial expressions together
  }
  Sign sign(Context * context) const override {
    if (m_functionType != DistributionMethod::Type::Inverse) {
      return Sign::Positive;
    }
    return Sign::Unknown;
  }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
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
    }
    switch (m_functionType) {
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

private:
  Expression shallowReduce(ReductionContext reductionContext) override;
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;

  friend class DistributionFunction;
  void setType(DistributionMethod::Type f) {
    m_functionType = f;
  }
  void setDistribution(Distribution::Type d) {
    m_distributionType = d;
  }
  void setName(const char * name) {
    m_name = name;
  }
private:
  const char * m_name;
  DistributionMethod::Type m_functionType;
  Distribution::Type m_distributionType;
};

class DistributionFunction final : public NAryExpression {
public:
  DistributionFunction(const DistributionFunctionNode * n) : NAryExpression(n) {}
  static DistributionFunction Builder(const Tuple & children = {}) { return TreeHandle::NAryBuilder<DistributionFunction, DistributionFunctionNode>(convert(children)); }

  template <Distribution::Type T, DistributionMethod::Type U>
  static Expression hook(Expression children) {
    Expression exp = UntypedBuilderMultipleChildren<DistributionFunction>(children);
    DistributionFunction dist = exp.convert<DistributionFunction>();
    dist.setDistribution(T);
    dist.setType(U);
    dist.setName(getName<T,U>());
    return dist;
  }

  Expression shallowReduce(Context * context, bool * stopReduction = nullptr);
  void setType(DistributionMethod::Type f) {
    static_cast<DistributionFunctionNode*>(node())->setType(f);
  }
  void setDistribution(Distribution::Type d) {
    static_cast<DistributionFunctionNode*>(node())->setDistribution(d);
  }
  void setName(const char * name) {
    static_cast<DistributionFunctionNode*>(node())->setName(name);
  }

  Distribution::Type distributionType() {
    return static_cast<DistributionFunctionNode*>(node())->m_distributionType;
  }

  DistributionMethod::Type functionType() {
    return static_cast<DistributionFunctionNode*>(node())->m_functionType;
  }
};

template <Distribution::Type T, DistributionMethod::Type U>
constexpr Expression::FunctionHelper makeHelper() {
  return Expression::FunctionHelper(
    getName<T,U>(),
    Distribution::numberOfParameters(T) + DistributionMethod::numberOfParameters(U),
    &DistributionFunction::hook<T, U>);
}

struct NormCDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Normal, DistributionMethod::Type::CDF>(); };
struct NormCDFRange { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Normal, DistributionMethod::Type::CDFRange>(); };
struct NormPDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Normal, DistributionMethod::Type::PDF>(); };
struct InvNorm      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Normal, DistributionMethod::Type::Inverse>(); };

struct StudentCDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Student, DistributionMethod::Type::CDF>(); };
struct StudentCDFRange { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Student, DistributionMethod::Type::CDFRange>(); };
struct StudentPDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Student, DistributionMethod::Type::PDF>(); };
struct InvStudent      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Student, DistributionMethod::Type::Inverse>(); };

struct PoissonCDF { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Poisson, DistributionMethod::Type::CDF>(); };
struct PoissonPDF { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Poisson, DistributionMethod::Type::PDF>(); };

struct BinomCDF { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Binomial, DistributionMethod::Type::CDF>(); };
struct BinomPDF { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Binomial, DistributionMethod::Type::PDF>(); };
struct InvBinom { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Binomial, DistributionMethod::Type::Inverse>(); };

struct GeomCDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Geometric, DistributionMethod::Type::CDF>(); };
struct GeomCDFRange { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Geometric, DistributionMethod::Type::CDFRange>(); };
struct GeomPDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Geometric, DistributionMethod::Type::PDF>(); };
struct InvGeom      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<Distribution::Type::Geometric, DistributionMethod::Type::Inverse>(); };

}

#endif
