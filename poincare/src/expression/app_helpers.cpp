#include "app_helpers.h"

#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/memory/multiple_nodes_iterator.h>
#include <poincare/src/memory/tree.h>

#include "simplification.h"

using namespace Poincare::Internal;
namespace Poincare::AppHelpers {

bool ExactAndApproximateExpressionsAreEqual(const Tree* exact,
                                            const Tree* approximated) {
  assert(exact && approximated);
  assert(!approximated->isUndefined());
  assert(Simplification::IsSystem(exact) &&
         Simplification::IsSystem(approximated));

  /* Turn floats and doubles into decimal so that they can be compared to
   * rationals. */
  if (exact->isRational() && approximated->isFloat()) {
    /* Using parsing is the safest way to create a Decimal that will be the same
     * as what the user will read. */
    /* TODO: We need to be careful to the number of significants digits 1/16 ~=
     * 0.63 with 2 digits. But until now the app will call this with the
     * truncated float. */
    Tree* layout = Layouter::LayoutExpression(approximated->cloneTree());
    Tree* parsed = Parser::Parse(layout, nullptr);
    assert(parsed->isRationalOrFloat() || parsed->isDecimal() ||
           (parsed->isOpposite() && parsed->child(0)->isRationalOrFloat() ||
            parsed->child(0)->isDecimal()));
    ProjectionContext ctx{};
    Simplification::ProjectAndReduce(parsed, &ctx, false);
    bool result = exact->treeIsIdenticalTo(parsed);
    parsed->removeTree();
    layout->removeTree();
    return result;
  }

  if (exact->type() != approximated->type()) {
    return false;
  }

  /* Check deeply for equality, because the expression can be a list, a matrix
   * or a complex composed of rationals.
   * Ex: 1 + i/2 == 1 + 0.5i */
  if (exact->numberOfChildren() != approximated->numberOfChildren()) {
    return false;
  }
  const Tree* approxChild = approximated->nextNode();
  for (const Tree* exactChild : exact->children()) {
    if (!ExactAndApproximateExpressionsAreEqual(exactChild, approxChild)) {
      return false;
    }
    approxChild = approxChild->nextTree();
  }

  /* The node content is ignored but should not matter, unless exact can be a
   * float. */
  assert(exact->nodeIsIdenticalTo(approximated));
  return true;
}

}  // namespace Poincare::AppHelpers
