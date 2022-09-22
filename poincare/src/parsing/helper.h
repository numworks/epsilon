#ifndef POINCARE_PARSING_HELPER_H
#define POINCARE_PARSING_HELPER_H

#include <poincare_expressions.h>
#include "token.h"

namespace Poincare {

class ParsingHelper {

public:
  /* The method GetReservedFunction passes through the successive
   * entries of the reserved functions array in order to determine
   * whether a token corresponds to an entry. The constexpr static
   * s_reservedFunctionsUpperBound marks the end of the array. */
  static const Expression::FunctionHelper * const * GetReservedFunction(const char * name, size_t nameLength);
  static const Expression::FunctionHelper * const * GetInverseFunction(const char * name, size_t nameLength);
  static const Expression::FunctionHelper * const * ReservedFunctionsUpperBound() { return s_reservedFunctionsUpperBound; }
  static bool IsSpecialIdentifierName(const char * name, size_t nameLength);
  static bool IsLogicalOperator(const char * name, size_t nameLength, Token::Type * returnType);
  static bool IsParameteredExpression(const Expression::FunctionHelper * helper);
  // True if f^2(x) = f(x)^2 (ex: cos^2(x) = cos(x)^2 but ln^2(x) != ln(x)^2)
  static bool IsSquarableFunction(const Expression::FunctionHelper * helper);

  // This must be called with an identifier name
  typedef Expression (*IdentifierBuilder) ();
  static const IdentifierBuilder GetIdentifierBuilder(const char * name, size_t nameLength);

private:
  // The array of special identifiers
  struct SpecialIdentifier {
    AliasesList identifierAliasesList;
    IdentifierBuilder identifierBuilder;
  };
  constexpr static SpecialIdentifier s_specialIdentifiers[] = {
    {Infinity::k_infinityAliases, [] {return static_cast<Expression>(Infinity::Builder(false));}},
    {Symbol::k_ansAliases, [] {return static_cast<Expression>(Symbol::Ans());}},
    {BooleanNode::k_falseAliases, [] {return static_cast<Expression>(Boolean::Builder(false));}},
    {Nonreal::Name(), [] {return static_cast<Expression>(Nonreal::Builder());}},
    {BooleanNode::k_trueAliases, [] {return static_cast<Expression>(Boolean::Builder(true));}},
    {Undefined::Name(), [] {return static_cast<Expression>(Undefined::Builder());}}
  };
  constexpr static int k_numberOfSpecialIdentifiers = sizeof(s_specialIdentifiers) / sizeof(SpecialIdentifier);

  static int SpecialIdentifierIndexForName(const char * name, size_t nameLength);

