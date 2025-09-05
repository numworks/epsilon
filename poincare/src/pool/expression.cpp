#include <poincare/expression.h>
#include <poincare/src/expression/binary.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/infinity.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/vector.h>

namespace Poincare {

using namespace Internal;

/* Dimension */

static DimensionType toDT(uint8_t type) {
  assert(type <= static_cast<uint8_t>(DimensionType::None));
  return static_cast<DimensionType>(type);
}

Poincare::Dimension::Dimension(const Expression e, const Context& context)
    : m_matrixDimensionRows(0),
      m_matrixDimensionCols(0),
      m_type(static_cast<uint8_t>(DimensionType::Scalar)),
      m_isList(false),
      m_isValid(false),
      m_isEmptyList(false) {
  static_assert(sizeof(DimensionType) == sizeof(m_type));
  static_assert(sizeof(MatrixDimension) ==
                sizeof(m_matrixDimensionCols) + sizeof(m_matrixDimensionRows));
  // TODO_PCJ: Remove checks in SystemExpression implementation of this
  if (!Internal::Dimension::DeepCheck(e.tree(), context)) {
    return;
  }
  Internal::Dimension dimension = Internal::Dimension::Get(e.tree(), context);
  m_type = static_cast<uint8_t>(dimension.type);
  m_isList = Internal::Dimension::IsList(e.tree(), context);
  m_isEmptyList = (m_isList && Internal::Dimension::ListLength(e.tree()) == 0);
  if (toDT(m_type) == DimensionType::Matrix) {
    m_matrixDimensionRows = dimension.matrix.rows;
    m_matrixDimensionCols = dimension.matrix.cols;
  }
  m_isValid = true;
}

bool Poincare::Dimension::isScalar() {
  return m_isValid && !m_isList && toDT(m_type) == DimensionType::Scalar;
}

bool Poincare::Dimension::isMatrix() {
  return m_isValid && !m_isList && toDT(m_type) == DimensionType::Matrix;
}

bool Poincare::Dimension::isVector() {
  return isMatrix() &&
         (m_matrixDimensionRows == 1 || m_matrixDimensionCols == 1);
}

bool Poincare::Dimension::isUnit() {
  return m_isValid && !m_isList && toDT(m_type) == DimensionType::Unit;
}

bool Poincare::Dimension::isBoolean() {
  return m_isValid && !m_isList && toDT(m_type) == DimensionType::Boolean;
}

bool Poincare::Dimension::isPoint() {
  /* TODO_PCJ: This method used to allow (undef, x) with x undefined. Restore
   * this behavior ? */
  return m_isValid && !m_isList && toDT(m_type) == DimensionType::Point;
}

bool Poincare::Dimension::isList() { return m_isValid && m_isList; }

bool Poincare::Dimension::isListOfScalars() {
  return m_isValid && m_isList && toDT(m_type) == DimensionType::Scalar;
}

bool Poincare::Dimension::isListOfUnits() {
  return m_isValid && m_isList && toDT(m_type) == DimensionType::Unit;
}

bool Poincare::Dimension::isListOfBooleans() {
  return m_isValid && m_isList && toDT(m_type) == DimensionType::Boolean;
}

bool Poincare::Dimension::isListOfPoints() {
  return m_isValid && m_isList && toDT(m_type) == DimensionType::Point;
}

bool Poincare::Dimension::isPointOrListOfPoints() {
  return m_isValid && toDT(m_type) == DimensionType::Point;
}

bool Poincare::Dimension::isEmptyList() { return m_isEmptyList; }

/* ExpressionObject */

ExpressionObject::ExpressionObject(const Tree* tree, size_t treeSize) {
  memcpy(m_blocks, tree->block(), treeSize);
}

size_t ExpressionObject::size() const {
  return sizeof(ExpressionObject) + tree()->treeSize();
}

#if POINCARE_TREE_LOG
void ExpressionObject::logAttributes(std::ostream& stream) const {
  stream << '\n';
  tree()->log(stream);
}
#endif

const Tree* ExpressionObject::tree() const {
  return Tree::FromBlocks(m_blocks);
}

/* Expression */

bool Expression::isIdenticalTo(const Expression e) const {
  return tree()->treeIsIdenticalTo(e.tree());
}

Expression Expression::ExpressionFromAddress(const void* address, size_t size) {
  if (address == nullptr || size == 0) {
    return Expression();
  }
  // Build the OExpression in the Tree Pool
  return Expression(static_cast<ExpressionObject*>(
      Pool::sharedPool->copyTreeFromAddress(address, size)));
}

const Tree* Expression::TreeFromAddress(const void* address) {
  if (address == nullptr) {
    return nullptr;
  }
  return reinterpret_cast<const ExpressionObject*>(address)->tree();
}

int Expression::numberOfDescendants(bool includeSelf) const {
  assert(tree());
  return tree()->numberOfDescendants(includeSelf);
}

Poincare::Dimension Expression::dimension(const Context& context) const {
  return Poincare::Dimension(*this, context);
}

bool Expression::isUndefined() const {
  // TODO_PCJ: this is terribly confusing. We should either:
  // - rename Expression::isUndefined() into something more specific
  // - create a Tree range for non-nonreal undefined
  return tree()->isUndefined() && !tree()->isNonReal();
}
bool Expression::isUndefinedOrNonReal() const { return tree()->isUndefined(); }

bool Expression::isMatrix() const { return tree()->isMatrix(); }

bool Expression::isOfMatrixDimension() const { return dimension().isMatrix(); }

bool Expression::isNAry() const { return tree()->isNAry(); }

bool Expression::isApproximate() const {
  return tree()->isDecimal() || tree()->isFloat() || tree()->isDoubleFloat();
}

bool Expression::isPlusOrMinusInfinity() const {
  return Internal::Infinity::IsPlusOrMinusInfinity(tree());
}

bool Expression::isPercent() const {
  return tree()->isPercentSimple() || tree()->isPercentAddition();
}

bool Expression::isBoolean() const { return tree()->isBoolean(); }

bool Expression::isList() const { return tree()->isList(); }

bool Expression::isUserSymbol() const { return tree()->isUserSymbol(); }

bool Expression::isUserFunction() const { return tree()->isUserFunction(); }

bool Expression::isStore() const { return tree()->isStore(); }

bool Expression::isFactor() const { return tree()->isFactor(); }

bool Expression::isPoint() const { return tree()->isPoint(); }

bool Expression::isNonReal() const { return tree()->isNonReal(); }

bool Expression::isOpposite() const { return tree()->isOpposite(); }

bool Expression::isDiv() const { return tree()->isDiv(); }

bool Expression::isBasedInteger() const {
  return tree()->isRational() && tree()->isInteger();
}

bool Expression::isDep() const { return tree()->isDep(); }

bool Expression::isComparison() const { return tree()->isComparison(); }

bool Expression::isEquality() const { return tree()->isEqual(); }

bool Expression::isRational() const { return tree()->isRational(); }

bool Expression::isPureAngleUnit() const {
  return !isUninitialized() && tree()->isUnit() &&
         Internal::Dimension::Get(tree()).isSimpleAngleUnit();
}

bool Expression::isVector() const {
  return !isUninitialized() && Vector::IsVector(tree());
}

bool Expression::isInRadians(const Context& context) const {
  return Internal::Dimension::Get(tree(), context).isSimpleRadianAngleUnit();
}

bool Expression::isConstantNumber() const {
  return !isUninitialized() && (tree()->isNumber() || tree()->isInf() ||
                                tree()->isUndefined() || tree()->isDecimal());
};

bool Expression::allChildrenAreUndefined() const {
  return !tree()->hasChildSatisfying(
      [](const Tree* e) { return !e->isUndefined(); });
}

bool Expression::hasRandomNumber() const {
  return tree()->hasDescendantSatisfying(
      [](const Tree* e) { return e->isRandom() || e->isRandInt(); });
}

bool Expression::hasRandomList() const {
  return tree()->hasDescendantSatisfying(
      [](const Tree* e) { return e->isRandIntNoRep(); });
}

int Expression::numberOfChildren() const { return tree()->numberOfChildren(); }

Comparison::Operator Expression::comparisonOperator() const {
  assert(isComparison());
  return Internal::Binary::ComparisonOperatorForType(tree()->type());
}

PoolHandle Expression::BuildPoolHandleFromTree(const Tree* tree) {
  if (!tree) {
    return Expression();
  }
  size_t size = tree->treeSize();
  void* bufferNode = Pool::sharedPool->alloc(sizeof(ExpressionObject) + size);
  ExpressionObject* node = new (bufferNode) ExpressionObject(tree, size);
  return PoolHandle::Build(node);
}

}  // namespace Poincare
