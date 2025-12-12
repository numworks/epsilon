#pragma once

#include <poincare/comparison_operator.h>
#include <poincare/pool_handle.h>
#include <poincare/pool_object.h>
#include <poincare/preferences.h>
#include <poincare/src/memory/block.h>
#include <poincare/symbol_context.h>

namespace Poincare {

class Expression;

class Dimension {
 public:
  Dimension(const Expression e,
            const SymbolContext& symbolContext = EmptySymbolContext{});

  bool isScalar();
  bool isMatrix();
  bool isVector();
  bool isUnit();
  bool isBoolean();
  bool isPoint();

  bool isList();
  bool isListOfScalars();
  bool isListOfUnits();
  bool isListOfBooleans();
  bool isListOfPoints();
  bool isEmptyList();

  bool isPointOrListOfPoints();

 private:
  /* MatrixDimension */ uint8_t m_matrixDimensionRows;
  /* MatrixDimension */ uint8_t m_matrixDimensionCols;
  /* DimensionType */ uint8_t m_type;
  bool m_isList;
  bool m_isValid;
  bool m_isEmptyList;
};

class ExpressionObject final : public PoolObject {
  friend class Expression;

 public:
  ExpressionObject(const Internal::Tree* tree, size_t treeSize);

  // PoolObject
  size_t size() const override;
#if POINCARE_TREE_LOG
  void logObjectName(std::ostream& stream) const override {
    stream << "Expression";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

 private:
  const Internal::Tree* tree() const;

  Internal::Block m_blocks[0];
};

class Expression : public PoolHandle {
  friend class ExpressionObject;

 public:
  Expression() : PoolHandle() {}
  Expression(const ExpressionObject* n) : PoolHandle(n) {}

  /* General properties */

  int numberOfDescendants(bool includeSelf) const;
  int numberOfChildren() const;
  // Comparing trees. Expressions could be of a different derived class.
  bool isIdenticalTo(const Expression e) const;

  /* Boolean properties */
  // TODO: Specialize these properties if relevant to a derived class.
  bool allChildrenAreUndefined() const;
  bool hasRandomList() const;
  bool hasRandomNumber() const;
  bool isApproximate() const;
  bool isBasedInteger() const;
  bool isBoolean() const;
  bool isComparison() const;
  // Return true if expression is a number, constant, inf or undef.
  bool isConstantNumber() const;
  bool isDep() const;
  bool isDiff() const;
  bool isDiv() const;
  bool isEquality() const;
  bool isFactor() const;
  bool isInRadians(const SymbolContext& symbolContext) const;
  bool isIntegral() const;
  bool isList() const;
  bool isMatrix() const;
  bool isNAry() const;
  bool isNonReal() const;
  bool isOfMatrixDimension() const;
  bool isOpposite() const;
  bool isPercent() const;
  bool isPlusOrMinusInfinity() const;
  bool isPoint() const;
  // Return true if Decimal, Integer or Opposite of those.
  bool isPureAngleUnit() const;
  bool isRational() const;
  bool isStore() const;
  bool isUndefined() const;
  bool isUndefinedOrNonReal() const;
  bool isUserFunction() const;
  bool isUserSymbol() const;
  bool isVector() const;
  bool isTrigonometryFunction() const;
  bool isParametric() const;

  /* Other properties */

  Dimension dimension(
      const SymbolContext& symbolContext = EmptySymbolContext{}) const;
  // Only called on expressions that are comparisons
  Comparison::Operator comparisonOperator() const;

  /* Tree manipulation */
  // TODO: These methods should be protected

  /* Get a Tree from the storage, more efficient and safer than
   * ExpressionFromAddress.tree() because it points to the storage directly. */
  static const Internal::Tree* TreeFromAddress(const void* address);
  operator const Internal::Tree*() const { return tree(); }
  const Internal::Tree* tree() const {
    return isUninitialized() ? nullptr : object()->tree();
  }

#if POINCARE_TREE_LOG
  void logSerialize(std::ostream& stream) const;
  __attribute__((__used__)) void logSerialize() const {
    logSerialize(std::cout);
  }
#endif

 protected:
  static Expression ExpressionFromAddress(const void* address, size_t size);
  static PoolHandle BuildPoolHandleFromTree(const Internal::Tree* tree);

 private:
  ExpressionObject* object() const {
    assert(identifier() != PoolObject::NoObjectIdentifier);
    return static_cast<ExpressionObject*>(PoolHandle::object());
  }
};

}  // namespace Poincare
