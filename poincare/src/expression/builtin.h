#ifndef POINCARE_EXPRESSION_BUILTINS_H
#define POINCARE_EXPRESSION_BUILTINS_H

#include <poincare/src/memory/type_block.h>

#include "aliases.h"

namespace Poincare::Internal {

class Tree;

// TODO: Reorganize this class to avoid the duplication of many methods.

class Builtin {
 public:
  constexpr Builtin(Type blockType, Aliases aliases)
      : m_blockType(blockType), m_aliases(aliases) {}

  constexpr Type blockType() const { return m_blockType; }
  constexpr const Aliases* aliases() const { return &m_aliases; }
  virtual bool has2DLayout() const { return false; }
  virtual Tree* pushNode(int numberOfChildren) const;
  virtual bool checkNumberOfParameters(int n) const;
  static bool IsReservedFunction(const Tree* tree) {
    return GetReservedFunction(tree) != nullptr;
  }
  static Aliases ReservedFunctionName(const Tree* tree) {
    assert(GetReservedFunction(tree));
    return GetReservedFunction(tree)->m_aliases;
  }
  static Aliases SpecialIdentifierName(Type type) {
    assert(GetSpecialIdentifier(type));
    return GetSpecialIdentifier(type)->m_aliases;
  }
  static bool HasReservedFunction(UnicodeDecoder* name) {
    return GetReservedFunction(name) != nullptr;
  }
  static bool HasSpecialIdentifier(UnicodeDecoder* name) {
    return GetSpecialIdentifier(name) != nullptr;
  }
  static bool HasSpecialIdentifier(Type type) {
    return GetSpecialIdentifier(type) != nullptr;
  }
  static bool HasCustomIdentifier(UnicodeDecoder* name);
  static const Builtin* GetReservedFunction(UnicodeDecoder* name);
  static const Builtin* GetReservedFunction(const Tree* tree);
  static constexpr const Builtin* GetReservedFunction(Type type);
  static const Builtin* GetSpecialIdentifier(UnicodeDecoder* name);
  static const Builtin* GetSpecialIdentifier(Type type);

 private:
  Type m_blockType;
  Aliases m_aliases;
};

class BuiltinWithLayout : public Builtin {
 public:
  constexpr BuiltinWithLayout(Type blockType, Aliases aliases,
                              LayoutType layoutType)
      : Builtin(blockType, aliases), m_layoutType(layoutType) {}
  LayoutType layoutType() const { return m_layoutType; }
  bool has2DLayout() const override { return true; }

  static constexpr const BuiltinWithLayout* GetReservedFunction(
      LayoutType layoutType);

 private:
  LayoutType m_layoutType;
};

class BuiltinAns : public Builtin {
  using Builtin::Builtin;
  bool checkNumberOfParameters(int n) const override { return n == 0; }
  Tree* pushNode(int numberOfChildren) const override;
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
    {Type::Cos, "cos"},
    {Type::Sin, "sin"},
    {Type::Tan, "tan"},
    {Type::ACos, BuiltinsAliases::k_acosAliases},
    {Type::ASin, BuiltinsAliases::k_asinAliases},
    {Type::ATan, BuiltinsAliases::k_atanAliases},
    {Type::Sec, "sec"},
    {Type::Csc, "csc"},
    {Type::Cot, "cot"},
    {Type::ASec, "arcsec"},
    {Type::ACsc, "arccsc"},
    {Type::ACot, "arccot"},
    {Type::CosH, "cosh"},
    {Type::SinH, "sinh"},
    {Type::TanH, "tanh"},
    {Type::ArCosH, "arcosh"},
    {Type::ArSinH, "arsinh"},
    {Type::ArTanH, "artanh"},
    {Type::Exp, "exp"},
    {Type::Logarithm, "log"},
    {Type::Log, "log"},
    {Type::Ln, "ln"},
    {Type::Cross, "cross"},
    {Type::Det, "det"},
    {Type::Dim, "dim"},
    {Type::Dot, "dot"},
    {Type::Identity, "identity"},
    {Type::Inverse, "inverse"},
    {Type::Ref, "ref"},
    {Type::Rref, "rref"},
    {Type::Trace, "trace"},
    {Type::Transpose, "transpose"},
    {Type::Arg, "arg"},
    {Type::Re, "re"},
    {Type::Im, "im"},
    {Type::GCD, "gcd"},
    {Type::LCM, "lcm"},
    {Type::Quo, "quo"},
    {Type::Rem, "rem"},
    {Type::Factor, "factor"},
    {Type::Frac, "frac"},
    {Type::Round, "round"},
    {Type::Sign, "sign"},
    {Type::Mean, "mean"},
    {Type::StdDev, "stddev"},
    {Type::Median, "med"},
    {Type::Variance, "var"},
    {Type::SampleStdDev, "samplestddev"},
    {Type::Min, "min"},
    {Type::Max, "max"},
    {Type::ListSum, "sum"},
    {Type::ListProduct, "prod"},
    {Type::ListSort, "sort"},
    {Type::Permute, "permute"},
    {Type::Random, "random"},
    {Type::RandInt, "randint"},
    {Type::RandIntNoRep, "randintnorep"},
    {Type::Diff, "diff"},            // 2D layout is special
    {Type::NthDiff, "diff"},         // 2D layout is special
    {Type::Piecewise, "piecewise"},  // TODO_PCJ 2D layout is a grid
    {Type::Dependency, "dep"},       // TODO dummy
};

constexpr static BuiltinWithLayout s_builtinsWithLayout[] = {
    {Type::Abs, "abs", LayoutType::Abs},
    {Type::Binomial, "binomial", LayoutType::Binomial},
    {Type::Sum, "sum", LayoutType::Sum},
    {Type::Product, "product", LayoutType::Product},
    {Type::Integral, "int", LayoutType::Integral},
    {Type::Sqrt, BuiltinsAliases::k_squareRootAliases, LayoutType::Sqrt},
    {Type::Root, "root", LayoutType::Root},
    {Type::Norm, "norm", LayoutType::VectorNorm},
    {Type::Conj, "conj", LayoutType::Conj},
    {Type::Ceil, "ceil", LayoutType::Ceil},
    {Type::Floor, "floor", LayoutType::Floor},
    {Type::ListSequence, "sequence", LayoutType::ListSequence},
};

constexpr const Builtin* Builtin::GetReservedFunction(Type type) {
  for (const Builtin& builtin : s_builtins) {
    if (builtin.m_blockType == type) {
      return &builtin;
    }
  }
  for (const Builtin& builtin : s_builtinsWithLayout) {
    if (builtin.m_blockType == type) {
      return &builtin;
    }
  }
  return nullptr;
}

constexpr const BuiltinWithLayout* BuiltinWithLayout::GetReservedFunction(
    LayoutType LayoutType) {
  for (const BuiltinWithLayout& builtin : s_builtinsWithLayout) {
    if (builtin.m_layoutType == LayoutType) {
      return &builtin;
    }
  }
  return nullptr;
}

}  // namespace Poincare::Internal

#endif
