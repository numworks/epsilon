#include <poincare/junior_expression.h>
#include <poincare/junior_layout.h>
#include <poincare/matrix.h>
#include <poincare/point_2D_layout.h>
#include <poincare_junior/src/expression/conversion.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/rack_from_text.h>
#include <poincare_junior/src/n_ary.h>

namespace Poincare {

/* JuniorExpressionNode */

Layout JuniorExpressionNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  return JuniorLayout::Builder(PoincareJ::Layoutter::LayoutExpression(
      tree()->clone(), false, numberOfSignificantDigits, floatDisplayMode));
}

size_t JuniorExpressionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  OExpression e = PoincareJ::ToPoincareExpression(tree());
  return e.node()->serialize(buffer, bufferSize, floatDisplayMode,
                             numberOfSignificantDigits);
}

/* JuniorExpression */

JuniorExpression JuniorExpression::Parse(const PoincareJ::Tree* layout,
                                         Context* context,
                                         bool addMissingParenthesis,
                                         bool parseForAssignment) {
  // TODO_PCJ: Use context, addMissingParenthesis and parseForAssignment.
  return Builder(PoincareJ::Parser::Parse(layout));
}

JuniorExpression JuniorExpression::Parse(char const* string, Context* context,
                                         bool addMissingParenthesis,
                                         bool parseForAssignment) {
  if (string[0] == 0) {
    return JuniorExpression();
  }
  PoincareJ::Tree* layout = PoincareJ::RackFromText(string);
  if (!layout) {
    return JuniorExpression();
  }
  JuniorExpression result =
      Parse(layout, context, addMissingParenthesis, parseForAssignment);
  layout->removeTree();
  return result;
}

JuniorExpression JuniorExpression::Builder(const PoincareJ::Tree* tree) {
  if (!tree) {
    return JuniorExpression();
  }
  size_t size = tree->treeSize();
  void* bufferNode =
      TreePool::sharedPool->alloc(sizeof(JuniorExpressionNode) + size);
  JuniorExpressionNode* node =
      new (bufferNode) JuniorExpressionNode(tree, size);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<JuniorExpression&>(h);
}

