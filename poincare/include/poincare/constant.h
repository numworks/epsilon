#ifndef POINCARE_CONSTANT_H
#define POINCARE_CONSTANT_H

#include <poincare/aliases_list.h>
#include <poincare/expression.h>
#include <poincare/expression_node.h>

namespace Poincare {

class ConstantNode final : public ExpressionNode {
 public:
  ConstantNode(const char* name, int length);

  const char* name() const { return constantInfo().m_aliasesList.mainAlias(); }

  // TreeNode
  size_t size() const override { return sizeof(ConstantNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Constant";
  }
  void logAttributes(std::ostream& stream) const override {
    stream << " name=\"" << name() << "\"";
  }
#endif

  // Complex
  bool isReal() const;

  // Expression Properties
  Type type() const override {
    return constantInfo().m_unit == nullptr ? Type::ConstantMaths
                                            : Type::ConstantPhysics;
  }
  TrinaryBoolean isPositive(Context* context) const override;
  TrinaryBoolean isNull(Context* context) const override {
    return TrinaryBoolean::False;
  }

  /* Layout */
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;

  /* Approximation */
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>();
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>();
  }

  constexpr static const char* k_exponentialEName = "e";
  constexpr static const char* k_complexIName = "i";

  /* Constant properties */
  struct ConstantInfo {
    AliasesList m_aliasesList;
    int m_comparisonRank;
    double m_value;
    const char* m_unit;
  };

  /* Some of these are currently not tested in simplification.cpp because
   * their units are weirdly simplified. These tests whould be updated when
   * the output units are updated. */
  constexpr static const ConstantInfo k_constants[] = {
      ConstantInfo{k_complexIName, 0, NAN, nullptr},
      ConstantInfo{AliasesLists::k_piAliases, 1, M_PI, nullptr},
      ConstantInfo{k_exponentialEName, 2, M_E, nullptr},
      ConstantInfo{"_c", 3, 299792458.0, "_m/_s"},
      ConstantInfo{"_e", 3, 1.602176634e-19, "_C"},
      ConstantInfo{"_G", 3, 6.67430e-11, "_m^3*_kg^-1*_s^-2"},
      ConstantInfo{"_g0", 3, 9.80665, "_m/_s^2"},
      ConstantInfo{"_k", 3, 1.380649e-23, "_J/_K"},
      ConstantInfo{"_ke", 3, 8.9875517923e9, "_N*_m^2/_C^2"},
      ConstantInfo{"_me", 3, 9.1093837015e-31, "_kg"},
      ConstantInfo{"_mn", 3, 1.67492749804e-27, "_kg"},
      ConstantInfo{"_mp", 3, 1.67262192369e-27, "_kg"},
      ConstantInfo{"_Na", 3, 6.02214076e23, "_mol^-1"},
      ConstantInfo{"_R", 3, 8.31446261815324, "_J*_mol^-1*_K^-1"},
      ConstantInfo{"_ε0", 3, 8.8541878128e-12, "_F/_m"},
      ConstantInfo{"_μ0", 3, 1.25663706212e-6, "_N*_A^-2"},
      /* "_hplanck" has the longest name. Modify the constexpr in
       * ConstantNode::createLayout if that's not the case anymore. */
      ConstantInfo{"_hplanck", 3, 6.62607015e-34, "_J*_s"}};
  constexpr static int k_numberOfConstants =
      sizeof(k_constants) / sizeof(ConstantInfo);

  const ConstantInfo constantInfo() const {
    assert(m_constantInfo);
    return *m_constantInfo;
  }

  bool isConstant(const char* constant) const;
  bool isPi() const {
    return isConstant(AliasesLists::k_piAliases.mainAlias());
  }
  bool isComplexI() const { return isConstant(k_complexIName); }
  bool isExponentialE() const { return isConstant(k_exponentialEName); }

  // Comparison
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::OneLetter;
  };

  /* Derivation */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) override;

 private:
  int rankOfConstant() const { return constantInfo().m_comparisonRank; }
  template <typename T>
  Evaluation<T> templatedApproximate() const;

  const ConstantInfo* m_constantInfo;
};

class Constant final : public Expression {
  friend class ConstantNode;

 public:
  Constant(const ConstantNode* node) : Expression(node) {}

  static Constant Builder(const char* name, int length);
  static Constant Builder(const char* name) {
    return Builder(name, strlen(name));
  }

  static Constant ExponentialEBuilder() {
    return Builder(ConstantNode::k_exponentialEName);
  }
  static Constant ComplexIBuilder() {
    return Builder(ConstantNode::k_complexIName);
  }
  static Constant PiBuilder() {
    return Builder(AliasesLists::k_piAliases.mainAlias());
  }

  // Constant properties
  static bool IsConstant(const char* name, size_t length) {
    return ConstantInfoIndexFromName(name, length) >= 0;
  }

  bool isPi() const { return node()->isPi(); }
  bool isComplexI() const { return node()->isComplexI(); }
  bool isExponentialE() const { return node()->isExponentialE(); }

  ConstantNode::ConstantInfo constantInfo() const {
    return node()->constantInfo();
  }

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue);

 private:
  // Return -1 if the name does not refer to a constant
  static int ConstantInfoIndexFromName(const char* name, int length);
  ConstantNode* node() const {
    return static_cast<ConstantNode*>(Expression::node());
  }
};

}  // namespace Poincare

#endif