  // The array of reserved functions' helpers
  constexpr static const Expression::FunctionHelper * s_reservedFunctions[] = {
    /* This MUST be ordered according to name, and then by numberOfChildren
     * otherwise GetReservedFunction won't work properly.
     * If the function has multiple aliases, take the first alias
     * in alphabetical order to choose position in list.
     *
     * Example:
     * { acos, arccos } -> take acos as reference for position
     * { asin, arcsin } -> take arcsin as reference for position
     * */
    &AbsoluteValue::s_functionHelper, // abs
    &ArcCosine::s_functionHelper, // acos, arccos
    &ArcCotangent::s_functionHelper, // arccot
    &ArcCosecant::s_functionHelper, // arccsc
    &HyperbolicArcCosine::s_functionHelper, //arcosh
    &ArcSecant::s_functionHelper, // arcsec
    &ArcSine::s_functionHelper, // arcsin, asin
    &ArcTangent::s_functionHelper, // arctan, atan
    &ComplexArgument::s_functionHelper, // arg
    &HyperbolicArcSine::s_functionHelper, // arsinh
    &HyperbolicArcTangent::s_functionHelper, // artanh
    &BinomCDF::s_functionHelper, // binomcdf
    &BinomialCoefficient::s_functionHelper, // binomial
    &BinomPDF::s_functionHelper, // binompdf
    &Ceiling::s_functionHelper, // ceil
    &Conjugate::s_functionHelper, // conj
    &Cosine::s_functionHelper, // cos
    &HyperbolicCosine::s_functionHelper, //cosh
    &Cotangent::s_functionHelper, // cot
    &VectorCross::s_functionHelper, // cross
    &Cosecant::s_functionHelper, // csc
    &Dependency::s_functionHelper, // dep
    &Determinant::s_functionHelper, // det
    &Derivative::s_functionHelperFirstOrder, // diff
    &Derivative::s_functionHelper, // diff
    &Dimension::s_functionHelper, // dim
    &VectorDot::s_functionHelper, // dot
    &Factor::s_functionHelper, // fact
    &Floor::s_functionHelper, // floor
    &FracPart::s_functionHelper, // frac
    &GreatCommonDivisor::s_functionHelper, // gcd
    &GeomCDF::s_functionHelper, // geomcdf
    &GeomCDFRange::s_functionHelper, //geomcdfrange
    &GeomPDF::s_functionHelper, // geompdf
    &HypergeomCDF::s_functionHelper, // hgeomcdf
    &HypergeomCDFRange::s_functionHelper, //hgeomcdfrange
    &HypergeomPDF::s_functionHelper, // hgeompdf
    &MatrixIdentity::s_functionHelper, // identity
    &ImaginaryPart::s_functionHelper, // im
    &Integral::s_functionHelper, // int
    &InvBinom::s_functionHelper, // invbinom
    &MatrixInverse::s_functionHelper, // inverse
    &InvGeom::s_functionHelper, // invgeom
    &InvHypergeom::s_functionHelper, // invhgeom
    &InvNorm::s_functionHelper, // invnorm
    &InvStudent::s_functionHelper, // invt
    &LeastCommonMultiple::s_functionHelper, // lcm
    &NaperianLogarithm::s_functionHelper, // ln
    &Logarithm::s_functionHelper, // log
    &ListMaximum::s_functionHelper, // max
    &ListMean::s_functionHelper, // mean
    &ListMedian::s_functionHelper, // med
    &ListMinimum::s_functionHelper, // min
    &VectorNorm::s_functionHelper, // norm
    &NormCDF::s_functionHelper, // normcdf
    &NormCDFRange::s_functionHelper, // normcdfrange
    &NormPDF::s_functionHelper, // normpdf
    &PermuteCoefficient::s_functionHelper, // permute
    &PiecewiseOperator::s_functionHelper, // piecewise
    &PoissonCDF::s_functionHelper, // poissoncdf
    &PoissonPDF::s_functionHelper, // poissonpdf
    &ListProduct::s_functionHelper, // prod
    &Product::s_functionHelper, // product
    &DivisionQuotient::s_functionHelper, // quo
    &Randint::s_functionHelper, // randint
    &Random::s_functionHelper, // random
    &RealPart::s_functionHelper, // re
    &MatrixRowEchelonForm::s_functionHelper, // ref
    &DivisionRemainder::s_functionHelper, // rem
    &NthRoot::s_functionHelper, // root
    &Round::s_functionHelper, // round
    &MatrixReducedRowEchelonForm::s_functionHelper, // rref
    &ListSampleStandardDeviation::s_functionHelper, // samplestddev
    &Secant::s_functionHelper, // sec
    &ListSequence::s_functionHelper, // sequence
    &SignFunction::s_functionHelper, // sign
    &Sine::s_functionHelper, // sin
    &HyperbolicSine::s_functionHelper, // sinh
    &ListSort::s_functionHelper, // sort
    &ListStandardDeviation::s_functionHelper, // stddev
    &ListSum::s_functionHelper, // sum (1 param)
    &Sum::s_functionHelper, // sum (4 params)
    &Tangent::s_functionHelper, // tan
    &HyperbolicTangent::s_functionHelper, // tanh
    &StudentCDF::s_functionHelper, // tcdf
    &StudentCDFRange::s_functionHelper, // tcdfrange
    &StudentPDF::s_functionHelper, // tpdf
    &MatrixTrace::s_functionHelper, // trace
    &MatrixTranspose::s_functionHelper, // transpose
    &ListVariance::s_functionHelper, // var
    &SquareRoot::s_functionHelper, // √
  };

  constexpr static const Expression::FunctionHelper * const * s_reservedFunctionsUpperBound = s_reservedFunctions + (sizeof(s_reservedFunctions)/sizeof(Expression::FunctionHelper *));

  // The array of functions that can be aliases to f^-1
  struct FunctionMapping { const Expression::FunctionHelper * mainFunction; const Expression::FunctionHelper * inverseFunction; };
  constexpr static const FunctionMapping s_inverses[] = {
    {&Cosine::s_functionHelper, &ArcCosine::s_functionHelper},
    {&Sine::s_functionHelper, &ArcSine::s_functionHelper},
    {&Tangent::s_functionHelper, &ArcTangent::s_functionHelper},
  };
  constexpr static int k_numberOfInverses = sizeof(s_inverses) / sizeof(FunctionMapping);
  constexpr static FunctionMapping const * s_inverseFunctionsUpperBound = s_inverses + (k_numberOfInverses);
};

}

#endif
