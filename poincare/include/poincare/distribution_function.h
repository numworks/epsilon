#ifndef POINCARE_DISTRIBUTION_FUNCTION_H
#define POINCARE_DISTRIBUTION_FUNCTION_H

#include <poincare/expression.h>
#include <poincare/n_ary_expression.h>

namespace Poincare {

enum class FunctionType {
  PDF,
  CDF,
  CDFRange,
  Inverse,
};

enum class DistributionType {
  Normal,
  Student,
  Binomial,
  Poisson,
  Geometric,
};

template<DistributionType T, FunctionType U>
constexpr const char * getName();

template<> constexpr const char * getName<DistributionType::Normal, FunctionType::CDF>() { return "normcdf"; }
template<> constexpr const char * getName<DistributionType::Normal, FunctionType::CDFRange>() { return "normcdfrange"; }
template<> constexpr const char * getName<DistributionType::Normal, FunctionType::PDF>() { return "normpdf"; }
template<> constexpr const char * getName<DistributionType::Normal, FunctionType::Inverse>() { return "invnorm"; }

template<> constexpr const char * getName<DistributionType::Student, FunctionType::CDF>() { return "tcdf"; }
template<> constexpr const char * getName<DistributionType::Student, FunctionType::CDFRange>() { return "tcdfrange"; }
template<> constexpr const char * getName<DistributionType::Student, FunctionType::PDF>() { return "tpdf"; }
template<> constexpr const char * getName<DistributionType::Student, FunctionType::Inverse>() { return "invt"; }

template<> constexpr const char * getName<DistributionType::Binomial, FunctionType::CDF>() { return "binomcdf"; }
template<> constexpr const char * getName<DistributionType::Binomial, FunctionType::PDF>() { return "binompdf"; }
template<> constexpr const char * getName<DistributionType::Binomial, FunctionType::Inverse>() { return "invbinom"; }

template<> constexpr const char * getName<DistributionType::Poisson, FunctionType::CDF>() { return "poissoncdf"; }
template<> constexpr const char * getName<DistributionType::Poisson, FunctionType::PDF>() { return "poissonpdf"; }

template<> constexpr const char * getName<DistributionType::Geometric, FunctionType::CDF>() { return "geomcdf"; }
template<> constexpr const char * getName<DistributionType::Geometric, FunctionType::CDFRange>() { return "geomcdfrange"; }
template<> constexpr const char * getName<DistributionType::Geometric, FunctionType::PDF>() { return "geompdf"; }
template<> constexpr const char * getName<DistributionType::Geometric, FunctionType::Inverse>() { return "invgeom"; }



static constexpr int numberOfParameters(FunctionType f) {
  switch(f) {
  case FunctionType::PDF:
  case FunctionType::CDF:
  case FunctionType::Inverse:
    return 1;
  case FunctionType::CDFRange:
    return 2;
  }
}

static constexpr int numberOfParameters(DistributionType d) {
  switch (d) {
  case DistributionType::Student:
  case DistributionType::Poisson:
  case DistributionType::Geometric:
    return 1;
  case DistributionType::Normal:
  case DistributionType::Binomial:
    return 2;
  }
}


class DistributionFunctionNode : public NAryExpressionNode {
public:
  // Simplication
  size_t size() const override { return sizeof(DistributionFunctionNode); }
  // int numberOfChildren() const override {
    // return numberOfParameters(m_functionType) + numberOfParameters(m_distributionType);
  // }

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
    if (m_functionType != FunctionType::Inverse) {
      return Sign::Positive;
    }
    return Sign::Unknown;
  }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    switch (m_distributionType) {
    case DistributionType::Normal:
      stream << "Norm";
      break;
    case DistributionType::Student:
      stream << "Student";
      break;
    case DistributionType::Binomial:
      stream << "Binom";
      break;
    case DistributionType::Poisson:
      stream << "Poisson";
      break;
    case DistributionType::Geometric:
      stream << "Geom";
      break;
    }
    switch (m_functionType) {
    case FunctionType::PDF:
      stream << "PDF";
      break;
    case FunctionType::CDF:
      stream << "CDF";
      break;
    case FunctionType::CDFRange:
      stream << "CDFRange";
      break;
    case FunctionType::Inverse:
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
  void setType(FunctionType f) {
    m_functionType = f;
  }
  void setDistribution(DistributionType d) {
    m_distributionType = d;
  }
  void setName(const char * name) {
    m_name = name;
  }
private:
  const char * m_name;
  FunctionType m_functionType;
  DistributionType m_distributionType;
};

class DistributionFunction final : public NAryExpression {
public:
  DistributionFunction(const DistributionFunctionNode * n) : NAryExpression(n) {}
  static DistributionFunction Builder(const Tuple & children = {}) { return TreeHandle::NAryBuilder<DistributionFunction, DistributionFunctionNode>(convert(children)); }

  template <DistributionType T, FunctionType U>
  static Expression hook(Expression children) {
    Expression exp = UntypedBuilderMultipleChildren<DistributionFunction>(children);
    DistributionFunction dist = exp.convert<DistributionFunction>();
    dist.setDistribution(T);
    dist.setType(U);
    dist.setName(getName<T,U>());
    return dist;
  }

  Expression shallowReduce(Context * context, bool * stopReduction = nullptr);
  void setType(FunctionType f) {
    static_cast<DistributionFunctionNode*>(node())->setType(f);
  }
  void setDistribution(DistributionType d) {
    static_cast<DistributionFunctionNode*>(node())->setDistribution(d);
  }
  void setName(const char * name) {
    static_cast<DistributionFunctionNode*>(node())->setName(name);
  }

  DistributionType distributionType() {
    return static_cast<DistributionFunctionNode*>(node())->m_distributionType;
  }

  FunctionType functionType() {
    return static_cast<DistributionFunctionNode*>(node())->m_functionType;
  }
};

template <DistributionType T, FunctionType U>
constexpr Expression::FunctionHelper makeHelper() {
  return Expression::FunctionHelper(
    getName<T,U>(),
    numberOfParameters(T) + numberOfParameters(U),
    &DistributionFunction::hook<T, U>);
}

struct NormCDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Normal, FunctionType::CDF>(); };
struct NormCDFRange { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Normal, FunctionType::CDFRange>(); };
struct NormPDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Normal, FunctionType::PDF>(); };
struct InvNorm      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Normal, FunctionType::Inverse>(); };

struct StudentCDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Student, FunctionType::CDF>(); };
struct StudentCDFRange { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Student, FunctionType::CDFRange>(); };
struct StudentPDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Student, FunctionType::PDF>(); };
struct InvStudent      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Student, FunctionType::Inverse>(); };

struct PoissonCDF { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Poisson, FunctionType::CDF>(); };
struct PoissonPDF { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Poisson, FunctionType::PDF>(); };

struct BinomCDF { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Binomial, FunctionType::CDF>(); };
struct BinomPDF { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Binomial, FunctionType::PDF>(); };
struct InvBinom { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Binomial, FunctionType::Inverse>(); };

struct GeomCDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Geometric, FunctionType::CDF>(); };
struct GeomCDFRange { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Geometric, FunctionType::CDFRange>(); };
struct GeomPDF      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Geometric, FunctionType::PDF>(); };
struct InvGeom      { static constexpr Expression::FunctionHelper s_functionHelper = makeHelper<DistributionType::Geometric, FunctionType::Inverse>(); };

}

#endif
