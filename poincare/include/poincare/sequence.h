#ifndef POINCARE_SEQUENCE_H
#define POINCARE_SEQUENCE_H

#include <poincare/symbol_abstract.h>

namespace Poincare {

class SequenceNode : public SymbolAbstractNode {
 public:
  SequenceNode(const char* newName, int length);
  const char* name() const override { return m_name; }

  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Sequence";
  }
#endif

  Type type() const override { return Type::Sequence; }
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;

 private:
  char m_name[0];

  size_t nodeSize() const override { return sizeof(SequenceNode); }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;

  // Name
  char* editableName() override { return m_name; }

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return strlen(m_name) > 1 ? LayoutShape::MoreLetters
                              : LayoutShape::OneLetter;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override;
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class Sequence : public SymbolAbstract {
  friend SequenceNode;

 public:
  Sequence(const SequenceNode* n) : SymbolAbstract(n) {}
  static Sequence Builder(const char* name, size_t length,
                          Expression child = Expression());

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext);
  Expression deepReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount, SymbolicComputation symbolicComputation) {
    return *this;
  }
};

}  // namespace Poincare

#endif