JuniorExpression JuniorExpression::Builder(PoincareJ::Tree* tree) {
  JuniorExpression result = Builder(const_cast<const PoincareJ::Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

JuniorExpression JuniorExpression::Juniorize(OExpression e) {
  if (e.isUninitialized() ||
      e.type() == ExpressionNode::Type::JuniorExpression) {
    // e is already a junior expression
    return static_cast<JuniorExpression&>(e);
  }
  return Builder(PoincareJ::FromPoincareExpression(e));
}

JuniorExpression JuniorExpression::childAtIndex(int i) const {
  assert(tree());
  return Builder(tree()->child(i));
}

/* Matrix */

Matrix Matrix::Builder() {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedEditionPool->push<PoincareJ::BlockType::Matrix>(0, 0));
  return static_cast<Matrix&>(expr);
}

void Matrix::setDimensions(int rows, int columns) {
  assert(rows * columns == tree()->numberOfChildren());
  PoincareJ::Tree* clone = tree()->clone();
  PoincareJ::Matrix::SetNumberOfColumns(clone, columns);
  PoincareJ::Matrix::SetNumberOfRows(clone, rows);
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<Matrix&>(temp);
}

bool Matrix::isVector() const {
  const PoincareJ::Tree* t = tree();
  return PoincareJ::Matrix::NumberOfRows(t) == 1 ||
         PoincareJ::Matrix::NumberOfColumns(t) == 1;
}

int Matrix::numberOfRows() const {
  return PoincareJ::Matrix::NumberOfRows(tree());
}

int Matrix::numberOfColumns() const {
  return PoincareJ::Matrix::NumberOfColumns(tree());
}

// TODO_PCJ: Rework this and its usage
void Matrix::addChildAtIndexInPlace(JuniorExpression t, int index,
                                    int currentNumberOfChildren) {
  PoincareJ::Tree* clone = tree()->clone();
  PoincareJ::EditionReference newChild = t.tree()->clone();
  if (index >= clone->numberOfChildren()) {
    int rows = PoincareJ::Matrix::NumberOfRows(clone);
    int columns = PoincareJ::Matrix::NumberOfColumns(clone);
    for (int i = 1; i < columns; i++) {
      PoincareJ::KUndef->clone();
    }
    PoincareJ::Matrix::SetNumberOfRows(clone, rows + 1);
  } else {
    PoincareJ::Tree* previousChild = clone->child(index);
    previousChild->removeTree();
    newChild->moveTreeAtNode(previousChild);
  }
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<Matrix&>(temp);
}

JuniorExpression Matrix::matrixChild(int i, int j) {
  return JuniorExpression::Builder(PoincareJ::Matrix::Child(tree(), i, j));
}

int Matrix::rank(Context* context, bool forceCanonization) {
  if (!forceCanonization) {
    return PoincareJ::Matrix::Rank(tree());
  }
  PoincareJ::Tree* clone = tree()->clone();
  int result = PoincareJ::Matrix::CanonizeAndRank(clone);
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<Matrix&>(temp);
  return result;
}

// TODO_PCJ: Rework this and its usage
template <typename T>
int Matrix::ArrayInverse(T* array, int numberOfRows, int numberOfColumns) {
  return OMatrix::ArrayInverse(array, numberOfRows, numberOfColumns);
}

template int Matrix::ArrayInverse<double>(double*, int, int);
template int Matrix::ArrayInverse<std::complex<float>>(std::complex<float>*,
                                                       int, int);
template int Matrix::ArrayInverse<std::complex<double>>(std::complex<double>*,
                                                        int, int);

/* Point */

Point Point::Builder(JuniorExpression x, JuniorExpression y) {
  PoincareJ::Tree* tree = PoincareJ::KPoint->cloneNode();
  x.tree()->clone();
  y.tree()->clone();
  JuniorExpression temp = JuniorExpression::Builder(tree);
  return static_cast<Point&>(temp);
}

template <typename T>
Coordinate2D<T> Point::approximate2D(
    const ApproximationContext& approximationContext) {
  // TODO_PCJ: Add context for angle unit and complex format.
  return Coordinate2D<T>(
      PoincareJ::Approximation::RootTreeTo<T>(tree()->child(0)),
      PoincareJ::Approximation::RootTreeTo<T>(tree()->child(1)));
}

Layout Point::create2DLayout(Preferences::PrintFloatMode floatDisplayMode,
                             int significantDigits, Context* context) const {
  Layout child0 = childAtIndex(0).createLayout(floatDisplayMode,
                                               significantDigits, context);
  Layout child1 = childAtIndex(1).createLayout(floatDisplayMode,
                                               significantDigits, context);
  return JuniorLayout::Create(KPoint2DL(KA, KB), {.KA = child0, .KB = child1});
}

template Coordinate2D<float> Point::approximate2D<float>(
    const ApproximationContext& approximationContext);
template Coordinate2D<double> Point::approximate2D<double>(
    const ApproximationContext& approximationContext);

/* List */

List List::Builder() {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedEditionPool->push<PoincareJ::BlockType::List>(0));
  return static_cast<List&>(expr);
}

// TODO_PCJ: Rework this and its usage
void List::addChildAtIndexInPlace(JuniorExpression t, int index,
                                  int currentNumberOfChildren) {
  PoincareJ::Tree* clone = tree()->clone();
  PoincareJ::EditionReference newChild = t.tree()->clone();
  PoincareJ::NAry::SetNumberOfChildren(clone, clone->numberOfChildren() + 1);
  JuniorExpression temp = JuniorExpression::Builder(clone);
  *this = static_cast<List&>(temp);
}

}  // namespace Poincare
