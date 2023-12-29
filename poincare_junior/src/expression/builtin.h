#ifndef POINCARE_EXPRESSION_BUILTINS_H
#define POINCARE_EXPRESSION_BUILTINS_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "aliases.h"

namespace PoincareJ {

// TODO: Reorganize this class to avoid the duplication of many methods.

class Builtin {
 public:
  constexpr Builtin(BlockType blockType, Aliases aliases)
      : m_blockType(blockType), m_aliases(aliases) {}

  const BlockType blockType() const { return m_blockType; }
  const Aliases* aliases() const { return &m_aliases; }
  virtual Tree* pushNode(int numberOfChildren) const;
  virtual bool checkNumberOfParameters(int n) const;
  static bool IsReservedFunction(const Tree* tree) {
    return GetReservedFunction(tree) != nullptr;
  }
  static Aliases ReservedFunctionName(const Tree* tree) {
    assert(GetReservedFunction(tree));
    return GetReservedFunction(tree)->m_aliases;
  }
  static Aliases SpecialIdentifierName(BlockType type) {
    assert(GetSpecialIdentifier(type));
    return GetSpecialIdentifier(type)->m_aliases;
  }
  static bool HasReservedFunction(UnicodeDecoder* name) {
    return GetReservedFunction(name) != nullptr;
  }
  static bool HasSpecialIdentifier(UnicodeDecoder* name) {
    return GetSpecialIdentifier(name) != nullptr;
  }
  static bool HasCustomIdentifier(UnicodeDecoder* name);
  static const Builtin* GetReservedFunction(UnicodeDecoder* name);
  static const Builtin* GetReservedFunction(const Tree* tree);
  static constexpr const Builtin* GetReservedFunction(BlockType type);
  static const Builtin* GetSpecialIdentifier(UnicodeDecoder* name);
  static const Builtin* GetSpecialIdentifier(BlockType type);
  static bool Promote(Tree* parameterList, const Builtin* builtin);
  EDITION_REF_WRAP_1(Promote, const Builtin*);

 private:
  BlockType m_blockType;
  Aliases m_aliases;
};

namespace BuiltinsAliases {
// Special identifiers
constexpr static Aliases k_ansAliases = "\01Ans\00ans\00";
constexpr static Aliases k_trueAliases = "\01True\00true\00";
constexpr static Aliases k_falseAliases = "\01False\00false\00";
constexpr static Aliases k_infinityAliases = "\01∞\00inf\00";
// Constants
constexpr static Aliases k_piAliases = "\01π\00pi\00";
// Symbols
constexpr static Aliases k_thetaAliases = "\01θ\00theta\00";
// Units
constexpr static Aliases k_litersAliases = "\01L\00l\00";
// Inverse trigo
constexpr static Aliases k_acosAliases = "\01arccos\00acos\00";
constexpr static Aliases k_asinAliases = "\01arcsin\00asin\00";
constexpr static Aliases k_atanAliases = "\01arctan\00atan\00";
// Other functions
constexpr static Aliases k_squareRootAliases = "\01√\00sqrt\00";
}  // namespace BuiltinsAliases

constexpr static Builtin s_builtins[] = {
    {BlockType::Abs, "abs"},
    {BlockType::Cosine, "cos"},
    {BlockType::Sine, "sin"},
    {BlockType::Tangent, "tan"},
    {BlockType::ArcCosine, BuiltinsAliases::k_acosAliases},
    {BlockType::ArcSine, BuiltinsAliases::k_asinAliases},
    {BlockType::ArcTangent, BuiltinsAliases::k_atanAliases},
    {BlockType::Secant, "sec"},
    {BlockType::Cosecant, "csc"},
    {BlockType::Cotangent, "cot"},
    {BlockType::ArcSecant, "arcsec"},
    {BlockType::ArcCosecant, "arccsc"},
    {BlockType::ArcCotangent, "arccot"},
    {BlockType::HyperbolicCosine, "cosh"},
    {BlockType::HyperbolicSine, "sinh"},
    {BlockType::HyperbolicTangent, "tanh"},
    {BlockType::HyperbolicArcCosine, "arcosh"},
    {BlockType::HyperbolicArcSine, "arsinh"},
    {BlockType::HyperbolicArcTangent, "artanh"},
    {BlockType::Sum, "sum"},
    {BlockType::Product, "product"},
    {BlockType::Derivative, "diff"},
    {BlockType::Integral, "int"},
    {BlockType::Logarithm, "log"},
    {BlockType::Log, "log"},
    {BlockType::Ln, "ln"},
    {BlockType::SquareRoot, BuiltinsAliases::k_squareRootAliases},
    {BlockType::NthRoot, "root"},
    {BlockType::Cross, "cross"},
    {BlockType::Det, "det"},
    {BlockType::Dim, "dim"},
    {BlockType::Dot, "dot"},
    {BlockType::Identity, "identity"},
    {BlockType::Inverse, "inverse"},
    {BlockType::Norm, "norm"},
    {BlockType::Ref, "ref"},
    {BlockType::Rref, "rref"},
    {BlockType::Trace, "trace"},
    {BlockType::Transpose, "transpose"},
    {BlockType::ComplexArgument, "arg"},
    {BlockType::RealPart, "re"},
    {BlockType::ImaginaryPart, "im"},
    {BlockType::Conjugate, "conj"},
    {BlockType::Dependency, "dep"},
    {BlockType::GCD, "gcd"},
    {BlockType::LCM, "lcm"},
    {BlockType::Quotient, "quo"},
    {BlockType::Remainder, "rem"},
    {BlockType::Factor, "factor"},
    {BlockType::Ceiling, "ceil"},
    {BlockType::Floor, "floor"},
    {BlockType::FracPart, "frac"},
    {BlockType::Round, "round"},
    {BlockType::Sign, "sign"},
    {BlockType::ListSequence, "sequence"},
    {BlockType::Mean, "mean"},
    {BlockType::StdDev, "stddev"},
    {BlockType::Median, "med"},
    {BlockType::Variance, "var"},
    {BlockType::SampleStdDev, "samplestddev"},
    {BlockType::Minimum, "min"},
    {BlockType::Maximum, "max"},
    {BlockType::ListSum, "sum"},
    {BlockType::ListProduct, "prod"},
    {BlockType::ListSort, "sort"},
    {BlockType::Binomial, "binomial"},
    {BlockType::Permute, "permute"},
    {BlockType::Random, "random"},
    {BlockType::RandInt, "randint"},
    {BlockType::RandIntNoRep, "randintnorep"},
    {BlockType::Piecewise, "piecewise"},
};

constexpr const Builtin* Builtin::GetReservedFunction(BlockType type) {
  for (const Builtin& builtin : s_builtins) {
    if (builtin.m_blockType == type) {
      return &builtin;
    }
  }
  return nullptr;
}

}  // namespace PoincareJ

#endif
