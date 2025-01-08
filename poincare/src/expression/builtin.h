#ifndef POINCARE_EXPRESSION_BUILTINS_H
#define POINCARE_EXPRESSION_BUILTINS_H

#include <poincare/src/layout/layout_span_decoder.h>
#include <poincare/src/memory/type_block.h>

#include "aliases.h"

namespace Poincare::Internal {

class Tree;

// TODO: Reorganize this class to avoid the duplication of many methods.

class Builtin {
 public:
  constexpr Builtin(EnabledType type, Aliases aliases)
      : m_type(type), m_aliases(aliases) {}

  constexpr TypeBlock type() const { return m_type; }
  constexpr const Aliases* aliases() const { return &m_aliases; }
  virtual bool has2DLayout() const { return false; }
  virtual Tree* pushNode(int numberOfChildren) const;
  virtual bool checkNumberOfParameters(int n) const;
  static bool IsReservedFunction(const Tree* e) {
    return GetReservedFunction(e) != nullptr;
  }
  static Aliases ReservedFunctionName(const Tree* e) {
    assert(GetReservedFunction(e));
    return GetReservedFunction(e)->m_aliases;
  }
  static Aliases SpecialIdentifierName(EnabledType type) {
    assert(GetSpecialIdentifier(type));
    return GetSpecialIdentifier(type)->m_aliases;
  }
  static bool HasReservedFunction(LayoutSpan name) {
    return GetReservedFunction(name) != nullptr;
  }
  static bool HasSpecialIdentifier(LayoutSpan name) {
    return GetSpecialIdentifier(name) != nullptr;
  }
  static bool HasSpecialIdentifier(EnabledType type) {
    return GetSpecialIdentifier(type) != nullptr;
  }
  static bool HasCustomIdentifier(LayoutSpan name);
  static const Builtin* GetReservedFunction(LayoutSpan name);
  static const Builtin* GetReservedFunction(const Tree* e);
  static constexpr const Builtin* GetReservedFunction(EnabledType type);
  static const Builtin* GetSpecialIdentifier(LayoutSpan name);
  static const Builtin* GetSpecialIdentifier(EnabledType type);

 private:
  EnabledType m_type;
  Aliases m_aliases;
};

class BuiltinWithLayout : public Builtin {
 public:
  constexpr BuiltinWithLayout(EnabledType type, Aliases aliases,
                              LayoutAnyType layoutType)
      : Builtin(type, aliases), m_layoutType(layoutType) {
    assert(TypeBlock::NumberOfMetaBlocks(
               static_cast<EnabledType>(layoutType)) == 1);
  }
  LayoutAnyType layoutType() const { return m_layoutType; }
  bool has2DLayout() const override { return true; }

  static constexpr const BuiltinWithLayout* GetReservedFunction(
      LayoutAnyType layoutType);

 private:
  LayoutAnyType m_layoutType;
};

class BuiltinAns : public Builtin {
  using Builtin::Builtin;
  bool checkNumberOfParameters(int n) const override { return n == 0; }
  Tree* pushNode(int numberOfChildren) const override;
};

namespace BuiltinsAliases {
// Special identifiers
constexpr static Aliases k_undefinedAlias = "undef";
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
constexpr static Aliases k_ohmAliases = "\01Ω\00ohm\00Ohm\00";
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
    {Type::LogBase, "log"},
    {Type::Log, "log"},
    {Type::LnUser, "ln"},
#if POINCARE_MATRIX
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
#endif
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
#if POINCARE_LIST
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
#endif
    {Type::Random, "random"},
    {Type::RandInt, "randint"},
    {Type::RandIntNoRep, "randintnorep"},
    {Type::Diff, "diff"},  // 2D layout is special
#if POINCARE_PIECEWISE
    {Type::Piecewise, "piecewise"},  // 2D layout is a grid
#endif
    {Type::Dep, "dep"},          // TODO shouldn't be user parsable
    {Type::NonNull, "nonNull"},  // TODO shouldn't be user parsable
    {Type::Real, "real"},        // TODO shouldn't be user parsable
    {Type::RealPos, "realPos"},  // TODO shouldn't be user parsable
};

constexpr static BuiltinWithLayout s_builtinsWithLayout[] = {
    {Type::Abs, "abs", LayoutType::Abs},
    {Type::Binomial, "binomial", LayoutType::Binomial},
    {Type::Sum, "sum", LayoutType::Sum},
    {Type::Product, "product", LayoutType::Product},
    {Type::Integral, "int", LayoutType::Integral},
    {Type::Sqrt, BuiltinsAliases::k_squareRootAliases, LayoutType::Sqrt},
    {Type::Root, "root", LayoutType::Root},
#if POINCARE_MATRIX
    {Type::Norm, "norm", LayoutType::VectorNorm},
#endif
    {Type::Conj, "conj", LayoutType::Conj},
    {Type::Ceil, "ceil", LayoutType::Ceil},
    {Type::Floor, "floor", LayoutType::Floor},
#if POINCARE_LIST
    {Type::ListSequence, "sequence", LayoutType::ListSequence},
#endif
};

constexpr const Builtin* Builtin::GetReservedFunction(EnabledType type) {
  for (const Builtin& builtin : s_builtins) {
    if (builtin.m_type == type) {
      return &builtin;
    }
  }
  for (const Builtin& builtin : s_builtinsWithLayout) {
    if (builtin.m_type == type) {
      return &builtin;
    }
  }
  return nullptr;
}

constexpr const BuiltinWithLayout* BuiltinWithLayout::GetReservedFunction(
    LayoutAnyType LayoutType) {
  for (const BuiltinWithLayout& builtin : s_builtinsWithLayout) {
    if (builtin.m_layoutType == LayoutType) {
      return &builtin;
    }
  }
  return nullptr;
}

}  // namespace Poincare::Internal

#endif
