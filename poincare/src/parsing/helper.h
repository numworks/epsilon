#ifndef POINCARE_PARSING_HELPER_H
#define POINCARE_PARSING_HELPER_H

#include <poincare_expressions.h>

namespace Poincare {

class ParsingHelper {

public:
  /* The method GetReservedFunction passes through the successive
   * entries of the reserved functions array in order to determine
   * whether a token corresponds to an entry. The static constexpr
   * s_reservedFunctionsUpperBound marks the end of the array. */
  static const Expression::FunctionHelper * const * GetReservedFunction(const char * name, size_t nameLength);
  static const Expression::FunctionHelper * const * ReservedFunctionsUpperBound() { return s_reservedFunctionsUpperBound; }
  static bool IsSpecialIdentifierName(const char * name, size_t nameLength);
  static bool IsParameteredExpression(const Expression::FunctionHelper * helper);
  static Expression ParameteredExpressionParameter(const char * text);

private:
  constexpr static const char * s_specialIdentifierNames[] = {
    /* This MUST be ordered according to name otherwise IsSpecialIdentifier
     * won't work properly. */
    Symbol::k_ans,
    Symbol::k_ansLowerCase,
    Infinity::Name(),
    Nonreal::Name(),
    Undefined::Name()
  };
  constexpr static int k_numberOfSpecialIdentifiers = sizeof(s_specialIdentifierNames) / sizeof(const char *);

   // The array of reserved functions' helpers
  constexpr static const Expression::FunctionHelper * s_reservedFunctions[] = {
    /* This MUST be ordered according to name, and then by numberOfChildren
     * otherwise GetReservedFunction won't work properly.
     * (name = s_functionHelper.name()) */
    &AbsoluteValue::s_functionHelper,
    &ArcCosine::s_functionHelper,
    &HyperbolicArcCosine::s_functionHelper,
    &ArcCotangent::s_functionHelper,
    &ArcCosecant::s_functionHelper,
    &ComplexArgument::s_functionHelper,
    &ArcSecant::s_functionHelper,
    &ArcSine::s_functionHelper,
    &HyperbolicArcSine::s_functionHelper,
    &ArcTangent::s_functionHelper,
    &HyperbolicArcTangent::s_functionHelper,
    &BinomCDF::s_functionHelper,
    &BinomialCoefficient::s_functionHelper,
    &BinomPDF::s_functionHelper,
    &Ceiling::s_functionHelper,
    &Conjugate::s_functionHelper,
    &Cosine::s_functionHelper,
    &HyperbolicCosine::s_functionHelper,
    &Cotangent::s_functionHelper,
    &VectorCross::s_functionHelper,
    &Cosecant::s_functionHelper,
    &Dependency::s_functionHelper,
    &Determinant::s_functionHelper,
    &Derivative::s_functionHelper,
    &Dimension::s_functionHelper,
    &VectorDot::s_functionHelper,
    &Factor::s_functionHelper,
    &Floor::s_functionHelper,
    &FracPart::s_functionHelper,
    &GreatCommonDivisor::s_functionHelper,
    &GeomCDF::s_functionHelper,
    &GeomCDFRange::s_functionHelper,
    &GeomPDF::s_functionHelper,
    &MatrixIdentity::s_functionHelper,
    &ImaginaryPart::s_functionHelper,
    &Integral::s_functionHelper,
    &InvBinom::s_functionHelper,
    &MatrixInverse::s_functionHelper,
    &InvGeom::s_functionHelper,
    &InvNorm::s_functionHelper,
    &InvStudent::s_functionHelper,
    &LeastCommonMultiple::s_functionHelper,
    &NaperianLogarithm::s_functionHelper,
    &CommonLogarithm::s_functionHelper,
    &Logarithm::s_functionHelper,
    &ListMaximum::s_functionHelper,
    &ListMean::s_functionHelperOneChild,
    &ListMean::s_functionHelperTwoChildren,
    &ListMedian::s_functionHelperOneChild,
    &ListMedian::s_functionHelperTwoChildren,
    &ListMinimum::s_functionHelper,
    &VectorNorm::s_functionHelper,
    &NormCDF::s_functionHelper,
    &NormCDFRange::s_functionHelper,
    &NormPDF::s_functionHelper,
    &PermuteCoefficient::s_functionHelper,
    &PoissonCDF::s_functionHelper,
    &PoissonPDF::s_functionHelper,
    &ListProduct::s_functionHelper,
    &Product::s_functionHelper,
    &DivisionQuotient::s_functionHelper,
    &Randint::s_functionHelper,
    &Random::s_functionHelper,
    &RealPart::s_functionHelper,
    &MatrixRowEchelonForm::s_functionHelper,
    &DivisionRemainder::s_functionHelper,
    &NthRoot::s_functionHelper,
    &Round::s_functionHelper,
    &MatrixReducedRowEchelonForm::s_functionHelper,
    &ListSampleStandardDeviation::s_functionHelperOneChild,
    &ListSampleStandardDeviation::s_functionHelperTwoChildren,
    &Secant::s_functionHelper,
    &ListSequence::s_functionHelper,
    &SignFunction::s_functionHelper,
    &Sine::s_functionHelper,
    &HyperbolicSine::s_functionHelper,
    &ListSort::s_functionHelper,
    &ListStandardDeviation::s_functionHelperOneChild,
    &ListStandardDeviation::s_functionHelperTwoChildren,
    &ListSum::s_functionHelper,
    &Sum::s_functionHelper,
    &Tangent::s_functionHelper,
    &HyperbolicTangent::s_functionHelper,
    &StudentCDF::s_functionHelper,
    &StudentCDFRange::s_functionHelper,
    &StudentPDF::s_functionHelper,
    &MatrixTrace::s_functionHelper,
    &MatrixTranspose::s_functionHelper,
    &ListVariance::s_functionHelperOneChild,
    &ListVariance::s_functionHelperTwoChildren,
    &SquareRoot::s_functionHelper,
  };

constexpr static const Expression::FunctionHelper * const * s_reservedFunctionsUpperBound = s_reservedFunctions + (sizeof(s_reservedFunctions)/sizeof(Expression::FunctionHelper *));

};

}

#endif
